/*
 * types.cpp - KVM types
 */

#include "vmm/kvm/detail/types.hpp"

namespace vmm::kvm::detail {

/**
 * Internal MsrIndexList/MsrFeatureList constructor.
 *
 * Relevant struct:
 *
 *     struct kvm_msr_list {
 *         __u32 nmsrs;
 *         __u32 indices[0];
 *     };
 *
 * Buffer size computation:
 *
 *     N + sizeof(__u32)
 */
MsrIndexList::MsrIndexList(const size_t n) : FamStruct(n + 1) { ptr_->nmsrs = n; }
MsrIndexList::MsrIndexList() : MsrIndexList(MAX_IO_MSRS) {}

// Capacity
auto MsrIndexList::size() const noexcept -> unsigned int { return ptr_->nmsrs; }
auto MsrIndexList::max_size() const noexcept -> unsigned int { return MAX_IO_MSRS; }

// Iterators
auto MsrIndexList::begin() noexcept -> uint32_t* { return ptr_->indices; }
auto MsrIndexList::end() noexcept -> uint32_t* { return ptr_->indices + ptr_->nmsrs; }
auto MsrIndexList::begin() const noexcept -> uint32_t const* { return ptr_->indices; }
auto MsrIndexList::end() const noexcept -> uint32_t const* { return ptr_->indices + ptr_->nmsrs; }
auto MsrIndexList::cbegin() const noexcept -> uint32_t const* { return begin(); }
auto MsrIndexList::cend() const noexcept -> uint32_t const* { return end(); }


/**
 * Internal MsrList constructor.
 *
 * Relevant structs:
 *
 *     struct kvm_msrs {
 *         __u32 nmsrs;
 *         __u32 pad;
 *         struct kvm_msr_entry entries[0];
 *     };
 *
 *     struct kvm_msr_entry {
 *         __u32 index;
 *         __u32 reserved;
 *         __u64 data;
 *     };
 *
 * Buffer size computation:
 *
 *     N * (sizeof(kvm_msr_entry) / sizeof(__u64)) + sizeof(__u64)
 */
MsrList::MsrList(const size_t n) : FamStruct(n * 2 + 1) { ptr_->nmsrs = n; }
MsrFeatureList::MsrFeatureList() : MsrIndexList(MAX_IO_MSRS_FEATURES) {}
MsrList::MsrList(kvm_msr_entry entry) : MsrList(1) { ptr_->entries[0] = entry; }
MsrList::MsrList(const MsrList& other) : MsrList(other.begin(), other.end()) {}

auto MsrList::operator=(MsrList other) -> MsrList& {
    other.ptr_.swap(this->ptr_);
    return *this;
}

auto MsrList::size() const noexcept -> uint32_t { return ptr_->nmsrs; }
auto MsrList::begin() noexcept -> kvm_msr_entry* { return ptr_->entries; }
auto MsrList::end() noexcept -> kvm_msr_entry* { return ptr_->entries + ptr_->nmsrs; }
auto MsrList::begin() const noexcept -> kvm_msr_entry const* { return ptr_->entries; }
auto MsrList::end() const noexcept -> kvm_msr_entry const* { return ptr_->entries + ptr_->nmsrs; }
auto MsrList::cbegin() const noexcept -> kvm_msr_entry const* { return begin(); }
auto MsrList::cend() const noexcept -> kvm_msr_entry const* { return end(); }


/*
 * Internal CpuidList constructor.
 *
 * Relevant structs:
 *
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
 *
 * Buffer size computation:
 *
 *     N * sizeof(kvm_cpuid_entry2) + 2 * sizeof(__u32)
 */
CpuidList::CpuidList(const uint32_t n) : FamStruct(n * sizeof(kvm_cpuid_entry2) + 2) { ptr_->nent = n; }
CpuidList::CpuidList() : CpuidList(MAX_CPUID_ENTRIES) {}
CpuidList::CpuidList(kvm_cpuid_entry2 entry) : CpuidList(1) { ptr_->entries[0] = entry; }
CpuidList::CpuidList(const CpuidList& other) : CpuidList(other.begin(), other.end()) {}

auto CpuidList::operator=(CpuidList other) -> CpuidList& {
    other.ptr_.swap(this->ptr_);
    return *this;
}

// Iterators
auto CpuidList::size() const noexcept -> uint32_t { return ptr_->nent; }
auto CpuidList::begin() noexcept -> kvm_cpuid_entry2* { return ptr_->entries; }
auto CpuidList::end() noexcept -> kvm_cpuid_entry2* { return ptr_->entries + ptr_->nent; }
auto CpuidList::begin() const noexcept -> kvm_cpuid_entry2 const* { return ptr_->entries; }
auto CpuidList::end() const noexcept -> kvm_cpuid_entry2 const* { return ptr_->entries + ptr_->nent; }
auto CpuidList::cbegin() const noexcept -> kvm_cpuid_entry2 const* { return begin(); }
auto CpuidList::cend() const noexcept -> kvm_cpuid_entry2 const* { return end(); }


/**
 * Internal IrqRoutingList constructor.
 *
 * Relevant structs:
 *
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
IrqRoutingList::IrqRoutingList(const uint32_t n)
    : FamStruct(n * (sizeof(kvm_irq_routing_entry) / 2) + 1) {
    ptr_->nr = n;
    ptr_->flags = 0;
}
IrqRoutingList::IrqRoutingList(kvm_irq_routing_entry entry) : IrqRoutingList(1) { ptr_->entries[0] = entry; }
IrqRoutingList::IrqRoutingList(const IrqRoutingList& other) : IrqRoutingList(other.begin(), other.end()) {}

auto IrqRoutingList::operator=(IrqRoutingList other) -> IrqRoutingList& {
    other.ptr_.swap(this->ptr_);
    return *this;
}

// Iterators
auto IrqRoutingList::size() const noexcept -> uint32_t { return ptr_->nr; }
auto IrqRoutingList::begin() noexcept -> kvm_irq_routing_entry* { return ptr_->entries; }
auto IrqRoutingList::end() noexcept -> kvm_irq_routing_entry* { return ptr_->entries + ptr_->nr; }
auto IrqRoutingList::begin() const noexcept -> kvm_irq_routing_entry const* { return ptr_->entries; }
auto IrqRoutingList::end() const noexcept -> kvm_irq_routing_entry const* { return ptr_->entries + ptr_->nr; }
auto IrqRoutingList::cbegin() const noexcept -> kvm_irq_routing_entry const* { return begin(); }
auto IrqRoutingList::cend() const noexcept -> kvm_irq_routing_entry const* { return end(); }

} // namespace vmm::kvm::detail
