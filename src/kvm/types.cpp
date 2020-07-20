/*
 * types.cpp - KVM types
 */

#include "../../include/vmm/kvm/types.hpp"

namespace vmm::kvm {
    /**
     * Internal Msrs constructor.
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
    Msrs::Msrs(const size_t n) : FamStruct(n * 2 + 1) {
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
     * kvm::Msrs msrs{entry};
     * ```
     */
    Msrs::Msrs(kvm_msr_entry entry) : Msrs(1) {
        ptr_->entries[0] = entry;
    }

    /**
     * Copy constructor.
     */
    Msrs::Msrs(const Msrs& other) : Msrs(other.begin(), other.end()) {}

    /**
     * Copy/move assignment operator.
     *
     * By taking `other` by value, the caller decides whether a
     * copy/move is done.
     */
    Msrs& Msrs::operator=(Msrs other) {
        other.ptr_.swap(this->ptr_);
        return *this;
    }

    uint32_t Msrs::nmsrs() const { return ptr_->nmsrs; }

    kvm_msr_entry* Msrs::begin()              { return ptr_->entries; }
    kvm_msr_entry* Msrs::end()                { return ptr_->entries + ptr_->nmsrs; }
    kvm_msr_entry const* Msrs::begin()  const { return ptr_->entries; }
    kvm_msr_entry const* Msrs::end()    const { return ptr_->entries + ptr_->nmsrs; }
    kvm_msr_entry const* Msrs::cbegin() const { return begin(); }
    kvm_msr_entry const* Msrs::cend()   const { return end(); }



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
     *     N + sizeof(__u32);
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
};
