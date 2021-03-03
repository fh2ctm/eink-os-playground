// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/filesystem_paths.hpp>

namespace
{
    constexpr inline auto PATH_SYS      = "/sys";
    constexpr inline auto PATH_USER     = "user";
    constexpr inline auto PATH_CURRENT  = "current";
    constexpr inline auto PATH_PREVIOUS = "previous";
    constexpr inline auto PATH_UPDATES  = "updates";
    constexpr inline auto PATH_TMP      = "tmp";
    constexpr inline auto PATH_BACKUP   = "backup";
    constexpr inline auto PATH_FACTORY  = "factory";
    constexpr inline auto eMMC_disk     = PATH_SYS;
} // namespace

namespace purefs
{
    std::filesystem::path createPath(const std::string &parent, const std::string &child) noexcept
    {
        return std::filesystem::path{parent} / child;
    }

    namespace dir
    {
        std::filesystem::path getRootDiskPath() noexcept
        {
            return std::filesystem::path{eMMC_disk};
        }
        std::filesystem::path getUserDiskPath() noexcept
        {
            return std::filesystem::path{eMMC_disk} / PATH_USER;
        }

        std::filesystem::path getCurrentOSPath() noexcept
        {
            return std::filesystem::path{eMMC_disk} / PATH_CURRENT;
        }

        std::filesystem::path getPreviousOSPath() noexcept
        {
            return std::filesystem::path{eMMC_disk} / PATH_PREVIOUS;
        }

        std::filesystem::path getUpdatesOSPath() noexcept
        {
            return std::filesystem::path{eMMC_disk} / PATH_USER / PATH_UPDATES;
        }

        std::filesystem::path getTemporaryPath() noexcept
        {
            return std::filesystem::path{eMMC_disk} / PATH_USER / PATH_TMP;
        }

        std::filesystem::path getBackupOSPath() noexcept
        {
            return std::filesystem::path{eMMC_disk} / PATH_USER / PATH_BACKUP;
        }

        std::filesystem::path getFactoryOSPath() noexcept
        {
            return std::filesystem::path{eMMC_disk} / PATH_FACTORY;
        }
    } // namespace dir
} // namespace purefs
