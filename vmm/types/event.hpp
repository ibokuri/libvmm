//
// event.hpp - Event file descriptor
//

#pragma once

#include <cstdint> // uint64_t
#include <system_error> // error_code, system_category, system_error
#include <sys/eventfd.h> // eventfd

#include "vmm/types/file_descriptor.hpp"

namespace vmm::types {

// An address in either programmable or memory-mapped I/O space.
enum class IoEventAddress {
    Pio,
    Mmio,
};

class EventFd : public FileDescriptor
{
    public:
        EventFd(int flags=0);

        // Increments the value of the 8-byte counter in the eventfd object by `value`.
        auto write(uint64_t value) const -> void;

        // Returns the value of the 8-byte counter in the eventfd object.
        [[nodiscard]] auto read() const -> uint64_t;
};

}  // namespace vmm::types
