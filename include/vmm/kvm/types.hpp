/*
 * types.hpp - KVM types
 */

#pragma once

#include <memory>
#include <linux/kvm.h>

/**
 * Size of reserved range for KVM-specific MSRs (0x4b564d00 to 0x4b564dff)
 */
#define MAX_IO_MSRS 256

/**
 * Size of `msr_based_features_all` array in linux/arch/x86/kvm/x86.c
 */
#define MAX_IO_MSRS_FEATURES 22

template<class struct_t, class buffer_t>
class FamStruct {
    protected:
        std::unique_ptr<struct_t, void(*)(struct_t*)> s_;
    public:
        FamStruct(const size_t n) : s_{reinterpret_cast<struct_t*>(new buffer_t[n]()),
                                       [](struct_t *p){ delete[] reinterpret_cast<buffer_t*>(p); }} {}
        struct_t* get() { return s_.get(); }
};

namespace vmm::kvm {
    class MsrIndexList : public FamStruct<kvm_msr_list, uint32_t> {
        protected:
            /**
             * Relevant struct:
             *
             *     struct kvm_msr_list {
             *         __u32 nmsrs;
             *         __u32 indices[0];
             *     };
             */
            MsrIndexList(const size_t n) : FamStruct(n + 1) { s_->nmsrs = n; }
        public:
            MsrIndexList() : MsrIndexList(MAX_IO_MSRS) {}

            uint32_t nmsrs() {return s_->nmsrs;}

            uint32_t* begin()              { return s_->indices; }
            uint32_t* end()                { return s_->indices + s_->nmsrs; }
            uint32_t const* begin()  const { return s_->indices; }
            uint32_t const* end()    const { return s_->indices + s_->nmsrs; }
            uint32_t const* cbegin() const { return begin(); }
            uint32_t const* cend()   const { return end(); }
    };

    class MsrFeatureList : public MsrIndexList {
        public:
            MsrFeatureList() : MsrIndexList(MAX_IO_MSRS_FEATURES) {}
    };

    class Msrs : public FamStruct<kvm_msrs, uint64_t> {
        public:
            /**
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
             */
            Msrs(const size_t n) : FamStruct(n * 2 + 1) { s_->nmsrs = n; }

            uint32_t nmsrs() { return s_->nmsrs; }

            kvm_msr_entry* begin()              { return s_->entries; }
            kvm_msr_entry* end()                { return s_->entries + s_->nmsrs; }
            kvm_msr_entry const* begin()  const { return s_->entries; }
            kvm_msr_entry const* end()    const { return s_->entries + s_->nmsrs; }
            kvm_msr_entry const* cbegin() const { return begin(); }
            kvm_msr_entry const* cend()   const { return end(); }
    };
};
