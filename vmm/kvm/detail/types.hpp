/*
 * types.hpp - KVM types
 */

#pragma once

#include <algorithm>
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

/**
 * Defined in linux/arch/x86/include/asm/kvm_host.h
 */
#define MAX_CPUID_ENTRIES 80

namespace vmm::kvm::detail {

/**
 * Light wrapper around C FAM structs.
 */
template<class Struct, class Buffer>
class FamStruct {
    protected:
        std::unique_ptr<Struct, void(*)(Struct*)> ptr_;

        FamStruct(const size_t n) : ptr_{reinterpret_cast<Struct*>(new Buffer[n]()),
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

class Msrs : public FamStruct<kvm_msrs, uint64_t> {
    private:
        Msrs(const size_t n);
    public:
        Msrs(kvm_msr_entry entry);

        /**
         * Range constructor.
         *
         * # Examples
         *
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
         * kvm::Msrs msrs{entries.begin(), entries.end()};
         * ```
         */
        template <class Iterator>
        Msrs(Iterator first, Iterator last) : Msrs(std::distance(first, last)) {
            std::copy_if(first, last, ptr_->entries, [](kvm_msr_entry) { return true; });
        }

        /**
         * Container constructor.
         *
         * # Examples
         *
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
         * kvm::Msrs msrs{entries};
         * ```
         */
        template <class Container>
        Msrs(Container& c) : Msrs(c.begin(), c.end()) { }

        Msrs(const Msrs& other);
        Msrs(Msrs&& other) = default;
        Msrs& operator=(Msrs other);

        uint32_t nmsrs() const;

        kvm_msr_entry* begin();
        kvm_msr_entry* end();
        kvm_msr_entry const* begin() const;
        kvm_msr_entry const* end() const;
        kvm_msr_entry const* cbegin() const;
        kvm_msr_entry const* cend() const;
};

class Cpuid : public FamStruct<kvm_cpuid2, uint32_t> {
    private:
        Cpuid(const uint32_t n);
    public:
        Cpuid() : Cpuid(MAX_CPUID_ENTRIES) {}
        Cpuid(kvm_cpuid_entry2 entry);

        /**
         * Range constructor.
         *
         * # Examples
         *
         * ```
         * #include <vmm/kvm.hpp>
         *
         * kvm::system kvm;
         * TODO
         * ```
         */
        template <class Iterator>
        Cpuid(Iterator first, Iterator last) : Cpuid(std::distance(first, last)) {
            std::copy_if(first, last, ptr_->entries, [](kvm_cpuid_entry2) { return true; });
        }

        /**
         * Container constructor.
         *
         * # Examples
         *
         * ```
         * #include <vmm/kvm.hpp>
         *
         * kvm::system kvm;
         * TODO
         * ```
         */
        template <class Container>
        Cpuid(Container& c) : Cpuid(c.begin(), c.end()) { }

        Cpuid(const Cpuid& other);
        Cpuid(Cpuid&& other) = default;
        Cpuid& operator=(Cpuid other);

        uint32_t nent() const;

        kvm_cpuid_entry2* begin();
        kvm_cpuid_entry2* end();
        kvm_cpuid_entry2 const* begin() const;
        kvm_cpuid_entry2 const* end() const;
        kvm_cpuid_entry2 const* cbegin() const;
        kvm_cpuid_entry2 const* cend() const;
};

}  // namespace vmm::kvm::detail
