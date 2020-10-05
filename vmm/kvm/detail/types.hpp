/*
 * types.hpp - KVM types
 */

#pragma once

#include <algorithm> // copy
#include <cstddef> // byte, size_t
#include <cstring> // memcpy, memset
#include <iterator> // distance
#include <initializer_list> // initializer_list
#include <linux/kvm.h> // kvm_*
#include <memory_resource> // polymorphic_allocator

#include "vmm/kvm/detail/macros.hpp"
#include "vmm/types/file_descriptor.hpp"

namespace vmm::kvm::detail {

class system;

/**
 * Base class for KVM file descriptors.
 */
class KvmFd : public vmm::types::FileDescriptor {
    public:
        explicit KvmFd(int fd) noexcept : vmm::types::FileDescriptor(fd) {}

        KvmFd(const KvmFd& other) = delete;
        KvmFd(KvmFd&& other) = default;
        auto operator=(const KvmFd& other) -> KvmFd& = delete;
        auto operator=(KvmFd&& other) -> KvmFd& = default;
};

/**
 * Wrapper for KVM FAM structs.
 *
 * This class should not be used for arbitrary FAM structs. It is only meant to
 * be used with KVM FAM structs, which have certain properties:
 *
 *     * Any required padding is explicitly provided.
 *
 *     * The size field is of type __u32 and is the first field in the struct.
 *
 *     * The struct only contains POD types.
 *
 * Size: type of size member (kvm_reg_list has __u64 size member);
 */
template<typename Struct, typename Entry, std::size_t N, typename Size=uint32_t>
class FamStruct {
    public:
        using value_type = Entry;
        using size_type = std::size_t;
        using allocator_type = std::pmr::polymorphic_allocator<std::byte>;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;
        using iterator = pointer;
        using const_iterator = const_pointer;

        static const auto alignment = alignof(Struct);
        static const auto storage_size = sizeof(Struct) + N * sizeof(Entry);

        // Allocator constructor
        explicit FamStruct(const allocator_type& alloc)
                : m_alloc{alloc},
                  m_ptr{static_cast<Struct*>( alloc.resource()->allocate(storage_size, alignment))} {
            static_assert(N >= 0);
            std::memset(m_ptr, 0, storage_size);
            *m_ptr = {N};
        }

        // Default constructor
        //
        // NOTE: We cannot pass m_alloc to the allocator constructor as it
        //       would be passed in as const, meaning that it can't be changed.
        //       However, it *is* changed in the member initializer list, which
        //       results in a segfault.
        FamStruct() : FamStruct(std::pmr::new_delete_resource()) {}

        // Single entry constructors
        explicit FamStruct(const_reference entry) noexcept : FamStruct() {
            std::memcpy(begin(), entry, sizeof(value_type));
        }

        explicit FamStruct(const_reference entry, const allocator_type& alloc) noexcept : FamStruct(alloc) {
            std::memcpy(begin(), entry, sizeof(value_type));
        }

        // Copy constructors
        constexpr FamStruct(const FamStruct& other)
                : FamStruct(other.begin(), other.end(), other.get_allocator()) {}

        constexpr FamStruct(const FamStruct& other, const allocator_type& alloc)
                : FamStruct(other.begin(), other.end(), alloc) {}

        // Move constructors
        //constexpr FamStruct(FamStruct&& other)
            //: m_entries{std::move(other.m_entries)} {}

        //constexpr FamStruct(FamStruct&& other, const allocator_type& alloc)
                //: m_entries{std::move(other.m_entries), alloc} {}

        // Iterator constructors
        // TODO: SFINAE
        // TODO: Cleaner distance()
        template <typename InputIt>
        explicit FamStruct(InputIt first, InputIt last) : FamStruct() {
            Size n = std::distance(first, last);
            *m_ptr = {n};
            std::copy(first, last, begin());
        }

        template <typename InputIt>
        explicit FamStruct(InputIt first, InputIt last, const allocator_type& alloc) : FamStruct(alloc) {
            Size n = std::distance(first, last);
            *m_ptr = {n};
            std::copy(first, last, begin());
        }

        // Initializer list constructors
        explicit FamStruct(std::initializer_list<value_type> ilist) : FamStruct(ilist.begin(), ilist.end()) {}
        explicit FamStruct(std::initializer_list<value_type> ilist, const allocator_type& alloc)
                : FamStruct(ilist.begin(), ilist.end(), alloc) {}

        // Destructor
        ~FamStruct() {
            m_alloc.resource()->deallocate(m_ptr, storage_size, alignment);
        }

        // Allocator
        auto get_allocator() const noexcept -> allocator_type { return m_alloc; }

        // Element access
        [[nodiscard]] auto operator[](Size pos) noexcept -> reference { return *(begin() + pos); }
        [[nodiscard]] auto operator[](Size pos) const noexcept -> const_reference { return *(begin() + pos); }

        [[nodiscard]] auto front() noexcept -> reference { return *begin(); }
        [[nodiscard]] auto front() const noexcept -> const_reference { return *begin(); }

        [[nodiscard]] auto back() noexcept -> reference { return *end(); }
        [[nodiscard]] auto back() const noexcept -> const_reference { return *end(); }

        [[nodiscard]] constexpr auto data() noexcept -> Struct* { return m_ptr; }
        [[nodiscard]] constexpr auto data() const noexcept -> const Struct* { return m_ptr; }

        // Iterators
        auto begin() noexcept -> iterator {
           iterator it = nullptr;
           std::memcpy(&it, &m_ptr, sizeof(iterator));
           return it + sizeof(Struct);
        }

        auto begin() const noexcept -> const_iterator {
            iterator it = nullptr;
            std::memcpy(&it, &m_ptr, sizeof(iterator));
            return it + sizeof(Struct);
        }

        auto end() noexcept -> iterator { return begin() + size(); }
        auto end() const noexcept -> const_iterator { return begin() + size(); }
        auto cbegin() const noexcept -> const_iterator { return begin(); }
        auto cend() const noexcept -> const_iterator { return end(); }

        // Capacity
        [[nodiscard]] auto size() const noexcept -> const Size {
            Size size = 0;
            std::memcpy(&size, m_ptr, sizeof(Size));
            return size;
        }

        [[nodiscard]] auto empty() const noexcept -> bool { return size() == 0; }

        [[nodiscard]] constexpr auto capacity() const noexcept -> size_type { return N; }
    private:
        allocator_type m_alloc;
        Struct *m_ptr = nullptr;
};

/**
 * struct kvm_msr_list {
 *    __u32 nmsrs;
 *    __u32 indices[0];
 * };
 *
 * NOTE: Because MsrList's value_type is an integer, it is very easy to confuse
 *       the size_type initializer_list and constructor. That is, one may think
 *       that `auto msr_list = MsrList{10}` constructs a FAM struct with enough
 *       space for 10 entries, but really it constructs a FAM struct with an
 *       initializer list, resulting in a FAM struct with only 1 entry with a
 *       value of 10. To use the size_type constructor, use `MsrList(10)`.
 */
template<std::size_t N>
class MsrList : public FamStruct<kvm_msr_list, uint32_t, N> {
    using Base = typename FamStruct<kvm_msr_list, uint32_t, N>::FamStruct;
    using Base::Base;
    using allocator_type = typename Base::allocator_type;

    private:
        friend system;

        MsrList() : Base::FamStruct() {}
        explicit MsrList(const allocator_type& alloc) : Base::FamStruct(alloc) {}
};

/**
 * struct kvm_msrs {
 *     __u32 nmsrs;
 *     __u32 pad;
 *     struct kvm_msr_entry entries[0];
 * };
 *
 * struct kvm_msr_entry {
 *     __u32 index;
 *     __u32 reserved;
 *     __u64 data;
 * };
 */
template<std::size_t N>
class Msrs : public FamStruct<kvm_msrs, kvm_msr_entry, N> {
    using FamStruct<kvm_msrs, kvm_msr_entry, N>::FamStruct;
};

/*
 * struct kvm_cpuid2 {
 *     __u32 nent;
 *     __u32 padding;
 *     struct kvm_cpuid_entry2 entries[0];
 * };
 *
 * struct kvm_cpuid_entry2 {
 *     __u32 function;
 *     __u32 index;
 *     __u32 flags;
 *     __u32 eax;
 *     __u32 ebx;
 *     __u32 ecx;
 *     __u32 edx;
 *     __u32 padding[3];
 * };
 */
template<std::size_t N>
class Cpuids : public FamStruct<kvm_cpuid2, kvm_cpuid_entry2, N> {
    using FamStruct<kvm_cpuid2, kvm_cpuid_entry2, N>::FamStruct;
};

/**
 * struct kvm_irq_routing {
 *       __u32 nr;
 *       __u32 flags;
 *       struct kvm_irq_routing_entry entries[0];
 * };
 *
 * struct kvm_irq_routing_entry {
 *       __u32 gsi;
 *       __u32 type;
 *       __u32 flags;
 *       __u32 pad;
 *       union {
 *               struct kvm_irq_routing_irqchip irqchip;
 *               struct kvm_irq_routing_msi msi;
 *               struct kvm_irq_routing_s390_adapter adapter;
 *               struct kvm_irq_routing_hv_sint hv_sint;
 *               __u32 pad[8];
 *       } u;
 * };
 */
template<std::size_t N>
class IrqRouting : public FamStruct<kvm_irq_routing, kvm_irq_routing_entry, N> {
    using FamStruct<kvm_irq_routing, kvm_irq_routing_entry, N>::FamStruct;
};

}  // namespace vmm::kvm::detail
