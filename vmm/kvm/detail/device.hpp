/*
 * device.hpp - device ioctls
 */

#pragma once

#include "vmm/kvm/detail/vm.hpp"
#include "vmm/kvm/detail/base.hpp"

namespace vmm::kvm::detail {

class device : public KvmIoctl {
    private:
        unsigned int type_;
        unsigned int flags_;

        device(const unsigned int fd, const kvm_create_device dev) noexcept
            : KvmIoctl(fd), type_{dev.type}, flags_{dev.flags} {}
        friend device vm::device(unsigned int type, unsigned int flags);
};

}  // namespace vmm::kvm::detail
