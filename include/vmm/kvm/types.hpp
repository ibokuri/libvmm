#pragma once

#include <memory>
#include <linux/kvm.h>

/**
 * NOTE: Model-specific registers (MSRs)
 * =====================================
 * A MSR is any control register in the x86 instruction set used for debugging,
 * program execution tracing, computer performance monitoring, and toggling
 * certain CPU features.
 *
 * A control register is one that changes/controls the general behavior of a
 * CPU (e.g., interrupt control, switching addressing modes, paging control).
 * For example, with the 80386 processor, Intel introduced experimental
 * features that would not be present in future versions of the processor. The
 * first of these were two "test registers" that allowed the 80386 to speed up
 * virtual-to-physical address conversions.
 *
 * NOTE: Copy constructors for MsrList & MsrFeatureList?
 * =====================================================
 * No.
 *
 * Usage of MsrList and MsrFeatureList objects revolves solely around MSR
 * enumeration; that is, you use them to either query the system to learn which
 * MSRs/features are supported or to learn the indices of certain MSRs so that
 * you may specify them in a subsequent KVM_GET_MSRS or KVM_SET_MSRS ioctl
 * call.
 *
 * Consequently, copying these objects makes no sense, as a mutable copy
 * provides no benefit in regards to the objects' original purpose: to
 * represent all supported or feature-exposing MSRs.
 */

/**
 * Size of reserved range for KVM-specific MSRs (0x4b564d00 to 0x4b564dff)
 */
#define MAX_IO_MSRS 256

/**
 * Size of `msr_based_features_all` array in linux/arch/x86/kvm/x86.c
 */
#define MAX_IO_MSRS_FEATURES 22

template<typename struct_t, typename buffer_t>
class FamStruct {
    protected:
        std::unique_ptr<struct_t, void(*)(struct_t*)> s_;
    public:
        FamStruct(const size_t n) : s_{reinterpret_cast<struct_t*>(new buffer_t[n]()),
                                       [](struct_t *p){ delete[] reinterpret_cast<buffer_t*>(p); }} {}
        struct_t* get() { return s_.get(); }
};

namespace vmm::kvm {
    class MsrList : public FamStruct<kvm_msr_list, uint32_t> {
        protected:
            /**
             * Relevant struct:
             *
             *     struct kvm_msr_list {
             *         __u32 nmsrs;
             *         __u32 indices[0];
             *     };
             */
            MsrList(const size_t n) : FamStruct(n + 1)
            {
                s_->nmsrs = n;
            }
        public:
            MsrList() : MsrList(MAX_IO_MSRS) {}

            uint32_t nmsrs() {return s_->nmsrs;}

            uint32_t* begin()              { return s_->indices; }
            uint32_t* end()                { return s_->indices + s_->nmsrs; }
            uint32_t const* begin()  const { return s_->indices; }
            uint32_t const* end()    const { return s_->indices + s_->nmsrs; }
            uint32_t const* cbegin() const { return begin(); }
            uint32_t const* cend()   const { return end(); }
    };

    class MsrFeatureList : public MsrList {
        public:
            MsrFeatureList() : MsrList(MAX_IO_MSRS_FEATURES) {}
    };

    class Msrs : public FamStruct<kvm_msrs, uint64_t> {
        public:
            /**
             * Constructs an Msrs with @size possible entries.
             *
             * The relevant structs are as follows:
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
             */
            Msrs(const size_t n) : FamStruct(n * 2 + 1)
            {
                s_->nmsrs = n;
            }

            uint32_t nmsrs() { return s_->nmsrs; }

            kvm_msr_entry* begin()              { return s_->entries; }
            kvm_msr_entry* end()                { return s_->entries + s_->nmsrs; }
            kvm_msr_entry const* begin()  const { return s_->entries; }
            kvm_msr_entry const* end()    const { return s_->entries + s_->nmsrs; }
            kvm_msr_entry const* cbegin() const { return begin(); }
            kvm_msr_entry const* cend()   const { return end(); }
    };
};
