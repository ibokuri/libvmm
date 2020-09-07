/*
 * vm.hpp - VM ioctls
 */

#pragma once

#include "vmm/kvm/detail/base.hpp"
#include "vmm/kvm/detail/system.hpp"
#include "vmm/kvm/detail/types.hpp"

#include <linux/kvm.h>

namespace vmm::kvm::detail {

class vcpu;
class device;

class vm {
    private:
        KvmFd fd_;
        unsigned int mmap_size_;

        vm(const unsigned int fd, unsigned int mmap_size) noexcept
            : fd_{fd}, mmap_size_{mmap_size} {}

        friend vm system::vm(unsigned int machine_type);
    public:
        vm(const vm& other) = delete;
        vm(vm&& other) = default;
        auto operator=(const vm& other) -> vm& = delete;
        auto operator=(vm&& other) -> vm& = default;

        // Creation routines
        [[nodiscard]] auto vcpu(unsigned long vcpu_id) -> vmm::kvm::detail::vcpu;
        [[nodiscard]] auto device(unsigned int type, unsigned int flags=0) -> vmm::kvm::detail::device;

        // Control routines
        [[nodiscard]] auto check_extension(unsigned int cap) -> unsigned int;
        auto set_bsp(unsigned long vcpu_id) -> void;
        auto memslot(kvm_userspace_memory_region region) -> void;
        auto irqchip(void) -> void;
        auto get_irqchip(kvm_irqchip &irqchip_p) -> void;
        auto set_irqchip(kvm_irqchip &irqchip_p) -> void;
        [[nodiscard]] auto get_clock(void) -> kvm_clock_data;
        auto set_clock(kvm_clock_data &clock) -> void;
        auto gsi_routing(IrqRoutingList &routing_list) -> void;

        // Convenient routines
        [[nodiscard]] auto mmap_size(void) -> unsigned int;
        [[nodiscard]] auto num_vcpus(void) -> unsigned int;
        [[nodiscard]] auto max_vcpus(void) -> unsigned int;
        [[nodiscard]] auto num_memslots(void) -> unsigned int;
};

}  // namespace vmm::kvm::detail
