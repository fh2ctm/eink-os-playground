// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include <purefs/fs/filesystem.hpp>
#include <purefs/fs/filesystem_operations.hpp>
#include <purefs/fs/mount_point.hpp>
#include <purefs/blkdev/disk_manager.hpp>
#include <purefs/fs/thread_local_cwd.hpp>
#include <purefs/blkdev/disk_handle.hpp>
#include <log/log.hpp>
#include <split_sv.hpp>
#include <errno.h>
#include <mutex.hpp>

namespace purefs::fs
{
    filesystem::filesystem(std::shared_ptr<blkdev::disk_manager> diskmm)
        : m_diskmm(diskmm), m_lock(new cpp_freertos::MutexRecursive)
    {}

    filesystem::~filesystem()
    {}

    auto filesystem::register_filesystem(std::string_view fsname, std::shared_ptr<filesystem_operations> fops) -> int
    {
        if (fops == nullptr) {
            LOG_ERROR("Filesystem operations doesn't exists");
            return -EINVAL;
        }
        cpp_freertos::LockGuard _lck(*m_lock);
        const auto it = m_fstypes.find(std::string(fsname));
        if (it != std::end(m_fstypes)) {
            LOG_ERROR("Disc: %.*s already registered.", int(fsname.length()), fsname.data());
            return -EEXIST;
        }
        else {
            const auto ret = fops->finalize_registration(m_diskmm);
            if (ret) {
                LOG_ERROR("Disc: Unable to register filesystem finalize error %i", ret);
                return ret;
            }
            m_fstypes.emplace(std::make_pair(fsname, fops));
            return {};
        }
    }

    auto filesystem::unregister_filesystem(std::string_view fsname) -> int
    {
        cpp_freertos::LockGuard _lck(*m_lock);
        const auto it = m_fstypes.find(std::string(fsname));
        if (it == std::end(m_fstypes)) {
            LOG_ERROR("VFS: filesystem %.*s doesn't exists in manager.", int(fsname.length()), fsname.data());
            return -ENOENT;
        }
        if (it->second->mount_count() > 0) {
            LOG_ERROR("VFS: fileystem %.*s  is already used", int(fsname.length()), fsname.data());
            return -EBUSY;
        }
        m_fstypes.erase(it);
        return {};
    }

    auto filesystem::mount(std::string_view dev_or_part,
                           std::string_view target,
                           std::string_view fs_type,
                           unsigned flags) -> int
    {
        // Sanity check input data
        if (target.size() <= 1 || target[0] != '/') {
            LOG_ERROR("VFS: Invalid target mountpoint path %.*s", int(target.length()), target.data());
            return -EINVAL;
        }
        if (flags & ~(mount_flags::remount | mount_flags::read_only)) {
            LOG_ERROR("VFS: passed mount flags is not supported");
            return -ENOTSUP;
        }
        {
            cpp_freertos::LockGuard _lock(*m_lock);
            const auto mpi = m_mounts.find(std::string(target));
            if (mpi != std::end(m_mounts)) {
                if (flags & mount_flags::remount) {
                    mpi->second->modify_flags(flags & ~mount_flags::remount);
                    return {};
                }
                else {
                    LOG_ERROR("VFS: mount point already exists %.*s", int(target.length()), target.data());
                    return -EBUSY;
                }
            }
            const auto mpp = m_partitions.find(std::string(dev_or_part));
            if (mpp != std::end(m_partitions)) {
                LOG_ERROR("VFS: partition already used %.*s", int(dev_or_part.length()), dev_or_part.data());
                return -EBUSY;
            }
            const auto vsi = m_fstypes.find(std::string(fs_type));
            if (vsi == std::end(m_fstypes)) {
                LOG_ERROR("VFS: requested filesystem %.*s not registered", int(fs_type.length()), fs_type.data());
                return -ENODEV;
            }
            // Trying to open disk or part by manager
            blkdev::disk_fd diskh;
            {
                auto disk_mgr = m_diskmm.lock();
                if (disk_mgr) {
                    diskh = disk_mgr->device_handle(dev_or_part);
                }
                else {
                    LOG_ERROR("VFS: Unable to lock device manager");
                    return -EIO;
                }
            }
            if (diskh) {
                const auto mnt_point = vsi->second->mount_prealloc(diskh, target, flags);
                const auto ret_mnt   = vsi->second->mount(mnt_point);
                if (!ret_mnt) {
                    m_mounts.emplace(std::make_pair(target, mnt_point));
                    m_partitions.emplace(dev_or_part);
                }
                else {
                    return ret_mnt;
                }
            }
            else {
                LOG_ERROR("Device or partition %.*s doesn't exists", int(dev_or_part.size()), dev_or_part.data());
                return -ENXIO;
            }
        }
        return {};
    }

    auto filesystem::umount(std::string_view mount_point) -> int
    {
        cpp_freertos::LockGuard _lck(*m_lock);
        auto mnti = m_mounts.find(std::string(mount_point));
        if (mnti == std::end(m_mounts)) {
            return -ENOENT;
        }
        auto fsops = mnti->second->fs_ops();
        if (!fsops) {
            LOG_ERROR("Unable to lock filesystem operation");
            return -EIO;
        }
        const auto diskh    = mnti->second->disk();
        const auto umnt_ret = fsops->umount(mnti->second);
        if (umnt_ret) {
            return umnt_ret;
        }
        if (diskh) {
            m_partitions.erase(std::string(diskh->name()));
        }
        m_mounts.erase(mnti);
        return {};
    }

    auto filesystem::read_mountpoints(std::list<std::string> &mountpoints) const -> int
    {
        cpp_freertos::LockGuard _lck(*m_lock);
        for (const auto &mntp : m_mounts) {
            mountpoints.push_back(mntp.first);
        }
        return {};
    }

    auto filesystem::find_mount_point(std::string_view path) const noexcept
        -> std::tuple<std::shared_ptr<internal::mount_point>, size_t>
    {
        size_t longest_match{};
        std::shared_ptr<internal::mount_point> mount_pnt;
        cpp_freertos::LockGuard _lck(*m_lock);
        for (const auto &mntp : m_mounts) {
            const auto slen = mntp.first.size();
            if ((slen < longest_match) || (slen > path.size())) {
                continue;
            }
            if ((slen > 1) && (path[slen] != '/') && (path[slen] != '\0')) {
                continue;
            }
            if (path.compare(0, slen, mntp.first) == 0) {
                mount_pnt     = mntp.second;
                longest_match = slen;
            }
        }
        return std::make_tuple(mount_pnt, longest_match);
    }

    auto filesystem::absolute_path(std::string_view path) noexcept -> std::string
    {
        std::string ret{};
        if (!path.empty() && path[0] == path_separator) {
            ret.append(path);
        }
        else {
            ret.append(internal::get_thread_local_cwd_path()).append("/").append(path);
        }
        return normalize_path(ret);
    }

    auto filesystem::normalize_path(std::string_view path) noexcept -> std::string
    {
        std::string ret;
        auto paths = utils::split<std::list, std::string_view>(path, "/");
        for (auto it = std::begin(paths); it != std::end(paths);) {
            if (!it->compare(".")) {
                it = paths.erase(it);
            }
            else if (!it->compare("..")) {
                auto pit = it;
                --pit;
                if (pit != std::end(paths)) {
                    it = paths.erase(pit);
                    if (it != std::end(paths))
                        it = paths.erase(it);
                }
                else {
                    it = paths.erase(it);
                }
            }
            else {
                ++it;
            }
        }
        for (auto ep : paths) {
            if (!ep.empty()) {
                ret.append("/").append(ep);
            }
        }
        if (ret.empty())
            ret.append("/");
        return ret;
    }

    auto filesystem::add_filehandle(fsfile file) noexcept -> int
    {
        cpp_freertos::LockGuard _lck(*m_lock);
        return m_fds.insert(file) + first_file_descriptor;
    }

    auto filesystem::remove_filehandle(int fds) noexcept -> fsfile
    {
        if (fds < first_file_descriptor) {
            return nullptr;
        }
        fds -= first_file_descriptor;
        cpp_freertos::LockGuard _lck(*m_lock);
        fsfile ret{};
        if (m_fds.exists(fds)) {
            ret = m_fds[fds];
            m_fds.remove(fds);
        }
        return ret;
    }

    auto filesystem::find_filehandle(int fds) const noexcept -> fsfile
    {
        fsfile ret{};
        if (fds < first_file_descriptor) {
            return ret;
        }
        fds -= first_file_descriptor;
        cpp_freertos::LockGuard _lck(*m_lock);
        if (m_fds.exists(fds)) {
            ret = m_fds[fds];
        }
        return ret;
    }

} // namespace purefs::fs
