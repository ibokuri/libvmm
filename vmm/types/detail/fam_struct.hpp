#pragma once

#include <memory> // new/delete, unique_ptr

/**
 * Basic wrapper around C FAM structs.
 */
template<typename Struct, typename Buffer, typename Entry>
class FamStruct {
    protected:
        using value_type = Entry;
        using allocator_type = std::allocator<Entry>;
        using pointer = typename std::allocator_traits<allocator_type>::pointer;
        using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;

        using reference = value_type&;
        using const_reference = const value_type&;
        using iterator = pointer;
        using const_iterator = const_pointer;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        std::unique_ptr<Struct, void(*)(Struct*)> ptr_;

        FamStruct(size_t n) : ptr_{reinterpret_cast<Struct*>(new Buffer[n]()),
                                   [](Struct *p){ delete[] reinterpret_cast<Buffer*>(p); }} {}
    public:
        [[nodiscard]] auto get() -> Struct* { return ptr_.get(); }
};
