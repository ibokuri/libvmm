/*
 * vm.cpp - VM ioctls
 */

#include "vmm/kvm/detail/vm.hpp"
#include "vmm/utility/utility.hpp"

namespace vmm::kvm::detail {

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

}  // namespace vmm::kvm::detail
