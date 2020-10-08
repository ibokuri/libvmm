/*
 * vcpu.hpp - vcpu ioctls
 */

#pragma once

#include "vmm/kvm/detail/ioctls/vm.hpp"
#include "vmm/kvm/detail/types/file_descriptor.hpp"

namespace vmm::kvm::detail {

class vcpu {
    friend vcpu vm::vcpu(unsigned vcpu_id) const;

    private:
        KvmFd m_fd;

        explicit vcpu(int fd) noexcept : m_fd{fd} {}
};

}  // namespace vmm::kvm::detail
