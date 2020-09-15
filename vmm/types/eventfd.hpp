#pragma once

#include <system_error> // error_code, system_category, system_error
#include <sys/eventfd.h> // eventfd

#include "vmm/types/file_descriptor.hpp"

namespace vmm::types {

class EventFd : public FileDescriptor {
    public:
        EventFd(int flags = 0);

        auto write(uint64_t value) const -> void;
        [[nodiscard]] auto read() const -> uint64_t;
};

}  // namespace vmm::types
