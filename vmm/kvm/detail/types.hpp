/*
 * types.hpp - KVM types
 */

#pragma once

#include <algorithm> // copy_if, swap
#include <iterator> // distance
#include <linux/kvm.h> // kvm_*

#include "vmm/kvm/detail/macros.hpp"
#include "vmm/types/detail/fam_struct.hpp"

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

class MsrIndexList : public FamStruct<kvm_msr_list, uint32_t, uint32_t> {
    public:
        // Constructors
        MsrIndexList(std::size_t n);

        // Element access
        [[nodiscard]] auto operator[](std::size_t pos) noexcept -> reference;
        [[nodiscard]] auto operator[](std::size_t pos) const noexcept -> const_reference;
        [[nodiscard]] auto data() noexcept -> pointer;
        [[nodiscard]] auto data() const noexcept -> const_pointer;

        // Capacity
        [[nodiscard]] auto empty() const noexcept -> bool;
        [[nodiscard]] auto size() const noexcept -> std::size_t;

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
        MsrFeatureList(std::size_t n);
};

class MsrList : public FamStruct<kvm_msrs, uint64_t, kvm_msr_entry> {
    private:
        MsrList(std::size_t n);
    public:
        // Constructors
        MsrList(value_type entry);

        template <typename Iterator>
        MsrList(Iterator first, Iterator last) : MsrList(std::distance(first, last)) {
            std::copy_if(first, last, data(), [](value_type) { return true; });
        }

        template <typename Container>
        MsrList(Container& c) : MsrList(c.begin(), c.end()) { }

        MsrList(const MsrList& other);
        MsrList(MsrList&& other) = default;
        auto operator=(MsrList other) -> MsrList&;

        // Element access
        [[nodiscard]] auto operator[](std::size_t pos) noexcept -> reference;
        [[nodiscard]] auto operator[](std::size_t pos) const noexcept -> const_reference;
        [[nodiscard]] auto data() noexcept -> pointer;
        [[nodiscard]] auto data() const noexcept -> const_pointer;

        // Capacity
        [[nodiscard]] auto empty() const noexcept -> bool;
        [[nodiscard]] auto size() const noexcept -> std::size_t;

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
        CpuidList(uint32_t n);
        CpuidList(value_type entry);

        template <typename Iterator>
        CpuidList(Iterator first, Iterator last) : CpuidList(std::distance(first, last)) {
            std::copy_if(first, last, data(), [](value_type) { return true; });
        }

        template <typename Container>
        CpuidList(Container& c) : CpuidList(c.begin(), c.end()) { }

        CpuidList(const CpuidList& other);
        CpuidList(CpuidList&& other) = default;
        auto operator=(CpuidList other) -> CpuidList&;

        // Element access
        [[nodiscard]] auto operator[](std::size_t pos) noexcept -> reference;
        [[nodiscard]] auto operator[](std::size_t pos) const noexcept -> const_reference;
        [[nodiscard]] auto data() noexcept -> pointer;
        [[nodiscard]] auto data() const noexcept -> const_pointer;

        // Capacity
        [[nodiscard]] auto empty() const noexcept -> bool;
        [[nodiscard]] auto size() const noexcept -> std::size_t;

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
        IrqRoutingList(uint32_t n);
    public:
        // Constructors
        IrqRoutingList(value_type entry);

        template <typename Iterator>
        IrqRoutingList(Iterator first, Iterator last) : IrqRoutingList(std::distance(first, last)) {
            std::copy_if(first, last, ptr_->entries, [](value_type) { return true; });
        }

        template <typename Container>
        IrqRoutingList(Container& c) : IrqRoutingList(c.begin(), c.end()) { }

        IrqRoutingList(const IrqRoutingList& other);
        IrqRoutingList(IrqRoutingList&& other) = default;
        auto operator=(IrqRoutingList other) -> IrqRoutingList&;

        // Element access
        [[nodiscard]] auto operator[](std::size_t pos) noexcept -> reference;
        [[nodiscard]] auto operator[](std::size_t pos) const noexcept -> const_reference;
        [[nodiscard]] auto data() noexcept -> pointer;
        [[nodiscard]] auto data() const noexcept -> const_pointer;

        // Capacity
        [[nodiscard]] auto empty() const noexcept -> bool;
        [[nodiscard]] auto size() const noexcept -> std::size_t;

        // Iterators
        auto begin() noexcept -> iterator;
        auto end() noexcept -> iterator;
        auto begin() const noexcept -> const_iterator;
        auto end() const noexcept -> const_iterator;
        auto cbegin() const noexcept -> const_iterator;
        auto cend() const noexcept -> const_iterator;
};

}  // namespace vmm::kvm::detail
