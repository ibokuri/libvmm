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
    public:
        static const auto alignment = alignof(Struct);
        static const auto storage_size = sizeof(Struct) + N * sizeof(Entry);

        // Allocator constructor
        explicit FamStruct(const allocator_type& alloc)
                : m_alloc{alloc},
                  m_ptr{static_cast<Struct*>(m_alloc.resource()->allocate(storage_size, alignment))} {
            // construction for our POD struct(s)
            std::memset(m_ptr, 0, storage_size);
        }

        // Default constructor
        explicit FamStruct() : FamStruct(std::pmr::new_delete_resource()) {}

        // Copy constructor and assignment operator
        FamStruct(const FamStruct& other) = delete;
        FamStruct(const FamStruct& other, const allocator_type& alloc) = delete;
        void operator=(const FamStruct&) = delete;

        // Move constructor and assignment operator
        FamStruct(FamStruct&& other) = delete;
        FamStruct(FamStruct&& other, const allocator_type& alloc) = delete;
        void operator=(FamStruct&&) = delete;

        // Range constructor
        //template<typename InputIt>
        //FamStruct(InputIt first, InputIt last, const allocator_type& alloc={});

        // Initializer list constructor and assignment operator
        //FamStruct(std::initializer_list<Entry> init, const allocator_type& alloc={})
              //: FamStruct(init.begin(), init.end(), alloc) {}

        //FamStruct& operator=(std::initializer_list<Entry> init);

        // Destructor
        ~FamStruct() {
            m_alloc.deallocate(reinterpret_cast<std::byte*>(m_ptr), storage_size);
        }

        // Element access
        [[nodiscard]] auto data() noexcept -> Struct* {
            return m_ptr;
        }

        [[nodiscard]] auto data() const noexcept -> const Struct* {
            return m_ptr;
        }

        // Capacity
        [[nodiscard]] constexpr auto max_size() const noexcept -> size_type {
            return N;
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

    public:
        using allocator_type = typename Base::allocator_type;
        using value_type = typename Base::value_type;
        using size_type = typename Base::size_type;
        using pointer = typename Base::pointer;
        using const_pointer = typename Base::const_pointer;
        using reference = typename Base::reference;
        using const_reference = typename Base::const_reference;
        using iterator = typename Base::iterator;
        using const_iterator = typename Base::const_iterator;

        static const auto alignment = Base::alignment;
        static const auto storage_size = Base::storage_size;

        // TODO: Move constructor

        // TODO: Allocator methods

        // Element access
        [[nodiscard]] auto operator[](std::size_t pos) noexcept -> reference {
            return entries()[pos];
        }

        [[nodiscard]] auto operator[](std::size_t pos) const noexcept -> const_reference {
            return entries()[pos];
        }

        // Capacity
        [[nodiscard]] auto size() const noexcept -> size_type {
            return Base::m_ptr->nmsrs;
        }

        [[nodiscard]] auto empty() const noexcept -> bool {
            return size() == 0;
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
    private:
        friend system;

        MsrList() noexcept {
            Base::m_ptr->nmsrs = N;
        };

        MsrList(const MsrList& other) {
            std::copy(other.begin(), other.end(), begin());
        };

        MsrList(const allocator_type& alloc) : Base(alloc) {
            Base::m_ptr->nr = N;
        };

        [[nodiscard]] auto entries() noexcept -> pointer {
            return Base::m_ptr->indices;
        }

        [[nodiscard]] auto entries() const noexcept -> const_pointer {
            return Base::m_ptr->indices;
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

    public:
        using allocator_type = typename Base::allocator_type;
        using value_type = typename Base::value_type;
        using size_type = typename Base::size_type;
        using pointer = typename Base::pointer;
        using const_pointer = typename Base::const_pointer;
        using reference = typename Base::reference;
        using const_reference = typename Base::const_reference;
        using iterator = typename Base::iterator;
        using const_iterator = typename Base::const_iterator;

        static const auto alignment = Base::alignment;
        static const auto storage_size = Base::storage_size;

        Msrs() noexcept {
            Base::m_ptr->nmsrs = N;
        };

        // FIXME: Why is Msrs the only one with an allocator constructor?
        Msrs(const allocator_type& alloc) : Base(alloc) {
            Base::m_ptr->nmsrs = N;
        };

        Msrs(const Msrs& other) : Msrs(other.m_alloc) {
            std::copy(other.begin(), other.end(), begin());
        };

        explicit Msrs(value_type entry) noexcept : Msrs() {
            static_assert(N == 1);

            std::memcpy(Base::m_ptr->entries, &entry, sizeof(value_type));
        }

        explicit Msrs(value_type entry, const allocator_type& alloc) noexcept : Msrs(alloc) {
            static_assert(N == 1);

            std::memcpy(Base::m_ptr->entries, &entry, sizeof(value_type));
        }

        template <typename Iterator>
        explicit Msrs(Iterator first, Iterator last) : Msrs() {
            std::copy_if(first, last, begin(), [](value_type) { return true; });
        }

        template <typename Iterator>
        explicit Msrs(Iterator first, Iterator last, const allocator_type& alloc) : Msrs(alloc) {
            std::copy_if(first, last, begin(), [](value_type) { return true; });
        }

        //template <typename Container>
        //explicit Msrs(Container& c) : Msrs(c.begin(), c.end()) {}

        // Element access
        [[nodiscard]] auto operator[](std::size_t pos) noexcept -> reference {
            return entries()[pos];
        }

        [[nodiscard]] auto operator[](std::size_t pos) const noexcept -> const_reference {
            return entries()[pos];
        }

        // Capacity
        [[nodiscard]] auto size() const noexcept -> size_type {
            return Base::m_ptr->nmsrs;
        }

        [[nodiscard]] auto empty() const noexcept -> bool {
            return size() == 0;
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
    private:
        [[nodiscard]] auto entries() noexcept -> pointer {
            return Base::m_ptr->entries;
        }

        [[nodiscard]] auto entries() const noexcept -> const_pointer {
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

    public:
        using allocator_type = typename Base::allocator_type;
        using value_type = typename Base::value_type;
        using size_type = typename Base::size_type;
        using pointer = typename Base::pointer;
        using const_pointer = typename Base::const_pointer;
        using reference = typename Base::reference;
        using const_reference = typename Base::const_reference;
        using iterator = typename Base::iterator;
        using const_iterator = typename Base::const_iterator;

        static const auto alignment = Base::alignment;
        static const auto storage_size = Base::storage_size;

        Cpuids() noexcept {
            Base::m_ptr->nent = N;
        }

        Cpuids(const allocator_type& alloc) : Base(alloc) {
            Base::m_ptr->nent = N;
        };

        Cpuids(const Cpuids& other) {
            std::copy(other.begin(), other.end(), begin());
        };

        explicit Cpuids(value_type entry) noexcept {
            static_assert(N == 1);

            std::memcpy(Base::m_ptr->entries, &entry, sizeof(value_type));
            Base::m_ptr->nent = N;
        }

        template <typename Iterator>
        explicit Cpuids(Iterator first, Iterator last) {
            std::copy_if(first, last, begin(), [](value_type) { return true; });
            Base::m_ptr->nent = std::distance(first, last);
        }

        template <typename Container>
        explicit Cpuids(Container& c) : Cpuids(c.begin(), c.end()) {}

        // Element access
        [[nodiscard]] auto operator[](std::size_t pos) noexcept -> reference {
            return entries()[pos];
        }

        [[nodiscard]] auto operator[](std::size_t pos) const noexcept -> const_reference {
            return entries()[pos];
        }

        // Capacity
        [[nodiscard]] auto size() const noexcept -> size_type {
            return Base::m_ptr->nent;
        }

        [[nodiscard]] auto empty() const noexcept -> bool {
            return size() == 0;
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
    private:
        [[nodiscard]] auto entries() noexcept -> pointer {
            return Base::m_ptr->entries;
        }

        [[nodiscard]] auto entries() const noexcept -> const_pointer {
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

    public:
        using allocator_type = typename Base::allocator_type;
        using value_type = typename Base::value_type;
        using size_type = typename Base::size_type;
        using pointer = typename Base::pointer;
        using const_pointer = typename Base::const_pointer;
        using reference = typename Base::reference;
        using const_reference = typename Base::const_reference;
        using iterator = typename Base::iterator;
        using const_iterator = typename Base::const_iterator;

        static const auto alignment = Base::alignment;
        static const auto storage_size = Base::storage_size;

        IrqRouting() noexcept {
            Base::m_ptr->nr = N;
        };

        IrqRouting(const allocator_type& alloc) : Base(alloc) {
            Base::m_ptr->nr = N;
        };

        explicit IrqRouting(value_type entry) noexcept {
            static_assert(N == 1);

            std::memcpy(Base::m_ptr->entries, &entry, sizeof(value_type));
            Base::m_ptr->nr = N;
        }

        template <typename Iterator>
        explicit IrqRouting(Iterator first, Iterator last) {
            std::copy_if(first, last, begin(), [](value_type) {return true;});
            Base::m_ptr->nr = N;
        }

        template <typename Container>
        explicit IrqRouting(Container& c) : IrqRouting(c.begin(), c.end()) {}

        // Element access
        [[nodiscard]] auto operator[](std::size_t pos) noexcept -> reference {
            return entries()[pos];
        }

        [[nodiscard]] auto operator[](std::size_t pos) const noexcept -> const_reference {
            return entries()[pos];
        }

        // Capacity
        [[nodiscard]] auto size() const noexcept -> size_type {
            return Base::m_ptr->nr;
        }

        [[nodiscard]] auto empty() const noexcept -> bool {
            return size() == 0;
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
    private:
        [[nodiscard]] auto entries() noexcept -> pointer {
            return Base::m_ptr->entries;
        }

        [[nodiscard]] auto entries() const noexcept -> const_pointer {
            return Base::m_ptr->entries;
        }
};

}  // namespace vmm::kvm::detail
