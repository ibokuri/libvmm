#include <unistd.h> // read, write, dup

#include "vmm/types/eventfd.hpp"

namespace vmm::types {

EventFd::EventFd(int flags) noexcept : FileDescriptor(create(flags)) {}

EventFd::EventFd(const EventFd& other) {
    fd_ = ::dup(other.fd_);

    if (fd_ < 0) {
        VMM_THROW(std::system_error(errno, std::system_category()));
    }
}

auto EventFd::operator=(const EventFd& other) -> EventFd& {
    if (&other == this)
        return *this;

    fd_ = ::dup(other.fd_);

    if (fd_ < 0) {
        VMM_THROW(std::system_error(errno, std::system_category()));
    }

    return *this;
}

auto EventFd::write(uint64_t value) const -> void {
    auto ret = ::write(fd_, &value, sizeof(uint64_t));

    if (ret < 0) {
        VMM_THROW(std::system_error(errno, std::system_category()));
    }
}

[[nodiscard]] auto EventFd::read() const -> uint64_t {
    auto buf = uint64_t{0};
    auto ret = ::read(fd_, &buf, sizeof(uint64_t));

    if (ret < 0) {
        VMM_THROW(std::system_error(errno, std::system_category()));
    }

    return buf;
}

}  // namespace vmm::types
