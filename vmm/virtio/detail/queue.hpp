//
// queue.hpp - Virtio queue
//

#pragma once

#include <bitset> // bitset
#include <cstdint> // uint*_t
#include <iostream> // ostream

namespace vmm::virtio::detail {

enum class VirtqueueDescriptorFlag : uint16_t {
    Next,
    Write,
    Indirect,
};

class GuestAddress
{
    private:
        uint64_t m_addr;
    public:
        GuestAddress(uint64_t addr) : m_addr{addr} {}

        friend auto operator<<(std::ostream& os,
                               const GuestAddress& ga) -> std::ostream&
        {
            os << ga.m_addr;
            return os;
        }
};

// Virtio descriptor constraints with C representation
//
// TODO:
//
//  * Implement ByteValued interface.
//
// REFERENCE:
//
//     #define VIRTQ_DESC_F_NEXT       1 // Marks a buffer as continuing via the `next` field.
//     #define VIRTQ_DESC_F_WRITE      2 // Marks a buffer as device write-only (otherwise device read-only).
//     #define VIRTQ_DESC_F_INDIRECT   4 // Means the buffer contains a list of buffer descriptors.
//
//     struct virtq_desc {
//         le64 addr;
//         le32 len;
//         le16 flags; // The flags as indicated above.
//         le16 next; // Next field if flags & NEXT.
//     };

class Descriptor
{
    private:
        // Guest's physical address of device specific data
        //uint64_t m_addr;
        GuestAddress m_addr;

        // Length of device-specific data
        uint32_t m_len;

        // Includes next, write, and indirect bits
        std::bitset<16> m_flags;

        // Index into the descriptor table of the next descriptor if `flags`
        // has the `next` bit set
        uint16_t m_next;
    public:
        // Returns the guest physical address of descriptor buffer.
        [[nodiscard]] auto addr() const noexcept -> GuestAddress;

        // Returns the length of descriptor buffer.
        [[nodiscard]] constexpr auto len() const noexcept -> uint32_t
        {
            return m_len;
        }

        // Returns the descriptor's flags, including the next, write and
        // indirect bits.
        [[nodiscard]] constexpr auto flags() const noexcept -> std::bitset<16>
        {
            return m_flags;
        }

        // Returns the value stored in the `next` field of the descriptor.
        [[nodiscard]] constexpr auto next() const noexcept -> uint16_t
        {
            return m_next;
        }

        // Checks if the `VIRTQ_DESC_F_NEXT` flag is set.
        [[nodiscard]] constexpr auto has_next() const noexcept -> bool
        {
            return m_flags[static_cast<std::size_t>(VirtqueueDescriptorFlag::Next)];
        }

        // Checks if the driver designated this as a write only descriptor.
        //
        // If false, the descriptor is read-only. Write-only means the
        // emulated device can write and the driver can read.
        [[nodiscard]] constexpr auto is_write_only() const noexcept -> bool
        {
            return m_flags[static_cast<std::size_t>(VirtqueueDescriptorFlag::Write)];
        }

        // Checks if the descriptor is an indirect descriptor.
        //
        // TODO: There are a couple of restrictions in terms of which flags
        // combinations are actually valid for indirect descriptors. Implement
        // those checks as well somewhere.
        [[nodiscard]] constexpr auto is_indirect() const noexcept -> bool
        {
            return m_flags[static_cast<std::size_t>(VirtqueueDescriptorFlag::Indirect)];
        }
};

struct VirtQueue
{
};

}  // namespace vmm::virtio::detail
