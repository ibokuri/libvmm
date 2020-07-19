/*
 * utils.hpp - System utilities
 */

#pragma once

#include <stdexcept>
#include <system_error>

#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

namespace vmm::utils {
    /**
     * Runs an ioctl.
     *
     * By default, the ioctl ran is equivalent to one with no arguments.
     *
     * # Examples
     *
     * ```
     * #include <vmm/utils.hpp>
     *
     * #include <fcntl.h>
     * #include <linux/kvm.h>
     * #include <sys/stat.h>
     *
     * using ioctl = vmm::utils::ioctl;
     *
     * auto kvm_fd    {open("/dev/kvm", O_RDWR | O_CLOEXEC)};
     * auto version   {ioctl(kvm_fd, KVM_GET_API_VERSION)};
     * auto supported {ioctl(kvm_fd, KVM_CHECK_EXTENSION, KVM_CAP_USER_MEMORY)};
     * ```
     */
	template<typename T = int>
	auto ioctl(const int fd, const unsigned long req, T arg=T{}) -> unsigned int {
		const auto ret {::ioctl(fd, req, arg)};
		if (ret < 0) throw std::system_error{errno, std::system_category()};
		return ret;
	}

    /**
     * Closes a file descriptor.
     *
     * # Examples
     *
     * ```
     * #include <vmm/utils.hpp>
     *
     * int kvm_fd {open("/dev/kvm", O_RDWR | O_CLOEXEC)};
     *
     * ...
     *
     * vmm::utils::close(kvm_fd);
     * ```
     */
    auto close(const int fd) -> void {
        if (::close(fd) < 0) throw std::system_error{errno, std::system_category()};
    }
}
