#pragma once

#include <module-utils/microtar/src/microtar.hpp>
#include <vfs.hpp>
#include <json/json11.hpp>

class ServiceDesktop;
namespace fs = std::filesystem;
namespace updateos
{
    namespace file
    {
        const inline std::string checksums = "checksums.txt";
        const inline std::string sql_mig   = "sqlmig.json";
        const inline std::string version   = "version.json";

    } // namespace file

    namespace extension
    {
        const inline std::string update = ".tar";
    }

    const inline int prefix_len = 8;

    enum class UpdateError
    {
        NoError,
        CantCreateTempDir,
        CantCreateUpdatesDir,
        CantRemoveUniqueTmpDir,
        CantRemoveUpdateFile,
        CantCreateUniqueTmpDir,
        CantCreateExtractedFile,
        CantOpenChecksumsFile,
        VerifyChecksumsFailure,
        VerifyVersionFailure,
        CantWriteBootloader,
        CantOpenUpdateFile,
        CantDeletePreviousOS,
        CantRenameCurrentToPrevious,
        CantRenameTempToCurrent,
        CantUpdateJSON,
        CantSaveJSON,
        CantUpdateCRC32JSON
    };

    enum class BootloaderUpdateError
    {
        NoError,
        NoBootloaderFile,
        CantOpenBootloaderFile,
        CantAllocateBuffer,
        CantLoadBootloaderFile
    };

    enum class UpdateState
    {
        Initial,
        UpdateFileSet,
        CreatingDirectories,
        ExtractingFiles,
        UpdatingBootloader,
        ChecksumVerification,
        VersionVerificiation,
        ReadyForReset
    };

    enum class UpdateMessageType
    {
        UpdateFoundOnBoot,
        UpdateCheckForUpdateOnce,
        UpdateNow,
        UpdateError,
        UpdateInform
    };

    struct UpdateStats
    {
        fs::path updateFile            = "";
        fs::path fileExtracted         = "";
        fs::path updateTempDirectory   = PATH_SYS "/" PATH_TMP;
        uint32_t totalBytes            = 0;
        uint32_t currentExtractedBytes = 0;
        uint32_t fileExtractedSize     = 0;
        uint32_t uuid                  = 0;
        std::string messageText        = "";
        updateos::UpdateState status;
        json11::Json versioInformation;
    };

}; // namespace updateos

struct FileInfo
{
    FileInfo(mtar_header_t &h, unsigned long crc32);
    json11::Json to_json() const;

    std::string fileName;
    std::size_t fileSize;
    unsigned long fileCRC32;
};

class UpdatePureOS : public updateos::UpdateStats
{
  public:
    UpdatePureOS(ServiceDesktop *ownerService);

    updateos::UpdateError runUpdate(const int resetDelay);
    updateos::UpdateError prepareTempDirForUpdate();
    updateos::UpdateError unpackUpdate();
    updateos::UpdateError verifyChecksums();
    updateos::UpdateError verifyVersion();
    updateos::UpdateError updateBootloader();
    updateos::UpdateError prepareRoot();
    updateos::UpdateError updateBootJSON();
    updateos::UpdateError setUpdateFile(fs::path updateFileToUse);
    updateos::UpdateError cleanupAfterUpdate();
    updateos::UpdateError updateUserData();

    void informError(const char *format, ...);
    void informDebug(const char *format, ...);
    void informUpdate(const char *format, ...);

    updateos::BootloaderUpdateError writeBootloader(fs::path bootloaderFile);

    void getChecksumInfo(const std::string &infoLine, std::string &filePath, unsigned long *fileCRC32Long);
    unsigned long getExtractedFileCRC32(const std::string &filePath);
    bool unpackFileToTemp(mtar_header_t &header, unsigned long *crc32);
    const fs::path getUpdateTmpChild(const fs::path &childPath);

    static const json11::Json getVersionInfoFromFile(const fs::path &updateFile);
    static bool isUpgradeToCurrent(const std::string &versionToCompare);
    static const fs::path checkForUpdate();

  private:
    std::vector<FileInfo> filesInUpdatePackage;
    mtar_t updateTar      = {};
    ServiceDesktop *owner = nullptr;
};
