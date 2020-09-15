#pragma once

#include <system_error> // error_code, system_category, system_error
#include <sys/eventfd.h> // eventfd

#include "vmm/types/file_descriptor.hpp"

namespace vmm::types {

class EventFd : public FileDescriptor {
    public:
        EventFd(int flags = 0) noexcept;

        EventFd(const EventFd& other);
        EventFd(EventFd&& other) = default;
        auto operator=(const EventFd& other) -> EventFd&;
        auto operator=(EventFd&& other) -> EventFd& = default;

        [[nodiscard]] static auto create(int flags) -> unsigned int {
            auto fd = ::eventfd(0, flags);

            if (fd < 0) {
                VMM_THROW(std::system_error(errno, std::system_category()));
            }

            return fd;
        }

        auto write(uint64_t value) const -> void;
        [[nodiscard]] auto read() const -> uint64_t;
};

}  // namespace vmm::types
