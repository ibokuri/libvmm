#pragma once

#include <cstddef> // size_t
#include <array> // array

/*
 * A wrapper for FAM structs used by KVM.
 *
 * This class should not be used for arbitrary FAM structs. It is only meant to
 * be used with KVM structs, which imply certain properties:
 *
 *     * No padding is provided in FamStruct as KVM provides it explicitly
 *       where needed.
 *
 *     * FamStruct ensures that the struct is able to be passed to KVM's API.
 */
template<typename Struct, typename Entry, std::size_t N>
class FamStruct {
    protected:
        using value_type = Entry;
        using size_type = std::size_t;

        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;
        using iterator = pointer;
        using const_iterator = const_pointer;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        Struct struct_ = {};
        std::array<value_type, N> entries_ = {};
    public:
        // Iterators
        auto begin() noexcept -> iterator {
            return this->entries_.begin();
        }

        auto end() noexcept -> iterator {
            return this->entries_.end();
        }

        auto begin() const noexcept -> const_iterator {
            return this->entries_.begin()();
        }

        auto end() const noexcept -> const_iterator {
            return this->entries_.end();
        }

        auto cbegin() const noexcept -> const_iterator {
            return begin();
        }

        auto cend() const noexcept -> const_iterator {
            return end();
        }

        // Element access
        [[nodiscard]] auto operator[](std::size_t pos) noexcept -> reference {
            return this->entries_[pos];
        }

        [[nodiscard]] auto operator[](std::size_t pos) const noexcept -> const_reference {
            return this->entries_[pos];
        }

        [[nodiscard]] auto data() noexcept -> Struct* {
            return &this->struct_;
        }

        [[nodiscard]] auto data() const noexcept -> const Struct* {
            return &this->struct_;
        }

        // Capacity
        [[nodiscard]] constexpr auto max_size() const noexcept -> size_type {
            return N;
        }
};
