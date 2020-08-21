/*
 * types.hpp - KVM types
 */

#pragma once

#include "vmm/utility/utility.hpp"

#include <algorithm>
#include <memory>
#include <linux/kvm.h>

namespace vmm::kvm::detail {

/**
 * Size of reserved range for KVM-specific MSRs (0x4b564d00 to 0x4b564dff)
 */
#define MAX_IO_MSRS 256

/**
 * Size of `msr_based_features_all` array in linux/arch/x86/kvm/x86.c
 */
#define MAX_IO_MSRS_FEATURES 22

/**
 * Defined in linux/arch/x86/include/asm/kvm_host.h
 */
#define MAX_CPUID_ENTRIES 80

/**
 * Basic wrapper around C FAM structs.
 */
template<typename Struct, typename Buffer, typename Entry>
class FamStruct {
    protected:
        std::unique_ptr<Struct, void(*)(Struct*)> ptr_;

        FamStruct(const size_t n)
            : ptr_{reinterpret_cast<Struct*>(new Buffer[n]()),
                   [](Struct *p){ delete[] reinterpret_cast<Buffer*>(p); }} {}

        using value_type = Entry;
        using pointer = value_type*;
        using const_pointer = const pointer;
        using reference = value_type&;
        using const_reference = const value_type&;
        using iterator = pointer;
        using const_iterator = const_pointer;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    public:
        auto get() -> Struct* { return ptr_.get(); }
};

class MsrIndexList : public FamStruct<kvm_msr_list, uint32_t, uint32_t> {
    public:
        // Constructors
        MsrIndexList(const size_t n);

        // Element access
        auto operator[](size_t pos) -> reference;
        auto operator[](size_t pos) const -> const_reference;
        auto data() noexcept -> pointer;
        auto data() const noexcept -> const_pointer;

        // Capacity
        auto empty() const noexcept -> bool;
        auto size() const noexcept -> uint32_t;

        // Iterators
        auto begin() noexcept -> iterator;
        auto end() noexcept -> iterator;
        auto begin() const noexcept -> const_iterator;
        auto end() const noexcept -> const_iterator;
        auto cbegin() const noexcept -> const_iterator;
        auto cend() const noexcept -> const_iterator;
};

class MsrFeatureList : public MsrIndexList {
    public:
        MsrFeatureList(const size_t n);
};

class MsrList : public FamStruct<kvm_msrs, uint64_t, kvm_msr_entry> {
    private:
        MsrList(const size_t n);
    public:
        // Constructors
        MsrList(value_type entry);

        template <typename Iterator>
        MsrList(Iterator first, Iterator last) : MsrList(std::distance(first, last)) {
            std::copy_if(first, last, data(), [](value_type) { return true; });
        }

        template <typename Container>
        MsrList(Container& c) : MsrList(c.begin(), c.end()) { }

        // Copy/move assignment
        MsrList(const MsrList& other);
        MsrList(MsrList&& other) = default;
        auto operator=(MsrList other) -> MsrList&;

        // Element access
        auto operator[](size_t pos) -> reference;
        auto operator[](size_t pos) const -> const_reference;
        auto data() noexcept -> pointer;
        auto data() const noexcept -> const_pointer;

        // Capacity
        auto empty() const noexcept -> bool;
        auto size() const noexcept -> uint32_t;

        // Iterators
        auto begin() noexcept -> iterator;
        auto end() noexcept -> iterator;
        auto begin() const noexcept -> const_iterator;
        auto end() const noexcept -> const_iterator;
        auto cbegin() const noexcept -> const_iterator;
        auto cend() const noexcept -> const_iterator;
};

class CpuidList : public FamStruct<kvm_cpuid2, uint32_t, kvm_cpuid_entry2> {
    public:
        // Constructors
        CpuidList(const uint32_t n);
        CpuidList(value_type entry);

        template <typename Iterator>
        CpuidList(Iterator first, Iterator last) : CpuidList(std::distance(first, last)) {
            std::copy_if(first, last, data(), [](value_type) { return true; });
        }

        template <typename Container>
        CpuidList(Container& c) : CpuidList(c.begin(), c.end()) { }

        // Copy/move assignment
        CpuidList(const CpuidList& other);
        CpuidList(CpuidList&& other) = default;
        auto operator=(CpuidList other) -> CpuidList&;

        // Element access
        auto operator[](size_t pos) -> reference;
        auto operator[](size_t pos) const -> const_reference;
        auto data() noexcept -> pointer;
        auto data() const noexcept -> const_pointer;

        // Capacity
        auto empty() const noexcept -> bool;
        auto size() const noexcept -> uint32_t;

        // Iterators
        auto begin() noexcept -> iterator;
        auto end() noexcept -> iterator;
        auto begin() const noexcept -> const_iterator;
        auto end() const noexcept -> const_iterator;
        auto cbegin() const noexcept -> const_iterator;
        auto cend() const noexcept -> const_iterator;
};

class IrqRoutingList : public FamStruct<kvm_irq_routing, uint64_t, kvm_irq_routing_entry> {
    private:
        IrqRoutingList(const uint32_t n);
    public:
        // Constructors
        IrqRoutingList(value_type entry);

        template <typename Iterator>
        IrqRoutingList(Iterator first, Iterator last) : IrqRoutingList(std::distance(first, last)) {
            std::copy_if(first, last, ptr_->entries, [](value_type) { return true; });
        }

        template <typename Container>
        IrqRoutingList(Container& c) : IrqRoutingList(c.begin(), c.end()) { }

        // Copy/move assignment
        IrqRoutingList(const IrqRoutingList& other);
        IrqRoutingList(IrqRoutingList&& other) = default;
        auto operator=(IrqRoutingList other) -> IrqRoutingList&;

        // Element access
        auto operator[](size_t pos) -> reference;
        auto operator[](size_t pos) const -> const_reference;
        auto data() noexcept -> pointer;
        auto data() const noexcept -> const_pointer;

        // Capacity
        auto empty() const noexcept -> bool;
        auto size() const noexcept -> uint32_t;

        // Iterators
        auto begin() noexcept -> iterator;
        auto end() noexcept -> iterator;
        auto begin() const noexcept -> const_iterator;
        auto end() const noexcept -> const_iterator;
        auto cbegin() const noexcept -> const_iterator;
        auto cend() const noexcept -> const_iterator;
};

}  // namespace vmm::kvm::detail
