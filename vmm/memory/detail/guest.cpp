#include "vmm/memory/detail/guest.hpp"
#include "vmm/types/detail/exceptions.hpp"

namespace vmm::memory::detail {

auto GuestAddress::data() const noexcept -> size_type
{
    return m_addr;
}

auto GuestAddress::operator&(const size_type mask) const noexcept -> size_type
{
    return m_addr & mask;
}

auto GuestAddress::operator|(const size_type mask) const noexcept -> size_type
{
    return m_addr | mask;
}

auto GuestAddress::operator+(const size_type val) const noexcept -> value_type
{
    return GuestAddress{m_addr + val};
}

auto GuestAddress::operator+(const_reference addr) const noexcept -> value_type
{
    return *this + addr.data();
}

auto GuestAddress::operator-(const size_type val) const noexcept -> value_type
{
    return GuestAddress{m_addr - val};
}

auto GuestAddress::operator-(const_reference addr) const noexcept -> value_type
{
    return *this - addr.data();
}

auto GuestAddress::align(const size_type alignment) noexcept -> reference
{
    const auto mask = alignment - 1;
    m_addr = (*this + mask) & ~mask;
    return *this;
}

auto MemoryRegionAddress::data() const noexcept -> size_type
{
    return m_addr;
}

auto MemoryRegionAddress::operator&(const size_type mask) const noexcept -> size_type
{
    return m_addr & mask;
}

auto MemoryRegionAddress::operator|(const size_type mask) const noexcept -> size_type
{
    return m_addr | mask;
}

auto MemoryRegionAddress::operator+(const size_type val) const noexcept -> value_type
{
    return MemoryRegionAddress{m_addr + val};
}

auto MemoryRegionAddress::operator+(const_reference addr) const noexcept -> value_type
{
    return *this + addr.data();
}

auto MemoryRegionAddress::operator-(const size_type val) const noexcept -> value_type
{
    return MemoryRegionAddress{m_addr - val};
}

auto MemoryRegionAddress::operator-(const_reference addr) const noexcept -> value_type
{
    return *this - addr.data();
}

auto MemoryRegionAddress::align(const size_type alignment) noexcept -> reference
{
    const auto mask = alignment - 1;
    m_addr = (*this + mask) & ~mask;
    return *this;
}

}  // vmm::memory::detail
