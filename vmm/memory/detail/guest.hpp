//
// guest.hpp -
//

#pragma once

#include <cassert> // assert
#include <cstdint> // uint*_t
#include <filesystem> // path
#include <fstream> // fstream
#include <ios> // ios_base
#include <iostream> // ostream
#include <limits> // numeric_limits
#include <memory> // shared_ptr
#include <string> // string

#include "vmm/memory/detail/address.hpp" // Address, AddressValue

namespace vmm::memory::detail {

// A guest physical address.
//
// On aarch64, a 32-bit hypervisor may be used to support a 64-bit guest. For
// simplicity, uint64_t is used to store the raw value regardless of whether
// the guest is 32-bit or 64-bit.
class GuestAddress : public Address<GuestAddress, uint64_t>
{
    public:
        using value_type = GuestAddress;
        using size_type = uint64_t;
        using reference = value_type&;
        using const_reference = const value_type&;

        explicit GuestAddress(size_type addr=0) noexcept : m_addr{addr} {}

        auto data() const noexcept -> size_type override;

        auto operator&(const size_type mask) const noexcept -> size_type override;
        auto operator|(const size_type mask) const noexcept -> size_type override;
        auto operator+(const size_type val) const noexcept -> value_type override;
        auto operator+(const_reference addr) const noexcept -> value_type override;
        auto operator-(const size_type val) const noexcept -> value_type override;
        auto operator-(const_reference addr) const noexcept -> value_type override;

        auto align(const size_type alignment) noexcept -> reference override;
    private:
        size_type m_addr{};
};

// An offset into a memory region.
class MemoryRegionAddress : public Address<MemoryRegionAddress, uint64_t>
{
    public:
        using value_type = MemoryRegionAddress;
        using size_type = uint64_t;
        using reference = value_type&;
        using const_reference = const value_type&;

        explicit MemoryRegionAddress(size_type addr=0) noexcept : m_addr{addr} {}

        auto data() const noexcept -> size_type override;

        auto operator&(const size_type mask) const noexcept -> size_type override;
        auto operator|(const size_type mask) const noexcept -> size_type override;
        auto operator+(const size_type val) const noexcept -> value_type override;
        auto operator+(const_reference addr) const noexcept -> value_type override;
        auto operator-(const size_type val) const noexcept -> value_type override;
        auto operator-(const_reference addr) const noexcept -> value_type override;

        auto align(const size_type alignment) noexcept -> reference override;
    private:
        size_type m_addr{};
};

}  // vmm::memory::detail
