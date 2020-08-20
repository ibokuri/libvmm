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
        using pointer = Entry*;
        using const_pointer = const Entry*;
        using reference = Entry&;
        using const_reference = const value_type&;
        using iterator = pointer;
        using const_iterator = const_pointer;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    public:
        auto get() -> Struct* { return ptr_.get(); }
};

class MsrIndexList : public FamStruct<kvm_msr_list, uint32_t, uint32_t> {
    protected:
        MsrIndexList(const size_t n);
    public:
        MsrIndexList();

        // Capacity
        auto size() const noexcept -> uint32_t;
        auto max_size() const noexcept-> uint32_t;

        // Iterators (TODO: rbegin, crbegin, rend, crend)
        auto begin() noexcept -> iterator;
        auto end() noexcept -> iterator;
        auto begin() const noexcept -> const_iterator;
        auto end() const noexcept -> const_iterator;
        auto cbegin() const noexcept -> const_iterator;
        auto cend() const noexcept -> const_iterator;
};

class MsrFeatureList : public MsrIndexList {
    public:
        MsrFeatureList();
};

class MsrList : public FamStruct<kvm_msrs, uint64_t, kvm_msr_entry> {
    private:
        MsrList(const size_t n);
    public:
        MsrList(value_type entry);

        /**
         * Range constructor.
         *
         * Examples
         * ========
         * ```
         * #include <vmm/kvm.hpp>
         *
         * auto kvm = vmm::kvm::system{};
         * auto msr_list = kvm.msr_feature_list();
         * auto entries = std::vector<kvm_msr_entry>{};
         *
         * for (auto msr : msr_list) {
         *     auto entry = kvm_msr_entry{msr};
         *     entries.push_back(entry);
         * }
         *
         * auto msrs = kvm::MsrList{entries.begin(), entries.end()};
         * ```
         */
        template <typename Iterator>
        MsrList(Iterator first, Iterator last) : MsrList(std::distance(first, last)) {
            std::copy_if(first, last, ptr_->entries, [](value_type) { return true; });
        }

        /**
         * Container constructor.
         *
         * Examples
         * ========
         * ```
         * #include <vmm/kvm.hpp>
         *
         * auto kvm = vmm::kvm::system{};
         * auto msr_list = kvm.msr_feature_list();
         * auto entries = std::vector<kvm_msr_entry>{};
         *
         * for (auto msr : msr_list) {
         *     auto entry = kvm_msr_entry{msr};
         *     entries.push_back(entry);
         * }
         *
         * auto msrs = kvm::MsrList{entries};
         * ```
         */
        template <typename Container>
        MsrList(Container& c) : MsrList(c.begin(), c.end()) { }

        MsrList(const MsrList& other);
        MsrList(MsrList&& other) = default;
        auto operator=(MsrList other) -> MsrList&;

        // Capacity
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
    private:
        CpuidList(const uint32_t n);
    public:
        CpuidList();
        CpuidList(value_type entry);

        template <typename Iterator>
        CpuidList(Iterator first, Iterator last) : CpuidList(std::distance(first, last)) {
            std::copy_if(first, last, ptr_->entries, [](value_type) { return true; });
        }

        template <typename Container>
        CpuidList(Container& c) : CpuidList(c.begin(), c.end()) { }

        CpuidList(const CpuidList& other);
        CpuidList(CpuidList&& other) = default;
        auto operator=(CpuidList other) -> CpuidList&;

        // Capacity
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

        // Capacity
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
