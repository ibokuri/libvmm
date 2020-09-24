/*
 * types.hpp - KVM types
 */

#pragma once

#include <algorithm> // copy_if, swap
#include <iterator> // distance
#include <linux/kvm.h> // kvm_*

#include "vmm/kvm/detail/macros.hpp"
#include "vmm/types/detail/fam_struct.hpp"

namespace vmm::kvm::detail {

class system;

/**
 * struct kvm_msr_list {
 *     __u32 nmsrs;
 *     __u32 indices[0];
 * };
 */
template<std::size_t N>
class MsrList : public fam_struct<kvm_msr_list, uint32_t, N> {
    using Base = fam_struct<kvm_msr_list, uint32_t, N>;

    public:
        using value_type = typename Base::value_type;
        using size_type = typename Base::size_type;

        // Capacity
        [[nodiscard]] auto size() const noexcept -> size_type {
            return this->struct_.nmsrs;
        }

        [[nodiscard]] auto empty() const noexcept -> bool {
            return size() == 0;
        }
    private:
        explicit MsrList() noexcept { this->struct_.nmsrs = N; };
        friend system;
};

/**
 * struct kvm_msrs {
 *     __u32 nmsrs;
 *     __u32 pad;
 *     struct kvm_msr_entry entries[0];
 * };
 *
 * struct kvm_msr_entry {
 *     __u32 index;
 *     __u32 reserved;
 *     __u64 data;
 * };
 */
template<std::size_t N>
class Msrs : public fam_struct<kvm_msrs, kvm_msr_entry, N> {
    using Base = fam_struct<kvm_msrs, kvm_msr_entry, N>;

    public:
        using value_type = typename Base::value_type;
        using size_type = typename Base::size_type;

        explicit Msrs() noexcept {
            this->struct_.nmsrs = N;
        };

        template <typename Iterator>
        explicit Msrs(Iterator first, Iterator last) {
            std::copy_if(first, last, this->entries_.begin(), [](value_type) { return true; });
        }

        template <typename Container>
        explicit Msrs(Container& c) : Msrs(c.begin(), c.end()) {
            //static_assert(std::is_same<value_type, typename Container::value_type>::value);
        }

        explicit Msrs(value_type entry) noexcept {
            static_assert(N == 1);
            this->struct_ = {.nmsrs = N};
            this->entries_ = {entry};
        }

        // Capacity
        [[nodiscard]] auto size() const noexcept -> size_type {
            return this->struct_.nmsrs;
        }

        [[nodiscard]] auto empty() const noexcept -> bool {
            return size() == 0;
        }
};

/*
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
 */
template<std::size_t N>
class Cpuids : public fam_struct<kvm_cpuid2, kvm_cpuid_entry2, N> {
    using Base = fam_struct<kvm_cpuid2, kvm_cpuid_entry2, N>;

    public:
        using value_type = typename Base::value_type;
        using size_type = typename Base::size_type;

        explicit Cpuids() noexcept {
            this->struct_ = {.nent = N};
        }

        explicit Cpuids(value_type entry) noexcept {
            static_assert(N == 1);
            this->struct_ = {.nent = N};
            this->entries_ = {entry};
        }

        template <typename Iterator>
        explicit Cpuids(Iterator first, Iterator last) {
            std::copy_if(first, last, this->entries_.begin(), [](value_type) { return true; });
            this->struct_ = {std::distance(first, last)};
        }

        template <typename Container>
        explicit Cpuids(Container& c) : Cpuids(c.begin(), c.end()) { }

        // Capacity
        [[nodiscard]] auto size() const noexcept -> size_type {
            return this->struct_.nent;
        }

        [[nodiscard]] auto empty() const noexcept -> bool {
            return size() == 0;
        }
};

/**
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
template<std::size_t N>
class IrqRouting : public fam_struct<kvm_irq_routing, kvm_irq_routing_entry, N> {
    using Base = fam_struct<kvm_irq_routing, kvm_irq_routing_entry, N>;

    public:
        using value_type = typename Base::value_type;
        using size_type = typename Base::size_type;

        explicit IrqRouting() noexcept {
            this->struct_.nr = N;
        };

        template <typename Iterator>
        explicit IrqRouting(Iterator first, Iterator last) {
            std::copy_if(first, last, this->entries_.begin(), [](value_type) {return true;});
        }

        template <typename Container>
        explicit IrqRouting(Container& c) : IrqRouting(c.begin(), c.end()) {
            //static_assert(std::is_same<value_type, typename Container::value_type>::value);
        }

        explicit IrqRouting(value_type entry) noexcept {
            static_assert(N == 1);
            this->struct_ = {.nr = N};
            this->entries_ = {entry};
        }

        // Capacity
        [[nodiscard]] auto size() const noexcept -> size_type {
            return this->struct_.nr;
        }

        [[nodiscard]] auto empty() const noexcept -> bool {
            return size() == 0;
        }
};

}  // namespace vmm::kvm::detail
