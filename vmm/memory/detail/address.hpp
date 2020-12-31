//
// address.hpp -
//

#pragma once

namespace vmm::memory::detail {

#include <optional>
#include <limits>

// An address within some address space.
//
// NOTE: The operators (+, -, &, |, etc.) are not supported, meaning clients
//       must explicitly invoke the corresponding methods. However, there are
//       exceptions: `Address` (BitAnd|BitOr) `AddressValue` are supported.
template<typename Concrete, typename Size>
class Address
{
    public:
        Address() = default;
        virtual ~Address() = default;

        Address(const Address&) = default;
        Address& operator=(const Address&) = default;
        Address(Address&&) = default;
        Address& operator=(Address&&) = default;

        // Returns the raw value of the address.
        virtual auto data() const noexcept -> Size = 0;

        // Returns the bitwise AND of the address and a mask.
        virtual auto operator&(const Size) const noexcept -> Size = 0;

        // Returns the bitwise OR of the address and a mask.
        virtual auto operator|(const Size) const noexcept -> Size = 0;

        // Adds a value to the address (sum is wrapped).
        virtual auto operator+(const Size) const noexcept -> Concrete = 0;

        // Adds two addresses together (sum is wrapped).
        virtual auto operator+(const Concrete&) const noexcept -> Concrete = 0;

        // Adds a value to the address (nullopt if wrapped).
        virtual auto add(const Size value) const -> std::optional<Concrete>
        {
            if (std::numeric_limits<Size>::max() - value < data())
                return std::nullopt;

            return *this + value;
        }

        // Adds two addresses together (nullopt if wrapped).
        virtual auto add(const Concrete& address) const -> std::optional<Concrete>
        {
            return add(address.data());
        }

        // Subtracts a value from the address (difference is wrapped).
        virtual auto operator-(const Size) const noexcept -> Concrete = 0;

        // Subtracts two addresses from each other (difference is wrapped).
        virtual auto operator-(const Concrete&) const noexcept -> Concrete = 0;

        // Subtracts a value from the address (nullopt if wrapped).
        virtual auto subtract(const Size value) const -> std::optional<Concrete>
        {
            if (data() < value)
                return std::nullopt;

            return *this - value;
        }

        // Subtracts two addresses from each other (nullopt if wrapped).
        virtual auto subtract(const Concrete& address) const -> std::optional<Concrete>
        {
            return subtract(address.data());
        }

        // Aligns the address to a power of 2.
        virtual auto align(const Size) noexcept -> Concrete& = 0;
};

//template<typename Derived>
//class EnableDownCast
//{
    //using Base = EnableDownCast;

    //protected:
        //// Disable deletion of Derived* through Base*.
        //// Enable deletion of Base* through Derived*.
        //~EnableDownCast() = default;
    //public:
        //[[nodiscard]] constexpr auto self() const noexcept -> const Derived*
        //{
            //return static_cast<const Derived*>(this);
        //}

        //constexpr auto self() noexcept -> Derived*
        //{
            //return static_cast<Derived*>(this);
        //}
//};

//template<typename Concrete, typename Size>
//class Address : EnableDownCast<Concrete>
//{
    //using EnableDownCast<Concrete>::self;

    //public:
        ////Address() = default;
        ////~Address() = default;

        ////Address(const Address&) = default;
        ////Address& operator=(const Address&) = default;
        ////Address(Address&&) = default;
        ////Address& operator=(Address&&) = default;

        //// Returns the raw value of the address.
        //auto data() const noexcept -> Size
        //{
            //return self()->data();
        //}

        //// Returns a bitwise AND of the address and a mask.
        //auto mask() const noexcept -> Size
        //{
            //return self()->mask();
        //}

        //// Adds `other` to the address' value.
        //auto operator+(const Size offset) const noexcept -> Concrete
        //{
            //return self()->operator+(offset);
        //}

        //// Subtracts `other` from the address' value.
        //auto operator-(const Size value) const noexcept -> Concrete
        //{
            //return self()->operator-(value);
        //}

        //auto operator-(const Concrete& addr) const noexcept -> Concrete
        //{
            //return self()->operator-(addr);
        //}

        //// Adds `other` to the address' value.
        ////
        //// Returns a pair consisting of the sum and a boolean indicating
        //// whether an overflow would have occurred. If an overflow would have
        //// occurred, the wrapped address is returned.
        //auto add(const Size offset) const -> Concrete
        //{
            //return self()->add(offset);
        //}

        //auto add(const Concrete& addr) const -> Concrete
        //{
            //return self()->add(addr);
        //}

            //// Subtracts `other` from the address' value.
            ////
            //// Returns a pair containing the difference and a boolean indicating
            //// whether an arithmetic underflow would have occurred. If an overflow
            //// would have occurred, then the wrapped address is returned.
        //auto subtract(const Size value) const -> Concrete
        //{
            //return self()->subtract(value);
        //}

        //auto subtract(const Concrete& addr) const -> Concrete
        //{
            //return self()->subtract(addr);
        //}

        //// Returns the offset from `m_addr` to `base`.
        ////virtual auto offset_from(const Concrete&) const noexcept -> Concrete;
        ////virtual auto checked_offset_from(const Concrete&) const -> Concrete;

        //// Aligns the address to a power of 2.
        //auto align_up(const Size alignment) noexcept -> void
        //{
            //return self()->align_up(alignment);
        //}

        ////auto checked_align_up(const Size alignment) -> void
        ////{
            ////return self()->checked_align_up(alignment);
        ////}
//};

}  // vmm::memory::detail
