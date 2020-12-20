#include "vmm/memory/detail/guest.hpp"
#include "vmm/types/detail/exceptions.hpp"
#include <stdexcept>
#include <string>

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

auto GuestAddress::align_up(const size_type alignment) noexcept -> void
{
    const auto mask = alignment - 1;
    m_addr = (*this + mask) & !mask;
}

}  // vmm::memory::detail
