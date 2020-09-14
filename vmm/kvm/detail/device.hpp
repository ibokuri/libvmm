/*
 * device.hpp - device ioctls
 */

#pragma once

#include <linux/kvm.h> // kvm_*

#include "vmm/kvm/detail/base.hpp"
#include "vmm/kvm/detail/vm.hpp"

namespace vmm::kvm::detail {

class device {
    private:
        KvmFd fd_;
        unsigned int type_;
        unsigned int flags_;

        device(const kvm_create_device& dev) noexcept
            : fd_{dev.fd}, type_{dev.type}, flags_{dev.flags} {}
        friend device vm::device(unsigned int type, unsigned int flags);
    public:
        auto get_attr(kvm_device_attr &attr) -> void;
        auto set_attr(kvm_device_attr &attr) -> void;
        auto has_attr(kvm_device_attr &attr) -> bool;
};

}  // namespace vmm::kvm::detail
