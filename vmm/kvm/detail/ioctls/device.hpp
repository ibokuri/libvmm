//
// device.hpp - device ioctls
//

#pragma once

#include <cstdint> // uint32_t
#include <linux/kvm.h> // kvm_*, KVM_*

#include "vmm/kvm/detail/ioctls/vm.hpp"
#include "vmm/kvm/detail/types/file_descriptor.hpp"

namespace vmm::kvm::detail {

class Device
{
    private:
        KvmFd m_fd;
        uint32_t m_type;
        uint32_t m_flags;

        friend Device Vm::device(uint32_t type, uint32_t flags) const;

        Device(const kvm_create_device& dev) noexcept
            : m_fd{static_cast<int>(dev.fd)}, m_type{dev.type},
              m_flags{dev.flags} {}
    public:
        // Checks whether an attribute for a device is supported.
        //
        // See the documentation for KVM_HAS_DEVICE_ATTR.
        auto has_attr(kvm_device_attr&) -> bool;

        // Gets a specified piece of device configuration/state.
        //
        // See the documentation for KVM_GET_DEVICE_ATTR.
        auto get_attr(kvm_device_attr&) -> void;

        // Sets a specified piece of device configuration/state.
        //
        // See the documentation for KVM_SET_DEVICE_ATTR.
        auto set_attr(kvm_device_attr&) -> void;
};

}  // namespace vmm::kvm::detail
