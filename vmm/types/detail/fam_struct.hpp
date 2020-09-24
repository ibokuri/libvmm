#pragma once

#include <cstddef>
#include <array>

/**
 * NOTE: DEPRECATED
 *
 * Buffer size computation (general form)
 *
 *     sizeof(struct_type) + N * sizeof(value_type)
 *
 * The left-hand side of the sum computes the size needed to hold the header.
 * The right-hand side of the sum computes the size needed to hold N entries.
 */
template<typename Struct, typename Entry, std::size_t N>
class fam_struct {
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
            static_assert(pos < N);
            return this->entries_[pos];
        }

        [[nodiscard]] auto operator[](std::size_t pos) const noexcept -> const_reference {
            static_assert(pos < N);
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
