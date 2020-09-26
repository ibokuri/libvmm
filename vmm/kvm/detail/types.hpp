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
        static const auto size = sizeof(Struct) + N * sizeof(Entry);

        // Allocator constructor
        explicit FamStruct(const allocator_type& alloc)
                : m_alloc{alloc}, m_ptr{static_cast<Struct*>(m_alloc.resource()->allocate(size, alignment))} {
            try {
                m_alloc.construct(m_ptr);
            } catch(...) {
                m_alloc.deallocate(reinterpret_cast<std::byte*>(m_ptr), size);
                throw;
            }

            std::memset(m_ptr, 0, size);
        }

        // Default constructor
        explicit FamStruct() : FamStruct(std::pmr::new_delete_resource()) {}

        // Destructor
        ~FamStruct() {
            m_alloc.destroy(m_ptr);
            m_alloc.deallocate(reinterpret_cast<std::byte*>(m_ptr), size);
        }

        // Copy constructor and assignment operator
        FamStruct(const FamStruct& other);
        FamStruct(const FamStruct& other, const allocator_type& alloc);

        // Move constructor and assignment operator
        FamStruct(FamStruct&& other) noexcept;
        FamStruct(FamStruct&& other, const allocator_type& alloc) noexcept;

        // Range constructor
        template<typename InputIt>
        FamStruct(InputIt first, InputIt last, const allocator_type& alloc={});

        // Initializer list constructor and assignment operator
        FamStruct(std::initializer_list<Entry> init, const allocator_type& alloc={})
              : FamStruct(init.begin(), init.end(), alloc) {}

        FamStruct& operator=(std::initializer_list<Entry> init);

        // Assignment operators (NOTE: deleted as they break the invariant that a FamStruct's size never changes)
        void operator=(FamStruct&&) = delete;
        void operator=(const FamStruct&) = delete;

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

    private:
        MsrList() noexcept {
            Base::m_ptr->nmsrs = N;
        };

        friend system;
    public:
        using value_type = typename Base::value_type;
        using size_type = typename Base::size_type;
        using reference = typename Base::reference;
        using const_reference = typename Base::const_reference;
        using iterator = typename Base::iterator;
        using const_iterator = typename Base::const_iterator;

        // Capacity
        [[nodiscard]] auto size() const noexcept -> size_type {
			return Base::m_ptr->nmsrs;
		}

        [[nodiscard]] auto empty() const noexcept -> bool {
			return size() == 0;
		}

        // Element access
        [[nodiscard]] auto operator[](std::size_t pos) noexcept -> reference {
			return Base::m_ptr->indices[pos];
		}

        [[nodiscard]] auto operator[](std::size_t pos) const noexcept -> const_reference {
			return Base::m_ptr->indices[pos];
		}

        // Iterators
        auto begin() noexcept -> iterator {
			return Base::m_ptr->indices;
		}

        auto end() noexcept -> iterator {
			return Base::m_ptr->indices + Base::m_ptr->nmsrs;
		}

        auto begin() const noexcept -> const_iterator {
			return Base::m_ptr->indices;
		}

        auto end() const noexcept -> const_iterator {
			return Base::m_ptr->indices + Base::m_ptr->nmsrs;
		}

        auto cbegin() const noexcept -> const_iterator {
			return begin();
		}

        auto cend() const noexcept -> const_iterator {
			return end();
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
        using value_type = typename Base::value_type;
        using size_type = typename Base::size_type;
        using reference = typename Base::reference;
        using const_reference = typename Base::const_reference;
        using iterator = typename Base::iterator;
        using const_iterator = typename Base::const_iterator;

        Msrs() noexcept {
            Base::m_ptr->nmsrs = N;
        };

        explicit Msrs(value_type entry) noexcept {
            static_assert(N == 1);

            std::memcpy(Base::m_ptr->entries, &entry, sizeof(value_type));
            Base::m_ptr->nmsrs = N;
        }

        template <typename Iterator>
        explicit Msrs(Iterator first, Iterator last) {
            std::copy_if(first, last, begin(), [](value_type) { return true; });
            Base::m_ptr->nmsrs = N;
        }

        template <typename Container>
        explicit Msrs(Container& c) : Msrs(c.begin(), c.end()) {}

        // Capacity
        [[nodiscard]] auto size() const noexcept -> size_type {
            return Base::m_ptr->nmsrs;
        }

        [[nodiscard]] auto empty() const noexcept -> bool {
            return size() == 0;
        }

        // Element access
        [[nodiscard]] auto operator[](std::size_t pos) noexcept -> reference {
            return Base::m_ptr->entries[pos];
        }

        [[nodiscard]] auto operator[](std::size_t pos) const noexcept -> const_reference {
            return Base::m_ptr->entries[pos];
        }

        // Iterators
        auto begin() noexcept -> iterator {
            return Base::m_ptr->entries;
        }

        auto end() noexcept -> iterator {
            return Base::m_ptr->entries + Base::m_ptr->nmsrs;
        }

        auto begin() const noexcept -> const_iterator {
            return Base::m_ptr->entries;
        }

        auto end() const noexcept -> const_iterator {
            return Base::m_ptr->entries + Base::m_ptr->nmsrs;
        }

        auto cbegin() const noexcept -> const_iterator {
            return begin();
        }

        auto cend() const noexcept -> const_iterator {
            return end();
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
        using value_type = typename Base::value_type;
        using size_type = typename Base::size_type;
        using reference = typename Base::reference;
        using const_reference = typename Base::const_reference;
        using iterator = typename Base::iterator;
        using const_iterator = typename Base::const_iterator;

        Cpuids() noexcept {
            Base::m_ptr->nent = N;
        }

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

        // Capacity
        [[nodiscard]] auto size() const noexcept -> size_type {
			return Base::m_ptr->nent;
		}

        [[nodiscard]] auto empty() const noexcept -> bool {
			return size() == 0;
		}

        // Element access
        [[nodiscard]] auto operator[](std::size_t pos) noexcept -> reference {
			return Base::m_ptr->entries[pos];
		}

        [[nodiscard]] auto operator[](std::size_t pos) const noexcept -> const_reference {
			return Base::m_ptr->entries[pos];
		}

        // Iterators
        auto begin() noexcept -> iterator {
			return Base::m_ptr->entries;
		}

        auto end() noexcept -> iterator {
			return Base::m_ptr->entries + Base::m_ptr->nent;
		}

        auto begin() const noexcept -> const_iterator {
			return Base::m_ptr->entries;
		}

        auto end() const noexcept -> const_iterator {
			return Base::m_ptr->entries + Base::m_ptr->nent;
		}

        auto cbegin() const noexcept -> const_iterator {
			return begin();
		}

        auto cend() const noexcept -> const_iterator {
			return end();
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
        using value_type = typename Base::value_type;
        using size_type = typename Base::size_type;
        using reference = typename Base::reference;
        using const_reference = typename Base::const_reference;
        using iterator = typename Base::iterator;
        using const_iterator = typename Base::const_iterator;

        IrqRouting() noexcept {
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

        // Capacity
        [[nodiscard]] auto size() const noexcept -> size_type {
			return Base::m_ptr->nr;
		}

        [[nodiscard]] auto empty() const noexcept -> bool {
			return size() == 0;
		}

        // Iterators
        auto begin() noexcept -> iterator {
			return Base::m_ptr->entries;
		}

        auto end() noexcept -> iterator {
			return Base::m_ptr->entries + Base::m_ptr->nr;
		}

        auto begin() const noexcept -> const_iterator {
			return Base::m_ptr->entries;
		}

        auto end() const noexcept -> const_iterator {
			return Base::m_ptr->entries + Base::m_ptr->nr;
		}

        auto cbegin() const noexcept -> const_iterator {
			return begin();
		}

        auto cend() const noexcept -> const_iterator {
			return end();
		}
};

}  // namespace vmm::kvm::detail
