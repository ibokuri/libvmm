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

uint32_t MsrIndexList::nmsrs() const { return ptr_->nmsrs; }

uint32_t* MsrIndexList::begin() { return ptr_->indices; }
uint32_t* MsrIndexList::end() { return ptr_->indices + ptr_->nmsrs; }
uint32_t const* MsrIndexList::begin() const { return ptr_->indices; }
uint32_t const* MsrIndexList::end() const { return ptr_->indices + ptr_->nmsrs; }
uint32_t const* MsrIndexList::cbegin() const { return begin(); }
uint32_t const* MsrIndexList::cend() const { return end(); }


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
 * kvm::system kvm;
 * kvm_msr_entry entry{0x174};
 * kvm::MsrList msrs{entry};
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
MsrList& MsrList::operator=(MsrList other) {
    other.ptr_.swap(this->ptr_);
    return *this;
}

uint32_t MsrList::nmsrs() const { return ptr_->nmsrs; }

kvm_msr_entry* MsrList::begin()              { return ptr_->entries; }
kvm_msr_entry* MsrList::end()                { return ptr_->entries + ptr_->nmsrs; }
kvm_msr_entry const* MsrList::begin()  const { return ptr_->entries; }
kvm_msr_entry const* MsrList::end()    const { return ptr_->entries + ptr_->nmsrs; }
kvm_msr_entry const* MsrList::cbegin() const { return begin(); }
kvm_msr_entry const* MsrList::cend()   const { return end(); }


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
CpuidList& CpuidList::operator=(CpuidList other) {
    other.ptr_.swap(this->ptr_);
    return *this;
}

uint32_t CpuidList::nent() const { return ptr_->nent; }

kvm_cpuid_entry2* CpuidList::begin() { return ptr_->entries; }
kvm_cpuid_entry2* CpuidList::end() { return ptr_->entries + ptr_->nent; }
kvm_cpuid_entry2 const* CpuidList::begin() const { return ptr_->entries; }
kvm_cpuid_entry2 const* CpuidList::end() const { return ptr_->entries + ptr_->nent; }
kvm_cpuid_entry2 const* CpuidList::cbegin() const { return begin(); }
kvm_cpuid_entry2 const* CpuidList::cend() const { return end(); }

} // namespace vmm::kvm::detail
