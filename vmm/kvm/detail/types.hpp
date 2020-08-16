/*
 * types.hpp - KVM types
 */

#pragma once

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
 * Light wrapper around C FAM structs.
 */
template<class Struct, class Buffer>
class FamStruct {
    protected:
        std::unique_ptr<Struct, void(*)(Struct*)> ptr_;

        FamStruct(const size_t n)
            : ptr_{reinterpret_cast<Struct*>(new Buffer[n]()),
                   [](Struct *p){ delete[] reinterpret_cast<Buffer*>(p); }} {}
    public:
        Struct* get() { return ptr_.get(); }
};

class MsrIndexList : public FamStruct<kvm_msr_list, uint32_t> {
    protected:
        MsrIndexList(const size_t n);
    public:
        MsrIndexList() : MsrIndexList(MAX_IO_MSRS) {}

        uint32_t nmsrs() const;

        uint32_t* begin();
        uint32_t* end();
        uint32_t const* begin() const;
        uint32_t const* end() const;
        uint32_t const* cbegin() const;
        uint32_t const* cend() const;
};

class MsrFeatureList : public MsrIndexList {
    public:
        MsrFeatureList() : MsrIndexList(MAX_IO_MSRS_FEATURES) {}
};

class MsrList : public FamStruct<kvm_msrs, uint64_t> {
    private:
        MsrList(const size_t n);
    public:
        MsrList(kvm_msr_entry entry);

        /**
         * Range constructor.
         *
         * Examples
         * ========
         * ```
         * #include <vmm/kvm.hpp>
         *
         * kvm::system kvm;
         * kvm::MsrFeatureList msr_list {kvm.msr_feature_list()};
         * std::vector<kvm_msr_entry> entries;
         *
         * for (auto msr : msr_list) {
         *     kvm_msr_entry entry{msr};
         *     entries.push_back(entry);
         * }
         *
         * kvm::MsrList msrs{entries.begin(), entries.end()};
         * ```
         */
        template <class Iterator>
        MsrList(Iterator first, Iterator last) : MsrList(std::distance(first, last)) {
            std::copy_if(first, last, ptr_->entries, [](kvm_msr_entry) { return true; });
        }

        /**
         * Container constructor.
         *
         * Examples
         * ========
         * ```
         * #include <vmm/kvm.hpp>
         *
         * kvm::system kvm;
         * kvm::MsrFeatureList msr_list {kvm.msr_feature_list()};
         * std::vector<kvm_msr_entry> entries;
         *
         * for (auto msr : msr_list) {
         *     kvm_msr_entry entry{msr};
         *     entries.push_back(entry);
         * }
         *
         * kvm::MsrList msrs{entries};
         * ```
         */
        template <class Container>
        MsrList(Container& c) : MsrList(c.begin(), c.end()) { }

        MsrList(const MsrList& other);
        MsrList(MsrList&& other) = default;
        MsrList& operator=(MsrList other);

        uint32_t nmsrs() const;

        kvm_msr_entry* begin();
        kvm_msr_entry* end();
        kvm_msr_entry const* begin() const;
        kvm_msr_entry const* end() const;
        kvm_msr_entry const* cbegin() const;
        kvm_msr_entry const* cend() const;
};

class CpuidList : public FamStruct<kvm_cpuid2, uint32_t> {
    private:
        CpuidList(const uint32_t n);
    public:
        CpuidList() : CpuidList(MAX_CPUID_ENTRIES) {}
        CpuidList(kvm_cpuid_entry2 entry);

        /**
         * Range constructor.
         *
         * Examples
         * ========
         * ```
         * #include <vmm/kvm.hpp>
         *
         * kvm::system kvm;
         * TODO
         * ```
         */
        template <class Iterator>
        CpuidList(Iterator first, Iterator last) : CpuidList(std::distance(first, last)) {
            std::copy_if(first, last, ptr_->entries, [](kvm_cpuid_entry2) { return true; });
        }

        /**
         * Container constructor.
         *
         * Examples
         * ========
         * ```
         * #include <vmm/kvm.hpp>
         *
         * kvm::system kvm;
         * TODO
         * ```
         */
        template <class Container>
        CpuidList(Container& c) : CpuidList(c.begin(), c.end()) { }

        CpuidList(const CpuidList& other);
        CpuidList(CpuidList&& other) = default;
        CpuidList& operator=(CpuidList other);

        uint32_t nent() const;

        kvm_cpuid_entry2* begin();
        kvm_cpuid_entry2* end();
        kvm_cpuid_entry2 const* begin() const;
        kvm_cpuid_entry2 const* end() const;
        kvm_cpuid_entry2 const* cbegin() const;
        kvm_cpuid_entry2 const* cend() const;
};

}  // namespace vmm::kvm::detail
