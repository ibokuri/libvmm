/*
 * vcpu.hpp - vcpu ioctls
 */

#pragma once

#include "vmm/kvm/detail/types.hpp"
#include "vmm/kvm/detail/vm.hpp"

namespace vmm::kvm::detail {

class vcpu {
    private:
        KvmFd fd_;

        explicit vcpu(int fd) noexcept : fd_{fd} {}
        friend vcpu vm::vcpu(unsigned int vcpu_id) const;
};

}  // namespace vmm::kvm::detail
