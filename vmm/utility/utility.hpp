/*
 * utility.hpp - System utilities
 */

#pragma once

#include <stdexcept>
#include <system_error>
#include <sys/ioctl.h>

namespace vmm::utility {

class FileDescriptor {
    protected:
        unsigned int fd_;
        bool closed_;
    public:
        FileDescriptor(unsigned int fd) noexcept;
        ~FileDescriptor() noexcept;

        /**
         * Closes a file descriptor.
         *
         * Examples
         * ========
         * ```
         * #include <vmm/utility.hpp>
         * #include <iostream>
         *
         * auto fd = FileDescriptor{open("/dev/kvm", O_RDWR | O_CLOEXEC)};
         *
         * try {
         *     vmm::utility::close(fd);
         * }
         * catch (std::system_error& err) {
         *     std::err << "ERROR: Failed to close /dev/kvm\n";
         * }
         * ```
         */
        auto close() -> void;

        /**
         * Runs an ioctl.
         *
         * By default, the ioctl ran is equivalent to one with no arguments.
         *
         * Examples
         * ========
         * ```
         * #include <vmm/utility.hpp>
         * #include <linux/kvm.h>
         * #include <fcntl.h>
         * #include <sys/stat.h>
         *
         * auto fd = FileDescriptor{open("/dev/kvm", O_RDWR | O_CLOEXEC)};
         * auto version = fd.ioctl(KVM_GET_API_VERSION);
         * auto supported = fd.ioctl(KVM_CHECK_EXTENSION, KVM_CAP_USER_MEMORY);
         * ```
         */
        template<typename T=int>
        auto ioctl(const unsigned long req, T arg=T{}) const -> unsigned int {
            const auto ret = ::ioctl(fd_, req, arg);
            if (ret < 0)
                throw std::system_error{errno, std::system_category()};
            return ret;
        }
};

}  // namespace vmm::utility
