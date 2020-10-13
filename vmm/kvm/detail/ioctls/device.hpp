//
// device.hpp - device ioctls
//

#pragma once

#include <cstdint> // uint32_t
#include <linux/kvm.h> // kvm_*, KVM_*

#include "vmm/kvm/detail/ioctls/vm.hpp"
#include "vmm/kvm/detail/types/file_descriptor.hpp"

namespace vmm::kvm::detail {

class device
{
    friend device vm::device(uint32_t type, uint32_t flags) const;

    public:
        auto get_attr(kvm_device_attr&) -> void;
        auto set_attr(kvm_device_attr&) -> void;

        // Checks whether an attribute for a device is supported.
        //
        // Examples
        // ========
        // ```
        // #include <vmm/kvm.hpp>
        //
        // auto kvm = vmm::kvm::system{};
        // auto vm = kvm.vm();
        // auto dev = vm.device(KVM_DEV_TYPE_VFIO);
        // auto vfio_fd = 0u;
        //
        // FIXME: set_attr() will give a "Invalid argument" error due to
        // .addr's value.
        //
        // auto attr = kvm_device_attr{
        //     .flags = 0,
        //     .group = KVM_DEV_VFIO_GROUP,
        //     .attr = KVM_DEV_VFIO_GROUP_ADD,
        //     .addr = reinterpret_cast<uint64_t>(&vfio_fd)
        // };
        //
        // if (dev.has_attr(attr))
        //    dev.set_attr(attr);
        // ```
        auto has_attr(kvm_device_attr&) -> bool;
    private:
        KvmFd m_fd;
        uint32_t m_type;
        uint32_t m_flags;

        device(const kvm_create_device& dev) noexcept
            : m_fd{static_cast<int>(dev.fd)}, m_type{dev.type},
              m_flags{dev.flags} {}
};

}  // namespace vmm::kvm::detail
