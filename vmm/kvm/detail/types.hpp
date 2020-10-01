/*
 * types.hpp - KVM types
 */

#pragma once

#include <algorithm> // copy_if
#include <cstddef> // size_t
#include <cstring> // memcpy, memset
#include <iterator> // distance
#include <initializer_list> // initializer_list
#include <linux/kvm.h> // kvm_*
#include <memory> // allocator_traits
#include <memory_resource> // polymorphic_allocator
#include <stdexcept> // length_error

#include "vmm/kvm/detail/macros.hpp"
#include "vmm/types/file_descriptor.hpp"

namespace vmm::kvm::detail {

class system;

/**
 * Base class for KVM file descriptors.
 */
class KvmFd : public vmm::types::FileDescriptor {
    public:
        KvmFd(int fd) noexcept : vmm::types::FileDescriptor(fd) {}

        KvmFd(const KvmFd& other) = delete;
        KvmFd(KvmFd&& other) = default;
        auto operator=(const KvmFd& other) -> KvmFd& = delete;
        auto operator=(KvmFd&& other) -> KvmFd& = default;
};

/**
 * Wrapper for KVM FAM structs.
 *
 * This class should not be used for arbitrary FAM structs. It is only meant to
 * be used with KVM FAM structs, which imply certain properties:
 *
 *     * No padding is provided in FamStruct as KVM provides it explicitly
 *       where needed.
 *
 *     * FamStruct ensures that the struct is able to be passed to KVM's API.
 */
template<typename Struct, typename Entry, std::size_t N>
class FamStruct {
    protected:
        using allocator_type = std::pmr::polymorphic_allocator<std::byte>;
        using size_type = decltype(N);
        using value_type = Entry;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;
        using iterator = pointer;
        using const_iterator = const_pointer;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        allocator_type m_alloc;
        Struct *m_ptr;
    private:
        [[nodiscard]] virtual auto entries() noexcept -> pointer = 0;
        [[nodiscard]] virtual auto entries() const noexcept -> const_pointer = 0;
    public:
        static const auto alignment = alignof(Struct);
        static const auto storage_size = sizeof(Struct) + N * sizeof(Entry);

        // Allocator constructor
        explicit FamStruct(const allocator_type& alloc)
            : m_alloc{alloc},
              m_ptr{static_cast<Struct*>(m_alloc.resource()->allocate(storage_size, alignment))} {
            std::memset(m_ptr, 0, storage_size);
        }

        // Default constructor
        FamStruct() : FamStruct(std::pmr::new_delete_resource()) {}

        // Copy constructor and assignment operator
        FamStruct(const FamStruct& other) = delete;
        FamStruct(const FamStruct& other, const allocator_type& alloc) = delete;
        void operator=(const FamStruct&) = delete;

        // Move constructor and assignment operator
        FamStruct(FamStruct&& other) = delete;
        FamStruct(FamStruct&& other, const allocator_type& alloc) = delete;
        void operator=(FamStruct&&) = delete;

        // Destructor
        virtual ~FamStruct() {
            m_alloc.deallocate(reinterpret_cast<std::byte*>(m_ptr), storage_size);
        }

        // Element access
        [[nodiscard]] auto operator[](std::size_t pos) noexcept -> reference {
            return entries()[pos];
        }

        [[nodiscard]] auto operator[](std::size_t pos) const noexcept -> const_reference {
            return entries()[pos];
        }

        [[nodiscard]] auto data() noexcept -> Struct* {
            return m_ptr;
        }

        [[nodiscard]] auto data() const noexcept -> const Struct* {
            return m_ptr;
        }

        // Capacity
        [[nodiscard]] virtual auto size() const noexcept -> const size_type = 0;

        [[nodiscard]] auto empty() const noexcept -> bool {
            return size() == 0;
        }

        [[nodiscard]] constexpr auto max_size() const noexcept -> size_type {
            return N;
        }

        // Iterators
        auto begin() noexcept -> iterator {
            return entries();
        }

        auto end() noexcept -> iterator {
            return entries() + size();
        }

        auto begin() const noexcept -> const_iterator {
            return entries();
        }

        auto end() const noexcept -> const_iterator {
            return entries() + size();
        }

        auto cbegin() const noexcept -> const_iterator {
            return begin();
        }

        auto cend() const noexcept -> const_iterator {
            return end();
        }
};

/**
 * struct kvm_msr_list {
 *    __u32 nmsrs;
 *    __u32 indices[0];
 * };
 */
template<std::size_t N>
class MsrList : public FamStruct<kvm_msr_list, uint32_t, N> {
    using Base = FamStruct<kvm_msr_list, uint32_t, N>;
    using Base::Base;

    public:
        using allocator_type = typename Base::allocator_type;
        using value_type = typename Base::value_type;
        using size_type = typename Base::size_type;
        using pointer = typename Base::pointer;
        using const_pointer = typename Base::const_pointer;

        static const auto alignment = Base::alignment;
        static const auto storage_size = Base::storage_size;

        // Capacity
        [[nodiscard]] auto size() const noexcept -> const size_type override {
            return Base::m_ptr->nmsrs;
        }
    private:
        friend system;

        [[nodiscard]] auto entries() noexcept -> pointer override {
            return Base::m_ptr->indices;
        }

        [[nodiscard]] auto entries() const noexcept -> const_pointer override {
            return Base::m_ptr->indices;
        }

        MsrList(const MsrList& other) : Base() {
            Base::m_ptr->nmsrs = other.m_ptr->nmsrs;
            std::copy(other.begin(), other.end(), Base::begin());
        }
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
    using Base = FamStruct<kvm_msrs, kvm_msr_entry, N>;
    using Base::Base;

    public:
        using allocator_type = typename Base::allocator_type;
        using value_type = typename Base::value_type;
        using size_type = typename Base::size_type;
        using pointer = typename Base::pointer;
        using const_pointer = typename Base::const_pointer;

        static const auto alignment = Base::alignment;
        static const auto storage_size = Base::storage_size;

        Msrs(const Msrs& other) : Base(other.m_alloc) {
            Base::m_ptr->nmsrs = other.m_ptr->nmsrs;
            std::copy(other.begin(), other.end(), Base::begin());
        }

        explicit Msrs(value_type entry) : Base() {
            static_assert(N == 1);
            Base::m_ptr->nmsrs = N;
            std::memcpy(Base::m_ptr->entries, &entry, sizeof(value_type));
        }

        explicit Msrs(value_type entry, const allocator_type& alloc) : Base(alloc) {
            static_assert(N == 1);
            Base::m_ptr->nmsrs = N;
            std::memcpy(Base::m_ptr->entries, &entry, sizeof(value_type));
        }

        explicit Msrs(std::initializer_list<value_type> l) : Base() {
            if (Base::m_ptr->nmsrs = l.size(); Base::m_ptr->nmsrs > N)
                VMM_THROW(std::length_error("Initializer list size too big"));

            std::copy(l.begin(), l.end(), Base::begin());
        }

        explicit Msrs(std::initializer_list<value_type> l, const allocator_type& alloc) : Base(alloc) {
            if (Base::m_ptr->nmsrs = l.size(); Base::m_ptr->nmsrs > N)
                VMM_THROW(std::length_error("Initializer list size too big"));

            std::copy(l.begin(), l.end(), Base::begin());
        }

        template <typename Iterator>
        explicit Msrs(Iterator first, Iterator last) : Msrs() {
            if (Base::m_ptr->nmsrs = std::distance(first, last); Base::m_ptr->nmsrs > N)
                VMM_THROW(std::length_error("Iterator range too big"));

            std::copy_if(first, last, Base::begin(), [](value_type) { return true; });
        }

        template <typename Iterator>
        explicit Msrs(Iterator first, Iterator last, const allocator_type& alloc) : Msrs(alloc) {
            if (Base::m_ptr->nmsrs = std::distance(first, last); Base::m_ptr->nmsrs > N)
                VMM_THROW(std::length_error("Iterator range too big"));

            std::copy_if(first, last, Base::begin(), [](value_type) { return true; });
        }

        // Capacity
        [[nodiscard]] auto size() const noexcept -> const size_type override {
            return Base::m_ptr->nmsrs;
        }
    private:
        [[nodiscard]] auto entries() noexcept -> pointer override {
            return Base::m_ptr->entries;
        }

        [[nodiscard]] auto entries() const noexcept -> const_pointer override {
            return Base::m_ptr->entries;
        }
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
    using Base = FamStruct<kvm_cpuid2, kvm_cpuid_entry2, N>;
    using Base::Base;

    public:
        using allocator_type = typename Base::allocator_type;
        using value_type = typename Base::value_type;
        using size_type = typename Base::size_type;
        using pointer = typename Base::pointer;
        using const_pointer = typename Base::const_pointer;

        static const auto alignment = Base::alignment;
        static const auto storage_size = Base::storage_size;

        Cpuids(const Cpuids& other) : Base(other.m_alloc) {
            Base::m_ptr->nent = other.m_ptr->nent;
            std::copy(other.begin(), other.end(), Base::begin());
        }

        explicit Cpuids(value_type entry) : Base() {
            static_assert(N == 1);
            Base::m_ptr->nent = 1;
            std::memcpy(Base::m_ptr->entries, &entry, sizeof(value_type));
        }

        explicit Cpuids(value_type entry, const allocator_type& alloc) : Base(alloc) {
            static_assert(N == 1);
            Base::m_ptr->nent = 1;
            std::memcpy(Base::m_ptr->entries, &entry, sizeof(value_type));
        }

        explicit Cpuids(std::initializer_list<value_type> l) : Base() {
            if (Base::m_ptr->nent = l.size(); Base::m_ptr->nent > N)
                VMM_THROW(std::length_error("Iterator range too big"));

            std::copy(l.begin(), l.end(), Base::begin());
        }

        explicit Cpuids(std::initializer_list<value_type> l, const allocator_type& alloc) : Base(alloc) {
            if (Base::m_ptr->nent = l.size(); Base::m_ptr->nent > N)
                VMM_THROW(std::length_error("Iterator range too big"));

            std::copy(l.begin(), l.end(), Base::begin());
        }

        template <typename Iterator>
        explicit Cpuids(Iterator first, Iterator last) : Base() {
            if (Base::m_ptr->nent = std::distance(first, last); Base::m_ptr->nent > N)
                VMM_THROW(std::length_error("Iterator range too big"));

            std::copy_if(first, last, Base::begin(), [](value_type) { return true; });
        }

        template <typename Iterator>
        explicit Cpuids(Iterator first, Iterator last, const allocator_type& alloc) : Base(alloc) {
            if (Base::m_ptr->nent = std::distance(first, last); Base::m_ptr->nent > N)
                VMM_THROW(std::length_error("Iterator range too big"));

            std::copy_if(first, last, Base::begin(), [](value_type) { return true; });
        }

        // Capacity
        [[nodiscard]] auto size() const noexcept -> const size_type override {
            return Base::m_ptr->nent;
        }
    private:
        [[nodiscard]] auto entries() noexcept -> pointer override {
            return Base::m_ptr->entries;
        }

        [[nodiscard]] auto entries() const noexcept -> const_pointer override {
            return Base::m_ptr->entries;
        }
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
    using Base = FamStruct<kvm_irq_routing, kvm_irq_routing_entry, N>;
    using Base::Base;

    public:
        using allocator_type = typename Base::allocator_type;
        using value_type = typename Base::value_type;
        using size_type = typename Base::size_type;
        using pointer = typename Base::pointer;
        using const_pointer = typename Base::const_pointer;

        static const auto alignment = Base::alignment;
        static const auto storage_size = Base::storage_size;

        IrqRouting(const IrqRouting& other) : Base(other.m_alloc) {
            Base::m_ptr->nr = other.m_ptr->nr;
            std::copy(other.begin(), other.end(), Base::begin());
        }

        explicit IrqRouting(value_type entry) : Base() {
            static_assert(N == 1);
            Base::m_ptr->nr = 1;
            std::memcpy(Base::m_ptr->entries, &entry, sizeof(value_type));
        }

        explicit IrqRouting(value_type entry, const allocator_type& alloc) : Base(alloc) {
            static_assert(N == 1);
            Base::m_ptr->nr = 1;
            std::memcpy(Base::m_ptr->entries, &entry, sizeof(value_type));
        }

        explicit IrqRouting(std::initializer_list<value_type> l) : Base() {
            if (Base::m_ptr->nr = l.size(); Base::m_ptr->nr > N)
                VMM_THROW(std::length_error("Iterator range too big"));

            std::copy(l.begin(), l.end(), Base::begin());
        }

        explicit IrqRouting(std::initializer_list<value_type> l, const allocator_type& alloc) : Base(alloc) {
            if (Base::m_ptr->nr = l.size(); Base::m_ptr->nr > N)
                VMM_THROW(std::length_error("Iterator range too big"));

            std::copy(l.begin(), l.end(), Base::begin());
        }

        template <typename Iterator>
        explicit IrqRouting(Iterator first, Iterator last) : Base() {
            if (Base::m_ptr->nr = std::distance(first, last); Base::m_ptr->nr > N)
                VMM_THROW(std::length_error("Iterator range too big"));

            std::copy_if(first, last, Base::begin(), [](value_type) { return true; });
        }

        template <typename Iterator>
        explicit IrqRouting(Iterator first, Iterator last, const allocator_type& alloc) : Base(alloc) {
            if (Base::m_ptr->nr = std::distance(first, last); Base::m_ptr->nr > N)
                VMM_THROW(std::length_error("Iterator range too big"));

            std::copy_if(first, last, Base::begin(), [](value_type) { return true; });
        }

        // Capacity
        [[nodiscard]] auto size() const noexcept -> const size_type override {
            return Base::m_ptr->nr;
        }
    private:
        [[nodiscard]] auto entries() noexcept -> pointer override {
            return Base::m_ptr->entries;
        }

        [[nodiscard]] auto entries() const noexcept -> const_pointer override {
            return Base::m_ptr->entries;
        }
};

}  // namespace vmm::kvm::detail
