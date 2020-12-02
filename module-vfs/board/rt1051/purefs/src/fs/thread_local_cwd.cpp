// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include <thread.hpp>
#include <limits.h>
#include <cstring>
#include <purefs/fs/thread_local_cwd.hpp>

#if configNUM_THREAD_LOCAL_STORAGE_POINTERS < 4
#error Not enough TLS pointers
#endif

namespace purefs::fs::internal
{
    namespace
    {
        constexpr auto CWD_THREAD_LOCAL_INDEX = 3;
        auto get_tls() noexcept -> char *
        {
            auto pcwd = reinterpret_cast<char *>(pvTaskGetThreadLocalStoragePointer(nullptr, CWD_THREAD_LOCAL_INDEX));
            if (pcwd == nullptr) {
                pcwd = new (std::nothrow) char[PATH_MAX + 1];
                if (pcwd) {
                    std::strncpy(pcwd, "/", PATH_MAX);
                    vTaskSetThreadLocalStoragePointer(nullptr, CWD_THREAD_LOCAL_INDEX, pcwd);
                }
            }
            return pcwd;
        }
    } // namespace
    auto get_thread_local_cwd_path() noexcept -> std::string_view
    {
        return get_tls();
    }
    auto set_thread_cwd_path(std::string_view path) noexcept -> int
    {
        if (path.size() > PATH_MAX) {
            return -ERANGE;
        }
        auto tls = get_tls();
        if (!tls) {
            return -ENOMEM;
        }
        else {
            std::memcpy(tls, path.data(), path.size());
            tls[path.size()] = '\0';
            return 0;
        }
    }
    auto cleanup_thread_local_cwd_mem() -> void
    {
        auto pcwd = reinterpret_cast<char *>(pvTaskGetThreadLocalStoragePointer(nullptr, CWD_THREAD_LOCAL_INDEX));
        if (pcwd) {
            delete[] pcwd;
            vTaskSetThreadLocalStoragePointer(nullptr, CWD_THREAD_LOCAL_INDEX, nullptr);
        }
    }
} // namespace purefs::fs::internal
