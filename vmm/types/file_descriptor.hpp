/*
 * types.hpp - System utilities
 */

#pragma once

#include <system_error> // error_code, system_error
#include <sys/ioctl.h> // ioctl
#include <cerrno> // errno

#include "vmm/types/detail/exceptions.hpp"

namespace vmm::types {

class FileDescriptor {
    protected:
        int fd_;
        bool closed_ = false;

        FileDescriptor() noexcept;
    public:
        FileDescriptor(int fd) noexcept;
        ~FileDescriptor() noexcept;

        FileDescriptor(const FileDescriptor& other);
        FileDescriptor(FileDescriptor&& other) = default;
        auto operator=(const FileDescriptor& other) -> FileDescriptor&;
        auto operator=(FileDescriptor&& other) -> FileDescriptor& = default;

        /**
         * Closes a file descriptor.
         *
         * Examples
         * ========
         * ```
         * #include <iostream>
         * #include <vmm/types.hpp>
         *
         * auto fd = FileDescriptor{open("/dev/kvm", O_RDWR | O_CLOEXEC)};
         *
         * // do stuff
         * ...
         *
         * try {
         *     fd.close();
         * }
         * catch (std::system_error& e) {
         *     std::cerr << "message: " << e.what() << std::endl;
         * }
         * ```
         */
        auto close() -> void;

        /**
         * Runs an ioctl.
         *
         * Examples
         * ========
         * ```
         * #include <fcntl.h>
         * #include <sys/stat.h>
         * #include <linux/kvm.h>
         * #include <vmm/types/file_descriptor.hpp>
         *
         * auto fd = FileDescriptor{open("/dev/kvm", O_RDWR | O_CLOEXEC)};
         * auto version = fd.ioctl(KVM_GET_API_VERSION);
         * auto supported = fd.ioctl(KVM_CHECK_EXTENSION, KVM_CAP_USER_MEMORY);
         * ```
         */
        template<typename T=int>
        auto ioctl(int req, T arg=T{}) const -> int {
            const auto ret = ::ioctl(fd_, req, arg);

            if (ret < 0) {
                VMM_THROW(std::system_error(errno, std::system_category()));
            }

            return ret;
        }
};

}
