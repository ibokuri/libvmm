/*
 * types.hpp - KVM types
 */

#pragma once

#include <algorithm> // copy
#include <cstddef> // byte, size_t
#include <cmath> // abs
#include <cstring> // memcpy, memset
#include <iterator> // distance
#include <initializer_list> // initializer_list
#include <limits> // numeric_limits
#include <linux/kvm.h> // kvm_*
#include <memory_resource> // polymorphic_allocator
#include <stdexcept> // overflow_error
#include <iostream>

#include "vmm/kvm/detail/macros.hpp"
#include "vmm/types/file_descriptor.hpp"

namespace vmm::kvm::detail {

class system;

class KvmFd : public vmm::types::FileDescriptor {
    public:
        explicit KvmFd(int fd) noexcept
            : vmm::types::FileDescriptor(fd) {}

        KvmFd(const KvmFd& other) = delete;
        KvmFd(KvmFd&& other) = default;
        auto operator=(const KvmFd& other) -> KvmFd& = delete;
        auto operator=(KvmFd&& other) -> KvmFd& = default;
};

template <class Struct, typename T>
T DataMemberPtrType(T Struct::*v);

template<typename Struct,
         typename Entry,
         auto SizeMember,
         auto EntriesMember,
         std::size_t N>
class FamStruct {
    public:
        using value_type = Entry;
        using size_type = decltype(DataMemberPtrType(SizeMember));
        using allocator_type = std::pmr::polymorphic_allocator<std::byte>;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;
        using iterator = pointer;
        using const_iterator = const_pointer;

        static_assert(N <= std::numeric_limits<size_type>::max());

        static const auto alignment = alignof(Struct);
        static const auto storage_size = sizeof(Struct) + N * sizeof(Entry);

        explicit FamStruct(const allocator_type& alloc={})
                : m_alloc{alloc}, m_ptr{allocate_fam(alloc)} {
            std::memset(m_ptr, 0, storage_size);
            m_ptr->*SizeMember = N;
        }

        // TODO: SFINAE
        template <typename InputIt>
        FamStruct(InputIt first, InputIt last, const allocator_type& alloc)
                : FamStruct(alloc) {
            if (auto n = std::distance(first, last); n != 0) {
                auto abs = std::abs(n);
                if (abs > std::numeric_limits<size_type>::max() ||
                        static_cast<size_type>(abs) > N)
                    VMM_THROW(std::overflow_error("Range too large"));

                m_ptr->*SizeMember = static_cast<size_type>(abs);
                std::copy(first, last, begin());
            }
        }

        template <typename InputIt>
        FamStruct(InputIt first, InputIt last)
                : FamStruct(first, last, std::pmr::new_delete_resource()) {}

        explicit FamStruct(std::initializer_list<value_type> ilist)
                : FamStruct(ilist.begin(), ilist.end()) {}

        FamStruct(std::initializer_list<value_type> ilist,
                  const allocator_type& alloc)
                : FamStruct(ilist.begin(), ilist.end(), alloc) {}

        auto operator=(std::initializer_list<value_type> ilist) -> FamStruct& {
            if (auto n = ilist.size(); n != 0) {
                if (n > std::numeric_limits<size_type>::max() || n > N)
                    VMM_THROW(std::overflow_error("Range too large"));

                m_ptr->*SizeMember = n;
                std::memset(m_ptr->*EntriesMember, 0, N * sizeof(Entry));
                std::copy(ilist.begin(), ilist.end(), begin());
            }

            return *this;
        }

        /**
         * Copy assignment operator
         */
        auto operator=(const FamStruct& other) -> FamStruct& {
            if (this != &other)
                std::memcpy(m_ptr, other.m_ptr, storage_size);

            return *this;
        }

        /**
         * Move assignment operator
         */
        auto operator=(FamStruct&& other) -> FamStruct& {
            if (m_alloc == other.m_alloc)
                std::swap(m_ptr, other.m_ptr);
            else
                operator=(other);

            return *this;
        }

        /**
         * Copy constructor
         */
        FamStruct(const FamStruct& other, const allocator_type& alloc={})
                : FamStruct(alloc) {
            operator=(other);
        }

        /**
         * Move constructors
         */
        FamStruct(FamStruct&& other) : m_alloc{other.get_allocator()} {
            operator=(std::move(other));
        }

        FamStruct(FamStruct&& other, const allocator_type& alloc)
                : FamStruct(alloc) {
            operator=(std::move(other));
        }

        /**
         * Destructor
         */
        ~FamStruct() {
            deallocate_fam();
        }

        [[nodiscard]] allocator_type get_allocator() const noexcept {
            return m_alloc;
        }

        /* Element access */
        [[nodiscard]] auto operator[](std::size_t pos) noexcept -> reference {
            static_assert(N > 0);
            return (m_ptr->*EntriesMember)[pos];
        }

        [[nodiscard]] auto operator[](std::size_t pos) const noexcept -> const_reference {
            static_assert(N > 0);
            return (m_ptr->*EntriesMember)[pos];
        }

        [[nodiscard]] auto front() noexcept -> reference {
            return *begin();
        }

        [[nodiscard]] auto front() const noexcept -> const_reference {
            return *begin();
        }

        [[nodiscard]] auto back() noexcept -> reference {
            return *end();
        }

        [[nodiscard]] auto back() const noexcept -> const_reference {
            return *end();
        }

        [[nodiscard]] constexpr auto data() noexcept -> Struct* {
            return m_ptr;
        }

        [[nodiscard]] constexpr auto data() const noexcept -> const Struct* {
            return m_ptr;
        }

        /* Iterators */
        auto begin() noexcept -> iterator {
            return m_ptr->*EntriesMember;
        }

        auto begin() const noexcept -> const_iterator {
			return m_ptr->*EntriesMember;
        }

        auto end() noexcept -> iterator {
			return begin() + size();
        }

        auto end() const noexcept -> const_iterator {
			return begin() + size();
        }

        auto cbegin() const noexcept -> const_iterator {
			return begin();
        }

        auto cend() const noexcept -> const_iterator {
			return end();
        }

        /* Capacity */
        [[nodiscard]] constexpr auto size() const noexcept -> size_type {
            return m_ptr->*SizeMember;
        }

        [[nodiscard]] constexpr auto empty() const noexcept -> bool {
            return size() == 0;
        }

        [[nodiscard]] constexpr auto capacity() const noexcept -> size_type {
            return N;
        }
    private:
        allocator_type m_alloc;
        Struct *m_ptr = nullptr;

        auto allocate_fam(const allocator_type& alloc) -> Struct* {
            return static_cast<Struct*>(alloc.resource()->allocate(storage_size, alignment));
        }

        auto deallocate_fam() -> void {
            m_alloc.resource()->deallocate(m_ptr, storage_size, alignment);
            m_ptr = nullptr;
        }
};

/**
 * NOTE: Because MsrList's value_type is an integer, it is very easy to confuse
 *       the size_type initializer_list and constructor. That is, one may think
 *       that `auto msr_list = MsrList{10}` constructs a FAM struct with enough
 *       space for 10 entries, but really it constructs a FAM struct with an
 *       initializer list, resulting in a FAM struct with only 1 entry with a
 *       value of 10. To use the size_type constructor, use `MsrList(10)`.
 */
template<std::size_t N>
class MsrList : public FamStruct<kvm_msr_list,
                                 uint32_t,
                                 &kvm_msr_list::nmsrs,
                                 &kvm_msr_list::indices,
                                 N> {
    using Base = FamStruct<kvm_msr_list,
                           uint32_t,
                           &kvm_msr_list::nmsrs,
                           &kvm_msr_list::indices,
                           N>;
    using Base::Base;
    using allocator_type = typename Base::allocator_type;

    private:
        friend system;

        MsrList() : Base::FamStruct() {}
        explicit MsrList(const allocator_type& alloc)
                : Base::FamStruct(alloc) {}
};

template<std::size_t N>
class Msrs : public FamStruct<kvm_msrs,
                              kvm_msr_entry,
                              &kvm_msrs::nmsrs,
                              &kvm_msrs::entries,
                              N> {
    using Base = FamStruct<kvm_msrs,
                           kvm_msr_entry,
                           &kvm_msrs::nmsrs,
                           &kvm_msrs::entries,
                           N>;
    using Base::Base;
};

template<std::size_t N>
class Cpuids : public FamStruct<kvm_cpuid2,
                                kvm_cpuid_entry2,
                                &kvm_cpuid2::nent,
                                &kvm_cpuid2::entries,
                                N> {
    using Base = FamStruct<kvm_cpuid2,
                           kvm_cpuid_entry2,
                           &kvm_cpuid2::nent,
                           &kvm_cpuid2::entries,
                           N>;
    using Base::Base;
};

template<std::size_t N>
class IrqRouting : public FamStruct<kvm_irq_routing,
                                    kvm_irq_routing_entry,
                                    &kvm_irq_routing::nr,
                                    &kvm_irq_routing::entries,
                                    N> {
    using Base = FamStruct<kvm_irq_routing,
                           kvm_irq_routing_entry,
                           &kvm_irq_routing::nr,
                           &kvm_irq_routing::entries,
                           N>;
    using Base::Base;
};

}  // namespace vmm::kvm::detail
