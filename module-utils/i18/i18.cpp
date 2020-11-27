// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "i18.hpp"

namespace utils
{
    namespace
    {
        const auto LanguageDirPath = std::filesystem::path{"assets"} / "lang";
        constexpr auto extension   = ".json";

        auto returnNonEmptyString(const std::string &str, const std::string &ret) -> const std::string &
        {
            return str.empty() ? ret : str;
        }
    } // namespace

    i18 localize;
    json11::Json LangLoader::createJson(const std::string &filename)
    {
        const fs::path path = LanguageDirPath / (filename + extension);
        auto fd             = vfs.fopen(path.c_str(), "r");
        if (fd == nullptr) {
            LOG_FATAL("Error during opening file %s", path.c_str());
            return json11::Json();
        }

        uint32_t fsize = vfs.filelength(fd);

        auto stream = new char[fsize + 1]; // +1 for NULL terminator

        memset(stream, 0, fsize + 1);

        vfs.fread(stream, 1, fsize, fd);

        std::string err;
        json11::Json js = json11::Json::parse(stream, err);

        delete[] stream;
        vfs.fclose(fd);

        // Error
        if (err.length() != 0) {
            LOG_FATAL("%s", err.c_str());
            return json11::Json();
        }
        else {
            return js;
        }
    }

    std::vector<Language> LangLoader::getAvailableDisplayLanguages() const
    {
        std::vector<std::string> languageNames;
        for (const auto &entry : fs::directory_iterator(LanguageDirPath)) {
            languageNames.push_back(fs::path(entry.path()).stem());
        }
        return languageNames;
    }

    void i18::setInputLanguage(const Language &lang)
    {
        if (lang == currentInputLanguage) {
            return;
        }
        currentInputLanguage = lang;
        if (lang == fallbackLanguageName) {
            inputLanguage = fallbackLanguage;
        }
        else {
            json11::Json pack = loader.createJson(lang);
            inputLanguage     = pack;
        }
    }
    const std::string &i18::getInputLanguage(const std::string &str)
    {
        // if language pack returned nothing then try default language
        if (inputLanguage[str].string_value().empty()) {
            return returnNonEmptyString(fallbackLanguage[str].string_value(), str);
        }
        return returnNonEmptyString(inputLanguage[str].string_value(), str);
    }

    const std::string &i18::get(const std::string &str)
    {
        // if language pack returned nothing then try default language
        if (displayLanguage[str].string_value().empty()) {
            return returnNonEmptyString(fallbackLanguage[str].string_value(), str);
        }
        return returnNonEmptyString(displayLanguage[str].string_value(), str);
    }

    void i18::setDisplayLanguage(const Language &lang)
    {
        if (!backupLanguageInitializer) {
            fallbackLanguage          = loader.createJson(fallbackLanguageName);
            displayLanguage           = fallbackLanguage;
            backupLanguageInitializer = true;
        }
        if (lang == currentDisplayLanguage) {
            return;
        }
        currentDisplayLanguage = lang;
        if (lang == fallbackLanguageName) {
            displayLanguage = fallbackLanguage;
        }
        else {
            json11::Json pack = loader.createJson(lang);
            // Suspend whole system during switching lang packs
            vTaskSuspendAll();
            displayLanguage = pack;
            xTaskResumeAll();
        }
    }

    void i18::setFallbackLanguage(const Language &lang)
    {
        fallbackLanguageName = std::move(lang);
    }

} // namespace utils
