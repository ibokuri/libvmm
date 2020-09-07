/*
 * vcpu.hpp - vcpu ioctls
 */

#pragma once

#include "vmm/kvm/detail/vm.hpp"
#include "vmm/kvm/detail/base.hpp"

namespace vmm::kvm::detail {

class vcpu {
    private:
        KvmFd fd_;

        /**
         * Restricted constructor for kvm::vm objects.
         */
        vcpu(unsigned int fd) noexcept : fd_{fd} {}
        friend vcpu vm::vcpu(unsigned long vcpu_id);
};

}  // namespace vmm::kvm::detail
