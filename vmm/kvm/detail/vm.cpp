/*
 * vm.cpp - VM ioctls
 */

#include "vmm/kvm/detail/vm.hpp"
#include "vmm/kvm/detail/vcpu.hpp"
#include "vmm/utility/utility.hpp"

namespace vmm::kvm::detail {

/* Creates, modifies, or deletes a guest physical memory slot.
 *
 * See documentation for `KVM_SET_USER_MEMORY_REGION`.
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
 * vm.user_memory_region(mem_region);
 * ```
 */
void vm::user_memory_region(kvm_userspace_memory_region region) {
    utility::ioctl(fd_, KVM_SET_USER_MEMORY_REGION, &region);
}

/* Adds a vcpu to a virtual machine.
 *
 * See documentation for `KVM_CREATE_VCPU`.
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
auto vm::vcpu(uint8_t id) -> vmm::kvm::detail::vcpu {
    return vmm::kvm::detail::vcpu{utility::ioctl(fd_, KVM_CREATE_VCPU, id)};
}

vm::~vm() noexcept {
    if (!closed_) {
        try {
            utility::close(fd_);
        }
        catch (std::system_error& e) {
            // TODO
        }
    }
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

auto vm::nr_vcpus() -> unsigned int {
    auto ret {check_extension(KVM_CAP_NR_VCPUS)};
    return ret > 0 ? ret : 4;
}

auto vm::max_vcpus() -> unsigned int {
    auto ret {check_extension(KVM_CAP_MAX_VCPUS)};
    return ret > 0 ? ret : nr_vcpus();
}

auto vm::nr_memslots() -> unsigned int {
    auto ret {check_extension(KVM_CAP_NR_MEMSLOTS)};
    return ret > 0 ? ret : 32;
}

/**
 * Closes the VM handle.
 *
 * Use this if you'd like to handle possible failures of `utility::close()`.
 *
 * Examples
 * ========
 * ```
 * #include <vmm/kvm.hpp>
 *
 * kvm::system kvm;
 * kvm::vm vm {kvm.vm()};
 *
 * try {
 *     vm.close();
 * }
 * catch (std::system_error)
 *     throw;
 * ```
 */
auto vm::close() -> void {
    utility::close(fd_);
    closed_ = true;
}

}  // namespace vmm::kvm::detail
