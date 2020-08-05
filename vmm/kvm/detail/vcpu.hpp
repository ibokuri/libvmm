/*
 * vcpu.hpp - vcpu ioctls
 */

#pragma once

#include "vmm/kvm/detail/vm.hpp"

namespace vmm::kvm::detail {

class vcpu {
    private:
        unsigned int fd_;
        bool closed_;

        /**
         * Restricted constructor for kvm::vm objects.
         */
        vcpu(unsigned int fd) noexcept : fd_{fd}, closed_{false} {}
        friend vcpu vm::vcpu(uint8_t id);
    public:
        ~vcpu() noexcept;
        auto close() -> void;
};

}  // namespace vmm::kvm::detail
