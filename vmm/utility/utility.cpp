#include <fcntl.h>
#include <unistd.h>

#include "vmm/utility/utility.hpp"

namespace vmm::utility {

/**
 * Closes a file descriptor.
 *
 * Examples
 * ========
 * ```
 * #include <vmm/utility.hpp>
 *
 * int kvm_fd {open("/dev/kvm", O_RDWR | O_CLOEXEC)};
 *
 * ...
 *
 * vmm::utility::close(kvm_fd);
 * ```
 */
auto close(const int fd) -> void {
    if (::close(fd) < 0) throw std::system_error{errno, std::system_category()};
}

}  // namespace vmm::utility
