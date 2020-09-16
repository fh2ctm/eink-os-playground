#include "UpdatePureOS.hpp"
#if defined(TARGET_RT1051)
#include "board/cross/eMMC/eMMC.hpp"
#endif
#include <service-desktop/ServiceDesktop.hpp>
#include <service-gui/ServiceGUI.hpp>
#include <math.h>
#include <module-apps/application-desktop/ApplicationDesktop.hpp>

FileInfo::FileInfo(mtar_header_t &h, unsigned long crc32) : fileSize(h.size), fileCRC32(crc32)
{
    if (h.name[0] == '.' && h.name[1] == '/') {
        // microtar relative paths, strip the leading ./away
        fileName = std::string(h.name).substr(2);
    }
    else {
        fileName = std::string(h.name);
    }
}

json11::Json FileInfo::to_json() const
{
    return json11::Json::object{
        {"name", fileName}, {"size", std::to_string(fileSize)}, {"crc32", std::to_string(fileCRC32)}};
}

UpdatePureOS::UpdatePureOS(ServiceDesktop *ownerService) : owner(ownerService)
{}

updateos::UpdateError UpdatePureOS::setUpdateFile(fs::path updateFileToUse)
{
    updateFile = purefs::dir::os_updates / updateFileToUse;
    if (vfs.fileExists(updateFile.c_str())) {
        versioInformation = UpdatePureOS::getVersionInfoFromFile(updateFile);
        if (mtar_open(&updateTar, updateFile.c_str(), "r") == MTAR_ESUCCESS) {

            totalBytes = vfs.filelength(updateTar.stream);
            informUpdate("UpdatePureOS::setUpdateFile TAR_FILE: %s opened", updateFile.c_str());
        }
        else {
            informError("UpdatePureOS::setUpdateFile can't open TAR file %s", updateFile.c_str());
            return updateos::UpdateError::CantOpenUpdateFile;
        }
    }
    else {
        informError("UpdatePureOS::setUpdateFile %s does not exist", updateFile.c_str());
        return updateos::UpdateError::CantOpenUpdateFile;
    }
    return updateos::UpdateError::NoError;
}

updateos::UpdateError UpdatePureOS::runUpdate()
{
    informDebug("runUpdate updateFile:%s", updateFile.c_str());

    updateos::UpdateError err = prepareTempDirForUpdate();
    if (err != updateos::UpdateError::NoError) {
        informError("runUpdate can't prepare temp directory for update");
        return err;
    }

    if ((err = unpackUpdate()) == updateos::UpdateError::NoError) {
        informUpdate("runUpdate %s unpacked", updateFile.c_str());
    }
    else {
        informError("runUpdate %s can't be unpacked", updateFile.c_str());
        return err;
    }

    if ((err = verifyChecksums()) == updateos::UpdateError::NoError) {
        informUpdate("runUpdate %s verifyChecksums success", updateFile.c_str());
    }
    else {
        informError("runUpdate %s checksum verification failed", updateFile.c_str());
        return err;
    }

    if ((err = verifyVersion()) == updateos::UpdateError::NoError) {
        informUpdate("runUpdate %s verifyVersion success", updateFile.c_str());
    }
    else {
        informError("runUpdate %s version verification failed", updateFile.c_str());
        return err;
    }

    if ((err = updateBootloader()) == updateos::UpdateError::NoError) {
        informUpdate("runUpdate %s updateBootloader success", updateFile.c_str());
    }
    else {
        informError("runUpdate %s version updateBootloader failed", updateFile.c_str());
        return err;
    }

    if ((err = prepareRoot()) == updateos::UpdateError::NoError) {
        informUpdate("runUpdate %s root ready for reset", updateFile.c_str());
    }
    else {
        informError("runUpdate %s can't prepare root dir for reset", updateFile.c_str());
    }

    return cleanupAfterUpdate();
}

updateos::UpdateError UpdatePureOS::unpackUpdate()
{
    mtar_header_t tarHeader;
    filesInUpdatePackage.clear();

    while ((mtar_read_header(&updateTar, &tarHeader)) != MTAR_ENULLRECORD) {
        unsigned long fileCRC32 = 0;
        informUpdate("unpackUpdate: reading tar header name %s...", tarHeader.name);

        if (tarHeader.type == MTAR_TDIR) {
            fs::path tmpPath = getUpdateTmpChild(tarHeader.name);
            if (vfs.mkdir(tmpPath.c_str()) != 0) {
                informError("unpackUpdate failed to create %s when extracting update tar", tmpPath.c_str());
                return (updateos::UpdateError::CantCreateExtractedFile);
            }
        }
        else {
            if (unpackFileToTemp(tarHeader, &fileCRC32) == false) {
                informError("unpackUpdate failed to extract update file %s", tarHeader.name);
                return (updateos::UpdateError::CantCreateExtractedFile);
            }

            filesInUpdatePackage.emplace_back(FileInfo(tarHeader, fileCRC32));
        }

        mtar_next(&updateTar);
    }

    informUpdate("unpackUpdate last header: %s file count: %d", tarHeader.name, filesInUpdatePackage.size());
    return updateos::UpdateError::NoError;
}

updateos::UpdateError UpdatePureOS::verifyChecksums()
{
    std::unique_ptr<char[]> lineBuff(
        new char[purefs::buffer::tar_buf]); // max line should be freertos max path + checksum, so this is enough
    fs::path checksumsFile = getUpdateTmpChild(updateos::file::checksums);
    vfs::FILE *fpChecksums = vfs.fopen(checksumsFile.c_str(), "r");

    if (fpChecksums == nullptr) {
        informError("verifyChecksums can't open checksums file %s", checksumsFile.c_str());
        return updateos::UpdateError::CantOpenChecksumsFile;
    }

    while (!vfs.eof(fpChecksums)) {
        char *line = vfs.fgets(lineBuff.get(), purefs::buffer::tar_buf, fpChecksums);
        std::string filePath;
        unsigned long fileCRC32;

        if (lineBuff[0] == ';' || line == nullptr)
            continue;

        getChecksumInfo(line, filePath, &fileCRC32);
        unsigned long computedCRC32 = getExtractedFileCRC32(filePath);
        if (computedCRC32 != fileCRC32) {
            informError("verifyChecksums %s crc32 match FAIL %lX != %lX", filePath.c_str(), fileCRC32, computedCRC32);
            vfs.fclose(fpChecksums);
            return updateos::UpdateError::VerifyChecksumsFailure;
        }
    }
    vfs.fclose(fpChecksums);

    informDebug("verifyChecksums noError");
    return updateos::UpdateError::NoError;
}

updateos::UpdateError UpdatePureOS::verifyVersion()
{
    informDebug("verifyVersion noError");
    if (!vfs.fileExists(getUpdateTmpChild(updateos::file::version).c_str())) {
        informError("verifyVersion %s does not exist", getUpdateTmpChild(updateos::file::version).c_str());
        return updateos::UpdateError::VerifyVersionFailure;
    }

    std::string versionJsonString = vfs.loadFileAsString(getUpdateTmpChild(updateos::file::version));
    std::string parserError;
    json11::Json updateVersionInformation = json11::Json::parse(versionJsonString, parserError);
    if (parserError != "") {
        informUpdate("verifyVersion parse json error: %s", parserError.c_str());
        return updateos::UpdateError::VerifyVersionFailure;
    }
    else {
    }
    return updateos::UpdateError::NoError;
}

updateos::UpdateError UpdatePureOS::updateBootloader()
{
    informDebug("updateBootloader noError");
    return updateos::UpdateError::NoError;
}

unsigned long UpdatePureOS::getExtractedFileCRC32(const std::string &filePath)
{
    for (auto file : filesInUpdatePackage) {
        if (file.fileName == filePath) {
            return file.fileCRC32;
        }
    }
    return 0;
}

void UpdatePureOS::getChecksumInfo(const std::string &infoLine, std::string &filePath, unsigned long *fileCRC32Long)
{
    std::size_t lastSpacePos = infoLine.find_last_of(' ');
    if (lastSpacePos > 0) {
        filePath                       = infoLine.substr(0, lastSpacePos);
        const std::string fileCRC32Str = infoLine.substr(lastSpacePos + 1, purefs::buffer::crc_char_size - 1);
        if (fileCRC32Long != nullptr) {
            *fileCRC32Long = strtoull(fileCRC32Str.c_str(), nullptr, purefs::buffer::crc_radix);
            informDebug("getChecksumInfo filePath: %s fileCRC32Str: %s fileCRC32Long: %lu fileCRC32Hex: %lX",
                      filePath.c_str(),
                      fileCRC32Str.c_str(),
                      *fileCRC32Long,
                      *fileCRC32Long);
        }
    }
}

updateos::UpdateError UpdatePureOS::prepareRoot()
{
    informDebug("prepareRoot()");
    int ret;
    // basic needed dirs

    informDebug("prepareRoot mkdir: %s", purefs::dir::os_previous.c_str());
    vfs.mkdir(purefs::dir::os_previous.c_str());
    informDebug("prepareRoot mkdir: %s", purefs::dir::os_current.c_str());
    vfs.mkdir(purefs::dir::os_current.c_str());
    informDebug("prepareRoot mkdir: %s", purefs::dir::user_disk.c_str());
    vfs.mkdir(purefs::dir::user_disk.c_str());

    // remove the previous OS version from partition
    informDebug("prepareRoot deltree: %s", purefs::dir::os_previous.c_str());
    ret = vfs.deltree(purefs::dir::os_previous.c_str());

    if (ret != 0) {
        informError("prepareRoot ff_deltree on %s caused an error %s",
                  purefs::dir::os_previous.c_str(),
                  vfs.lastErrnoToStr().c_str());
    }

    if (vfs.isDir(purefs::dir::os_previous.c_str())) {
        informError("prepareRoot %s still exists, we can't continue", purefs::dir::os_previous.c_str());
        return updateos::UpdateError::CantDeletePreviousOS;
    }
    // rename the current OS to previous on partition
    informDebug("prepareRoot rename: %s->%s", purefs::dir::os_current.c_str(), purefs::dir::os_previous.c_str());
    ret = vfs.rename(purefs::dir::os_current.c_str(), purefs::dir::os_previous.c_str());

    if (ret != 0) {
        informError("prepareRoot can't rename %s -> %s error %s",
                  purefs::dir::os_current.c_str(),
                  purefs::dir::os_previous.c_str(),
                  vfs.lastErrnoToStr().c_str());
        return updateos::UpdateError::CantRenameCurrentToPrevious;
    }

    // rename the temp directory to current (extracted update)
    informDebug("prepareRoot rename: %s->%s", updateTempDirectory.c_str(), purefs::dir::os_current.c_str());
    ret = vfs.rename(updateTempDirectory.c_str(), purefs::dir::os_current.c_str());

    if (ret != 0) {
        informError("prepareRoot can't rename %s -> %s error %s",
                  updateTempDirectory.c_str(),
                  purefs::dir::os_current.c_str(),
                  vfs.lastErrnoToStr().c_str());
        return updateos::UpdateError::CantRenameTempToCurrent;
    }

    // move the contents of /sys/current/user if it exists to /user
    ret = (int)updateUserData();

    return updateBootJSON();
}

updateos::UpdateError UpdatePureOS::updateBootJSON()
{
    unsigned long bootJSONAbsoulteCRC = 0;
    fs::path bootJSONAbsoulte         = purefs::dir::eMMC_disk / purefs::file::boot_json;
    informDebug("updateBootJSON %s", bootJSONAbsoulte.c_str());

    vfs::FILE *fp = vfs.fopen(bootJSONAbsoulte.c_str(), "r");

    if (fp != nullptr) {
        vfs.computeCRC32(fp, &bootJSONAbsoulteCRC);
        bootJSONAbsoulte += purefs::extension::crc32;

        vfs::FILE *fpCRC = vfs.fopen(bootJSONAbsoulte.c_str(), "w");
        if (fpCRC != nullptr) {
            std::array<char, purefs::buffer::crc_char_size> crcBuf;
            sprintf(crcBuf.data(), "%08lX", bootJSONAbsoulteCRC);
            vfs.fwrite(crcBuf.data(), 1, purefs::buffer::crc_char_size, fpCRC);
            vfs.fclose(fpCRC);
        }
        else {
            return updateos::UpdateError::CantUpdateCRC32JSON;
        }

        vfs.fclose(fp);
    }
    else {
        return updateos::UpdateError::CantUpdateCRC32JSON;
    }

    informDebug("updateBootJSON no error");
    return updateos::UpdateError::NoError;
}

bool UpdatePureOS::unpackFileToTemp(mtar_header_t &h, unsigned long *crc32)
{
    std::unique_ptr<unsigned char[]> readBuf(new unsigned char[purefs::buffer::tar_buf]);
    const fs::path fullPath = getUpdateTmpChild(h.name);

    uint32_t blocksToRead = (h.size / purefs::buffer::tar_buf) + 1;
    uint32_t sizeToRead   = purefs::buffer::tar_buf;

    informDebug("unpackFileToTemp %s blocksToRead %u sizeToRead %u",
              fullPath.c_str(),
              (unsigned int)blocksToRead,
              (unsigned int)sizeToRead);

    if (crc32 != nullptr) {
        *crc32 = 0;
    }
    else {
        return false;
    }

    int errCode   = MTAR_ESUCCESS;
    vfs::FILE *fp = vfs.fopen(fullPath.c_str(), "w+");
    if (fp == nullptr) {
        informError("unpackFileToTemp %s can't open for writing", fullPath.c_str());
        return false;
    }

    for (uint32_t i = 0; i < blocksToRead; i++) {
        if (i + 1 == blocksToRead) {
            sizeToRead = h.size % purefs::buffer::tar_buf;
        }
        else {
            sizeToRead = purefs::buffer::tar_buf;
        }

        if (sizeToRead == 0)
            break;

        if ((errCode = mtar_read_data(&updateTar, readBuf.get(), sizeToRead)) != MTAR_ESUCCESS) {
            informError("unpackFileToTemp mtar_read_data failed, errCode=%d", errCode);
            return false;
        }

        const uint32_t dataWritten = vfs.fwrite(readBuf.get(), 1, sizeToRead, fp);
        if (dataWritten != sizeToRead) {
            informError(
                "unpackFileToTemp %s can't write to file error: %s", fullPath.c_str(), vfs.lastErrnoToStr().c_str());
            vfs.fclose(fp);
            return false;
        }

        currentExtractedBytes += dataWritten;

        *crc32 = Crc32_ComputeBuf(*crc32, readBuf.get(), sizeToRead);
    }
    vfs.fclose(fp);
    return true;
}

updateos::UpdateError UpdatePureOS::cleanupAfterUpdate()
{
    if (vfs.isDir(updateTempDirectory.c_str()) && vfs.deltree(updateTempDirectory.c_str())) {
        informError("ff_deltree failed on %s", updateTempDirectory.c_str());
        return updateos::UpdateError::CantRemoveUniqueTmpDir;
    }
    return updateos::UpdateError::NoError;
}

const fs::path UpdatePureOS::getUpdateTmpChild(const fs::path &childPath)
{
    return updateTempDirectory / childPath;
}

updateos::UpdateError UpdatePureOS::prepareTempDirForUpdate()
{
    updateTempDirectory = purefs::dir::tmp / vfs::generateRandomId(updateos::prefix_len);

    informDebug("prepareTempDirForUpdate %s", updateTempDirectory.c_str());

    if (vfs.isDir(purefs::dir::os_updates.c_str()) == false) {
        informDebug("prepareTempDirForUpdate %s is not a directory", purefs::dir::os_updates.c_str());
        if (vfs.mkdir(purefs::dir::os_updates.c_str()) != 0) {
            informError("%s can't create it %s", purefs::dir::os_updates.c_str(), vfs.lastErrnoToStr().c_str());
            return updateos::UpdateError::CantCreateUpdatesDir;
        }
        else {
            informDebug("prepareTempDirForUpdate %s created", purefs::dir::os_updates.c_str());
        }
    }
    else {
        informDebug("prepareTempDirForUpdate %s exists", purefs::dir::os_updates.c_str());
    }

    if (vfs.isDir(purefs::dir::tmp.c_str()) == false) {
        informDebug("prepareTempDirForUpdate %s is not a directory", purefs::dir::tmp.c_str());
        if (vfs.mkdir(purefs::dir::tmp.c_str()) != 0) {
            informError("%s can't create it %s", purefs::dir::tmp.c_str(), vfs.lastErrnoToStr().c_str());
            return updateos::UpdateError::CantCreateTempDir;
        }
        else {
            informDebug("prepareTempDirForUpdate %s created", purefs::dir::tmp.c_str());
        }
    }
    else {
        informDebug("prepareTempDirForUpdate %s exists", purefs::dir::tmp.c_str());
    }

    if (vfs.isDir(updateTempDirectory.c_str())) {
        informDebug("prepareTempDirForUpdate %s exists already, try to remove it", updateTempDirectory.c_str());
        if (vfs.deltree(updateTempDirectory.c_str()) != 0) {
            informError("prepareTempDirForUpdate can't remove %s", updateTempDirectory.c_str());
            return updateos::UpdateError::CantRemoveUniqueTmpDir;
        }
        else {
            informDebug("prepareTempDirForUpdate %s removed", updateTempDirectory.c_str());
        }
    }

    informDebug("prepareTempDirForUpdate trying to create %s as tempDir", updateTempDirectory.c_str());
    if (vfs.mkdir(updateTempDirectory.c_str()) != 0) {
        informError("prepareTempDirForUpdate failed to create: %s error: %s",
                  updateTempDirectory.c_str(),
                  vfs.lastErrnoToStr().c_str());
        return updateos::UpdateError::CantCreateUniqueTmpDir;
    }

    informUpdate("prepareTempDirForUpdate tempDir selected %s", updateTempDirectory.c_str());
    return updateos::UpdateError::NoError;
}

updateos::BootloaderUpdateError UpdatePureOS::writeBootloader(fs::path bootloaderFile)
{

#if defined(TARGET_Linux)
    return updateos::BootloaderUpdateError::NoError;
#else

    if (vfs.fileExists(bootloaderFile.c_str()) == false) {
        informError("[Bootloader Update] File %s doesn't exist!\n", bootloaderFile.c_str());
        return updateos::BootloaderUpdateError::NoBootloaderFile;
    }

    auto fileHandler = vfs.fopen(bootloaderFile.c_str(), "r");
    if (fileHandler == nullptr) {
        informError("[Bootloader Update] Failed to open file %s\n", bootloaderFile.c_str());
        return updateos::BootloaderUpdateError::CantOpenBootloaderFile;
    }

    unsigned long fileLen = vfs.filelength(fileHandler);
    auto fileBuf          = std::make_unique<uint8_t[]>(fileLen);
    if (fileBuf == nullptr) {
        informError("[Bootloader Update] Failed to allocate buffer\n");
        return updateos::BootloaderUpdateError::CantAllocateBuffer;
    }

    auto filesLoaded = vfs.fread(fileBuf.get(), fileLen, 1, fileHandler);
    if (filesLoaded == 0) {
        informError("[Bootloader Update] Failed to load file %s\n", bootloaderFile.c_str());
        return updateos::BootloaderUpdateError::CantOpenBootloaderFile;
    }

    informUpdate("[Bootloader Update] File size: %lu B, Writing...", fileLen);

    bsp::eMMC emmc;
    emmc.Init();
    emmc.SwitchPartition(bsp::eMMC::Partition::Boot1);
    emmc.WriteBlocks(fileBuf.get(), 0, std::ceil(fileLen / FSL_SDMMC_DEFAULT_BLOCK_SIZE));

    informUpdate("[Bootloader Update] DONE!\n");
    emmc.SwitchPartition(bsp::eMMC::Partition::UserArea);

    return updateos::BootloaderUpdateError::NoError;
#endif
}

const json11::Json UpdatePureOS::getVersionInfoFromFile(const fs::path &updateFile)
{
    if (vfs.fileExists(updateFile.c_str())) {
        mtar_t tar;
        mtar_header_t h;

        if (mtar_open(&tar, updateFile.c_str(), "r") == MTAR_EOPENFAIL) {
            LOG_INFO("UpdatePureOS::getVersionInfoFromFile %s can't open", updateFile.c_str());
            return json11::Json();
        }

        std::unique_ptr<char[]> versionFilename(new char[purefs::buffer::crc_buf]);
        sprintf(versionFilename.get(), "./%s", updateos::file::version.c_str());
        if (mtar_find(&tar, versionFilename.get(), &h) == MTAR_ENOTFOUND) {
            LOG_INFO("UpdatePureOS::getVersionInfoFromFile can't find %s in %s",
                     updateos::file::version.c_str(),
                     updateFile.c_str());

            mtar_close(&tar);
            return json11::Json();
        }

        /* this file should never be larger then purefs::buffer::tar_buf */
        std::unique_ptr<unsigned char[]> readBuf(new unsigned char[purefs::buffer::tar_buf]);
        if (mtar_read_data(&tar, readBuf.get(), h.size) != MTAR_ESUCCESS) {
            LOG_INFO("UpdatePureOS::getVersionInfoFromFile can't read %s in %s",
                     updateos::file::version.c_str(),
                     updateFile.c_str());

            mtar_close(&tar);
            return json11::Json();
        }

        mtar_close(&tar);

        std::string parserError;
        json11::Json versionInfo = json11::Json::parse(std::string((const char *)readBuf.get(), h.size), parserError);
        if (parserError != "") {
            LOG_INFO("UpdatePureOS::getVersionInfoFromFile can't parse %s as JSON error: \"%s\"",
                     updateos::file::version.c_str(),
                     parserError.c_str());
            return json11::Json();
        }

        return versionInfo;
    }
    else {
        LOG_INFO("UpdatePureOS::getVersionInfoFromFile %s does not exist", updateFile.c_str());
    }

    return json11::Json();
}

bool UpdatePureOS::isUpgradeToCurrent(const std::string &versionToCompare)
{
    return true;
}

const fs::path UpdatePureOS::checkForUpdate()
{
    LOG_INFO("checkForUpdate %s", purefs::dir::os_updates.c_str());
    std::vector<vfs::DirectoryEntry> fileList = vfs.listdir(purefs::dir::os_updates.c_str(), updateos::extension::update, true);
    for (auto &file : fileList) {
        LOG_INFO("\t%s:%lu", file.fileName.c_str(), file.fileSize);
        json11::Json versionInfo = UpdatePureOS::getVersionInfoFromFile(purefs::dir::os_updates / file.fileName);
        if (versionInfo.is_null())
            continue;

        LOG_INFO("\t%s", versionInfo.dump().c_str());

        if (versionInfo[purefs::json::os_version][purefs::json::version_string].is_string()) {
            LOG_INFO("\t%slooks like a vlid update image, see if it's an update to us", file.fileName.c_str());

            if (UpdatePureOS::isUpgradeToCurrent(versionInfo[purefs::json::os_version][purefs::json::version_string].string_value())) {
                return purefs::dir::os_updates / file.fileName;
            }
        }
    }

    return fs::path();
}

updateos::UpdateError UpdatePureOS::updateUserData()
{
    informDebug("updateUserData filesToCheck %d", filesInUpdatePackage.size());
    for (unsigned int x=0; x < filesInUpdatePackage.size(); x++) {
        informDebug("file %u: %s", x, fs::path(filesInUpdatePackage[x].fileName).c_str());
    }
    return updateos::UpdateError::NoError;
}

void UpdatePureOS::informError(const char *format, ...)
{
    va_list argptr;
    std::unique_ptr<char[]> readBuf(new char[purefs::buffer::tar_buf]);
    va_start(argptr, format);
    vsnprintf (readBuf.get(), purefs::buffer::tar_buf, format, argptr);
    va_end(argptr);

    LOG_ERROR("UPDATE_ERRROR %s", readBuf.get());
}

void UpdatePureOS::informDebug(const char *format, ...)
{
    va_list argptr;
    std::unique_ptr<char[]> readBuf(new char[purefs::buffer::tar_buf]);
    va_start(argptr, format);
    vsnprintf (readBuf.get(), purefs::buffer::tar_buf, format, argptr);
    va_end(argptr);

    LOG_DEBUG("UPDATE_DEBUG %s", readBuf.get());
}

void UpdatePureOS::informUpdate(const char *format, ...)
{
    va_list argptr;
    std::unique_ptr<char[]> readBuf(new char[purefs::buffer::tar_buf]);
    va_start(argptr, format);
    vsnprintf (readBuf.get(), purefs::buffer::tar_buf, format, argptr);
    va_end(argptr);

    LOG_INFO("UPDATE_INFO %s", readBuf.get());

    auto msgToSend = std::make_shared<sdesktop::UpdateOsMessage>(sdesktop::UpdateMessageType::UpdateInform);
    messageText = std::string(readBuf.get());
    msgToSend->updateStats = (sdesktop::UpdateStats)(*this);
    sys::Bus::SendUnicast (msgToSend, app::name_desktop, owner);
}