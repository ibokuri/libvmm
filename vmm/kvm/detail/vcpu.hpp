/*
 * vcpu.hpp - vcpu ioctls
 */

#pragma once

#include "vmm/kvm/detail/vm.hpp"
#include "vmm/kvm/detail/base.hpp"

namespace vmm::kvm::detail {

class vcpu : public KvmIoctl {
    private:
        /**
         * Restricted constructor for kvm::vm objects.
         */
        vcpu(const unsigned int fd) noexcept : KvmIoctl(fd) {}
        friend vcpu vm::vcpu(unsigned long vcpu_id);
};

}  // namespace vmm::kvm::detail
