/*
 * device.hpp - device ioctls
 */

#pragma once

#include <cstdint> // uint32_t
#include <linux/kvm.h> // kvm_*, KVM_*

#include "vmm/kvm/detail/types.hpp"
#include "vmm/kvm/detail/vm.hpp"

namespace vmm::kvm::detail {

class device {
    friend device vm::device(uint32_t type, uint32_t flags) const;

    public:
        auto get_attr(kvm_device_attr&) -> void;
        auto set_attr(kvm_device_attr&) -> void;
        auto has_attr(kvm_device_attr&) -> bool;
    private:
        KvmFd m_fd;
        uint32_t m_type;
        uint32_t m_flags;

        device(const kvm_create_device& dev) noexcept
            : m_fd{static_cast<int>(dev.fd)},
              m_type{dev.type},
              m_flags{dev.flags} {}
};

}  // namespace vmm::kvm::detail
