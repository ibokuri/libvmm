//
// event.cpp - Event file descriptor
//

#include <unistd.h> // read, write, dup

#include "vmm/types/event.hpp"

namespace vmm::types {

EventFd::EventFd(int flags)
{
    m_fd = ::eventfd(0, flags);

    if (m_fd < 0)
        VMM_THROW(std::system_error(errno, std::system_category()));
}

auto EventFd::write(uint64_t value) const -> void
{
    auto ret = ::write(m_fd, &value, sizeof(uint64_t));

    if (ret < 0)
        VMM_THROW(std::system_error(errno, std::system_category()));
}

[[nodiscard]] auto EventFd::read() const -> uint64_t
{
    auto buf = uint64_t{};
    auto ret = ::read(m_fd, &buf, sizeof(uint64_t));

    if (ret < 0)
        VMM_THROW(std::system_error(errno, std::system_category()));

    return buf;
}

}  // namespace vmm::types
