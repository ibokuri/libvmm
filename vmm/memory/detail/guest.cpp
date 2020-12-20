#include "vmm/memory/detail/guest.hpp"
#include "vmm/types/detail/exceptions.hpp"
#include <stdexcept>
#include <string>

namespace vmm::memory::detail {

auto GuestAddress::data() const noexcept -> size_type
{
    return m_addr;
}

auto GuestAddress::mask(const size_type mask) const noexcept -> size_type
{
    return m_addr & mask;
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

auto GuestAddress::add(const size_type val) const -> value_type
{
    if (m_addr > std::numeric_limits<size_type>::max() - val) {
        VMM_THROW(std::overflow_error(std::to_string(m_addr) + " + " + std::to_string(val) + " results in overflow."));
    }

    return *this + val;
}

auto GuestAddress::add(const_reference addr) const -> value_type
{
    return add(addr.data());
}

auto GuestAddress::subtract(const size_type val) const -> value_type
{
    if (m_addr < std::numeric_limits<size_type>::min() + val) {
        VMM_THROW(std::underflow_error(std::to_string(m_addr) + " - " + std::to_string(val) + " results in underflow."));
    }

    return *this - val;
}

auto GuestAddress::subtract(const_reference addr) const -> value_type
{
    return subtract(addr.data());
}

auto GuestAddress::align_up(const size_type alignment) noexcept -> void
{
    const auto mask = alignment - 1;
    m_addr = (*this + mask).data() & !mask;
}

auto GuestAddress::checked_align_up(const size_type alignment) -> void
{
    const auto mask = alignment - 1;

    assert(alignment);
    assert(!(alignment & mask));

    m_addr = add(mask).data() & !mask;
}

}  // vmm::memory::detail
