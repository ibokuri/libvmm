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
 * Buffer size computations:
 *
 *     N + sizeof(__u32)
 */
MsrIndexList::MsrIndexList(const size_t n) : FamStruct(n + 1) {
    ptr_->nmsrs = n;
}

/**
 * Default MsrIndexList constructor.
 */
MsrIndexList::MsrIndexList() : MsrIndexList(MAX_IO_MSRS) {}

auto MsrIndexList::nmsrs() const -> unsigned int { return ptr_->nmsrs; }
auto MsrIndexList::begin() -> uint32_t* { return ptr_->indices; }
auto MsrIndexList::end() -> uint32_t* { return ptr_->indices + ptr_->nmsrs; }
auto MsrIndexList::begin() const -> uint32_t const* { return ptr_->indices; }
auto MsrIndexList::end() const -> uint32_t const* { return ptr_->indices + ptr_->nmsrs; }
auto MsrIndexList::cbegin() const -> uint32_t const* { return begin(); }
auto MsrIndexList::cend() const -> uint32_t const* { return end(); }



/**
 * Default MsrFeatureList constructor.
 */
MsrFeatureList::MsrFeatureList() : MsrIndexList(MAX_IO_MSRS_FEATURES) {}



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
MsrList::MsrList(const size_t n) : FamStruct(n * 2 + 1) {
    ptr_->nmsrs = n;
}

/**
 * Constructor for a single MSR entry.
 *
 * # Examples
 *
 * ```
 * #include <vmm/kvm.hpp>
 *
 * auto kvm = vmm::kvm::system{};
 * auto entry = kvm_msr_entry{0x174};
 * auto msrs = vmm::kvm::MsrList{entry};
 * ```
 */
MsrList::MsrList(kvm_msr_entry entry) : MsrList(1) {
    ptr_->entries[0] = entry;
}

/**
 * Copy constructor.
 */
MsrList::MsrList(const MsrList& other) : MsrList(other.begin(), other.end()) {}

/**
 * Copy/move assignment operator.
 *
 * By taking `other` by value, the caller decides whether a
 * copy/move is done.
 */
auto MsrList::operator=(MsrList other) -> MsrList& {
    other.ptr_.swap(this->ptr_);
    return *this;
}

auto MsrList::nmsrs() const -> uint32_t { return ptr_->nmsrs; }
auto MsrList::begin() -> kvm_msr_entry* { return ptr_->entries; }
auto MsrList::end() -> kvm_msr_entry* { return ptr_->entries + ptr_->nmsrs; }
auto MsrList::begin() const -> kvm_msr_entry const* { return ptr_->entries; }
auto MsrList::end() const -> kvm_msr_entry const* { return ptr_->entries + ptr_->nmsrs; }
auto MsrList::cbegin() const-> kvm_msr_entry const* { return begin(); }
auto MsrList::cend() const -> kvm_msr_entry const* { return end(); }



/*
 * Internal constructor.
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
CpuidList::CpuidList(const uint32_t n) : FamStruct(n * sizeof(kvm_cpuid_entry2) + 2) {
    ptr_->nent = n;
}

/**
 * Default CpuidList constructor.
 */
CpuidList::CpuidList() : CpuidList(MAX_CPUID_ENTRIES) {}

/**
 * Constructor for a single cpuid entry.
 */
CpuidList::CpuidList(kvm_cpuid_entry2 entry) : CpuidList(1) {
    ptr_->entries[0] = entry;
}

/*
 * Copy constructor.
 */
CpuidList::CpuidList(const CpuidList& other) : CpuidList(other.begin(), other.end()) {}

/**
 * Copy/move assignment operator.
 */
auto CpuidList::operator=(CpuidList other) -> CpuidList& {
    other.ptr_.swap(this->ptr_);
    return *this;
}

auto CpuidList::nent() const -> uint32_t { return ptr_->nent; }
auto CpuidList::begin() -> kvm_cpuid_entry2* { return ptr_->entries; }
auto CpuidList::end() -> kvm_cpuid_entry2* { return ptr_->entries + ptr_->nent; }
auto CpuidList::begin() const -> kvm_cpuid_entry2 const* { return ptr_->entries; }
auto CpuidList::end() const -> kvm_cpuid_entry2 const* { return ptr_->entries + ptr_->nent; }
auto CpuidList::cbegin() const-> kvm_cpuid_entry2 const* { return begin(); }
auto CpuidList::cend() const -> kvm_cpuid_entry2 const* { return end(); }

} // namespace vmm::kvm::detail
