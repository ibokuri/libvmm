/*
 * vm.hpp - VM ioctls
 */

#pragma once

#include "vmm/kvm/detail/base.hpp"
#include "vmm/kvm/detail/system.hpp"

namespace vmm::kvm::detail {

class vcpu;
class device;

class vm : public KvmIoctl {
    private:
        unsigned int mmap_size_;

        // Restricted constructor for kvm::system objects.
        vm(const unsigned int fd, const unsigned int mmap_size) noexcept
            : KvmIoctl(fd), mmap_size_{mmap_size} {}
        friend vm system::vm(unsigned int machine_type);
    public:
        // Creation routines
        auto vcpu(unsigned long vcpu_id) -> vmm::kvm::detail::vcpu;
        auto device(const unsigned int type, const unsigned int flags=0) -> vmm::kvm::detail::device;

        // Control routines
        auto check_extension(const unsigned int cap) -> unsigned int;
        auto set_bsp(unsigned long vcpu_id) -> void;
        auto memslot(kvm_userspace_memory_region region) -> void;
        auto irqchip(void) -> void;
        auto get_irqchip(kvm_irqchip &irqchip_p) -> void;
        auto set_irqchip(kvm_irqchip &irqchip_p) -> void;
        auto get_clock(void) -> kvm_clock_data;
        auto set_clock(kvm_clock_data &clock) -> void;
        auto gsi_routing(IrqRoutingList &routing_list) -> void;

        // Convenient routines
        auto num_vcpus(void) -> unsigned int;
        auto max_vcpus(void) -> unsigned int;
        auto num_memslots(void) -> unsigned int;
};

}  // namespace vmm::kvm::detail
