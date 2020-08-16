/*
 * vm.hpp - VM ioctls
 */

#pragma once

#include "vmm/kvm/detail/system.hpp"

namespace vmm::kvm::detail {

class vcpu;

class vm {
    private:
        unsigned int fd_;
        unsigned int mmap_size_;
        bool closed_;

        /**
         * Restricted constructor for kvm::system objects.
         */
        vm(const unsigned int fd, const unsigned int mmap_size) noexcept
            : fd_{fd}, mmap_size_{mmap_size}, closed_{false} {}
        friend vm system::vm(unsigned int machine_type);
    public:
        ~vm() noexcept;
        auto close(void) -> void;

        auto check_extension(unsigned int cap) -> unsigned int;

        auto vcpu(unsigned long vcpu_id) -> vmm::kvm::detail::vcpu;
        void set_bsp(unsigned long vcpu_id);
        void memslot(kvm_userspace_memory_region region);
        void irqchip(void);
        void get_irqchip(kvm_irqchip *irqchip_p);
        void set_irqchip(kvm_irqchip *irqchip_p);
        auto get_clock(void) -> kvm_clock_data;
        void set_clock(kvm_clock_data *clock);
        auto num_vcpus(void) -> unsigned int;
        auto max_vcpus(void) -> unsigned int;
        auto num_memslots(void) -> unsigned int;
};

}  // namespace vmm::kvm::detail
