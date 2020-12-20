//
// guest.hpp -
//

#pragma once

#include <cassert> // assert
#include <cstdint> // uint*_t
#include <iostream> // ostream
#include <limits> // numeric_limits

#include "vmm/memory/detail/address.hpp" // Address, AddressValue

namespace vmm::memory::detail {

// A guest physical address.
//
// On aarch64, a 32-bit hypervisor may be used to support a 64-bit guest. For
// simplicity, uint64_t is used to store the raw value regardless of whether
// the guest is 32-bit or 64-bit.
class GuestAddress : public Address<GuestAddress, uint64_t>
{
    using value_type = GuestAddress;
    using size_type = uint64_t;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = pointer;
    using const_iterator = const_pointer;

    private:
        size_type m_addr{};
    public:
        explicit GuestAddress(size_type addr=0) noexcept : m_addr{addr} {}

        // Returns the raw value of the address.
        auto data() const noexcept -> size_type override;

        // Returns the bitwise AND of the address and a mask.
        auto mask(const size_type mask) const noexcept -> size_type override;

        // Returns the address plus some value (sum is wrapped).
        auto operator+(const size_type val) const noexcept -> value_type override;

        // Returns the sum of the address and another address (sum is wrapped).
        auto operator+(const_reference addr) const noexcept -> value_type override;

        // Returns the address minus some value (difference is wrapped).
        auto operator-(const size_type val) const noexcept -> value_type override;

        // Returns the difference between the address and another address
        // (difference is wrapped).
        auto operator-(const_reference addr) const noexcept -> value_type override;

        // Returns the address plus some value.
        //
        // If an "overflow" occurs, an exception is thrown.
        auto add(const size_type val) const -> value_type override;

        // Returns the sum of the address and another address.
        //
        // If an "overflow" occurs, an exception is thrown.
        auto add(const_reference addr) const -> value_type override;

        // Returns the address minus some value.
        //
        // If an "underflow" occurs, an exception is thrown.
        auto subtract(const size_type val) const -> value_type override;

        // Returns the difference between the address and another address.
        //
        // If an "underflow" occurs, an exception is thrown.
        auto subtract(const_reference addr) const -> value_type override;

        // Returns the offset between `m_addr` to `base`.
        //auto offset_from(const_reference base) const noexcept -> value_type;
        //auto checked_offset_from(const_reference base) const -> value_type;

        // Aligns the address to a power of 2.
        auto align_up(const size_type alignment) noexcept -> void override;

        // Aligns the address to a power of 2.
        //
        // If an "overflow" occurs, an exception is thrown.
        auto checked_align_up(const size_type alignment) -> void;
};

}  // vmm::memory::detail
