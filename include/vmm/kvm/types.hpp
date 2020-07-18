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

namespace vmm::kvm {
    class MsrList {
        protected:
            std::unique_ptr<kvm_msr_list, void(*)(kvm_msr_list*)> list_;

            /**
             * Constructs an MSR index list with @size possible entries.
             *
             * The relevant struct is as follows:
             *
             *     struct kvm_msr_list {
             *         __u32 nmsrs;
             *         __u32 indices[0];
             *     };
             */
            MsrList(const std::size_t size)
                : list_{reinterpret_cast<kvm_msr_list*>(new uint32_t[size + 1]),
                        [](kvm_msr_list *l){ delete[] reinterpret_cast<uint32_t*>(l); }}
            {
                list_->nmsrs = size;
            }
        public:
            MsrList() : MsrList(MAX_IO_MSRS) {}

            kvm_msr_list* data() { return list_.get(); }
            uint32_t nmsrs() { return list_->nmsrs; }
            uint32_t* indices() { return list_->indices; }
    };

    class MsrFeatureList : public MsrList {
        public:
            MsrFeatureList() : MsrList(MAX_IO_MSRS_FEATURES) {}
    };

    class Msrs {
        private:
            std::unique_ptr<kvm_msrs, void(*)(kvm_msrs*)> msrs_;
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
            Msrs(const std::size_t size)
                : msrs_{reinterpret_cast<kvm_msrs*>(new uint64_t[size * 2 + 1]),
                        [](kvm_msrs *m){ delete[] reinterpret_cast<uint64_t*>(m); }}
            {
                msrs_->nmsrs = size;
            }

            uint32_t nmsrs() { return msrs_->nmsrs; }
    };
};
