#include <unistd.h> // read, write, dup

#include "vmm/types/event.hpp"

namespace vmm::types {

/**
 * Examples
 * ========
 * ```
 * #include <vmm/types/event.hpp>
 *
 * auto fd = vmm::types::EventFd{EFD_NONBLOCK};
 * ```
 */
EventFd::EventFd(int flags) {
    fd_ = ::eventfd(0, flags);

    if (fd_ < 0) {
        VMM_THROW(std::system_error(errno, std::system_category()));
    }
}

/**
 * Increments the value of the 8-byte counter in the eventfd object by `value`.
 *
 * Examples
 * ========
 * ```
 * #include <vmm/types/event.hpp>
 *
 * auto fd = vmm::types::EventFd{};
 * fd.write(99);
 * ```
 */
auto EventFd::write(uint64_t value) const -> void {
    auto ret = ::write(fd_, &value, sizeof(uint64_t));

    if (ret < 0) {
        VMM_THROW(std::system_error(errno, std::system_category()));
    }
}

/**
 * Returns the value of the 8-byte counter in the eventfd object.
 *
 * Examples
 * ========
 * ```
 * #include <vmm/types/event.hpp>
 *
 * auto fd = vmm::types::EventFd{};
 * fd.write(99);
 * auto data = fd.read();
 * ```
 */
[[nodiscard]] auto EventFd::read() const -> uint64_t {
    auto buf = uint64_t{};
    auto ret = ::read(fd_, &buf, sizeof(uint64_t));

    if (ret < 0) {
        VMM_THROW(std::system_error(errno, std::system_category()));
    }

    return buf;
}

}  // namespace vmm::types