/*
 * vm.cpp - VM ioctls
 */

#include "vmm/kvm/detail/vm.hpp"
#include "vmm/kvm/detail/vcpu.hpp"
#include "vmm/kvm/detail/device.hpp"
#include "vmm/utility/utility.hpp"

namespace vmm::kvm::detail {

/**
 * Adds a vcpu to a virtual machine.
 *
 * See the documentation for KVM_CREATE_VCPU.
 *
 * Examples
 * ========
 * ```
 * #include <vmm/kvm.hpp>
 *
 * vmm::kvm::system kvm;
 * auto vm {kvm.vm()};
 * auto vcpu {vm.vcpu(0)};
 * ```
 */
auto vm::vcpu(unsigned long vcpu_id) -> vmm::kvm::detail::vcpu {
    return vmm::kvm::detail::vcpu{utility::ioctl(fd_, KVM_CREATE_VCPU, vcpu_id)};
}

/**
 * Adds a device to a virtual machine.
 *
 * See the documentation for KVM_CREATE_DEVICE.
 *
 * Examples
 * ========
 * ```
 * #include <vmm/kvm.hpp>
 *
 * vmm::kvm::system kvm;
 * auto vm {kvm.vm()};
 * auto device {vm.device(TODO)};
 * ```
 */
auto vm::device(const unsigned int type, const unsigned int flags) -> vmm::kvm::detail::device {
    kvm_create_device dev {
        .type = type,
        .flags = flags
    };
    auto fd {utility::ioctl(fd_, KVM_CREATE_DEVICE, &dev)};
    return vmm::kvm::detail::device{fd, dev};
}

/**
 * Defines which vcpu is the Bootstrap Processor (BSP).
 *
 * The KVM_SET_BOOT_CPU_ID ioctl must be called before any vcpus are created
 * for a VM, otherwise the call will fail.
 *
 * See the documentation for KVM_SET_BOOT_CPU_ID.
 *
 * Architectures
 * =============
 * x86
 *
 * Examples
 * ========
 * ```
 * #include <vmm/kvm.hpp>
 *
 * vmm::kvm::system kvm;
 * auto vm {kvm.vm()};
 *
 * if (vm.check_extension(KVM_CAP_SET_BOOT_CPU_ID) > 0)
 *     throw;
 *
 * vm.set_bsp(0);
 * ```
 */
void vm::set_bsp(unsigned long vcpu_id) {
    utility::ioctl(fd_, KVM_SET_BOOT_CPU_ID, vcpu_id);
}

/**
 * Creates, modifies, or deletes a guest physical memory slot.
 *
 * See the documentation for KVM_SET_USER_MEMORY_REGION.
 *
 * Examples
 * ========
 * ```
 * #include <vmm/kvm.hpp>
 *
 * vmm::kvm::system kvm;
 * vmm::kvm::vm vm {kvm.vm()};
 * kvm_userspace_memory_region mem_region {
 *     .slot = 0,
 *     .flags = 0,
 *     .guest_phys_addr = 0x10000,
 *     .memory_size = 0x10000,
 *     .userspace_addr = 0,
 * };
 *
 * vm.memslot(mem_region);
 * ```
 */
void vm::memslot(kvm_userspace_memory_region region) {
    utility::ioctl(fd_, KVM_SET_USER_MEMORY_REGION, &region);
}

/**
 * Creates an interrupt controller model in the kernel
 *
 * See the documentation for `KVM_CREATE_IRQCHIP`.
 *
 * Architectures
 * =============
 * x86, x86_64, arm, aarch64
 *
 * Examples
 * ========
 * ```
 * #include <vmm/kvm.hpp>
 *
 * vmm::kvm::system kvm;
 * auto vm {kvm.vm()};
 * vm.irqchip();
 * ```
 */
void vm::irqchip(void) {
    utility::ioctl(fd_, KVM_CREATE_IRQCHIP);
}

/**
 * Reads the state of a kernel interrupt controller into a buffer provided by
 * the caller.
 *
 * See the documentation for `KVM_GET_IRQCHIP`.
 *
 * Architectures
 * =============
 * x86
 *
 * Examples
 * ========
 * ```
 * #include <vmm/kvm.hpp>
 *
 * vmm::kvm::system kvm;
 * auto vm {kvm.vm()};
 * kvm_irqchip irqchip { .chip_id = KVM_IRQCHIP_PIC_MASTER };
 *
 * vm.irqchip();
 * vm.getirqchip(&irqchip);
 * ```
 */
void vm::get_irqchip(kvm_irqchip *irqchip_p) {
    utility::ioctl(fd_, KVM_GET_IRQCHIP, irqchip_p);
}

/**
 * Sets the state of a kernel interrupt controller from a buffer provided by
 * the caller.
 *
 * See the documentation for `KVM_SET_IRQCHIP`.
 *
 * Architectures
 * =============
 * x86
 *
 * Examples
 * ========
 * ```
 * #include <vmm/kvm.hpp>
 *
 * vmm::kvm::system kvm;
 * auto vm {kvm.vm()};
 * kvm_irqchip irqchip {
 *     .chip_id = KVM_IRQCHIP_PIC_MASTER,
 *     .chip.pic.irq_base = 99
 * };
 *
 * vm.irqchip();
 * vm.set_irqchip(&irqchip);
 * ```
 */
void vm::set_irqchip(kvm_irqchip *irqchip_p) {
    utility::ioctl(fd_, KVM_SET_IRQCHIP, irqchip_p);
}

/**
 * Gets the current timestamp of kvmclock as seen by the current guest.
 *
 * See the documentation for `KVM_GET_CLOCK`.
 *
 * Architectures
 * =============
 * x86
 *
 * Examples
 * ========
 * ```
 * #include <vmm/kvm.hpp>
 *
 * vmm::kvm::system kvm;
 * auto vm {kvm.vm()};
 * auto clock {vm.get_clock(&clock)};
 * ```
 */
auto vm::get_clock(void) -> kvm_clock_data {
    kvm_clock_data clock {0};
    utility::ioctl(fd_, KVM_GET_CLOCK, &clock);
    return clock;
}

/**
 * Sets the current timestamp of kvmclock.
 *
 * See the documentation for `KVM_SET_CLOCK`.
 *
 * Architectures
 * =============
 * x86
 *
 * Examples
 * ========
 * ```
 * #include <vmm/kvm.hpp>
 *
 * vmm::kvm::system kvm;
 * auto vm {kvm.vm()};
 * kvm_clock_data clock { .clock = 10 };
 *
 * vm.set_clock(&clock);
 * ```
 */
void vm::set_clock(kvm_clock_data *clock) {
    utility::ioctl(fd_, KVM_SET_CLOCK, clock);
}

/**
 * Returns a positive integer if a KVM extension is available; 0 otherwise.
 *
 * Examples
 * ========
 * ```
 * #include <cassert>
 * #include <vmm/kvm.hpp>
 *
 * vmm::kvm::system kvm;
 * vmm::kvm::vm vm {kvm.vm()};
 *
 * assert(vm.check_extension(KVM_CAP_ARM_VM_IPA_SIZE) >= 32);
 * ```
 */
auto vm::check_extension(unsigned int cap) -> unsigned int {
    return utility::ioctl(fd_, KVM_CHECK_EXTENSION, cap);
}

auto vm::num_vcpus(void) -> unsigned int {
    auto ret {check_extension(KVM_CAP_NR_VCPUS)};
    return ret > 0 ? ret : 4;
}

auto vm::max_vcpus(void) -> unsigned int {
    auto ret {check_extension(KVM_CAP_MAX_VCPUS)};
    return ret > 0 ? ret : num_vcpus();
}

auto vm::num_memslots(void) -> unsigned int {
    auto ret {check_extension(KVM_CAP_NR_MEMSLOTS)};
    return ret > 0 ? ret : 32;
}

}  // namespace vmm::kvm::detail
