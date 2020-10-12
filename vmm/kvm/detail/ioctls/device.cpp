/*
 * device.cpp - device ioctls
 */

#include "vmm/kvm/detail/ioctls/device.hpp"

namespace vmm::kvm::detail {

auto device::get_attr(kvm_device_attr &attr) -> void
{
    m_fd.ioctl(KVM_GET_DEVICE_ATTR, &attr);
}

auto device::set_attr(kvm_device_attr &attr) -> void
{
    m_fd.ioctl(KVM_SET_DEVICE_ATTR, &attr);
}

/*
 * Checks whether an attribute for a device is supported.
 *
 * Examples
 * ========
 * ```
 * #include <vmm/kvm.hpp>
 *
 * auto kvm = vmm::kvm::system{};
 * auto vm = kvm.vm();
 * auto dev = vm.device(KVM_DEV_TYPE_VFIO);
 * auto vfio_fd = 0u;
 *
 * FIXME: set_attr() will give a "Invalid argument" error due to .addr's value.
 *
 * auto attr = kvm_device_attr{
 *     .flags = 0,
 *     .group = KVM_DEV_VFIO_GROUP,
 *     .attr = KVM_DEV_VFIO_GROUP_ADD,
 *     .addr = reinterpret_cast<uint64_t>(&vfio_fd)
 * };
 *
 * if (dev.has_attr(attr))
 *    dev.set_attr(attr);
 * ```
 */
auto device::has_attr(kvm_device_attr &attr) -> bool
{
    return m_fd.ioctl(KVM_HAS_DEVICE_ATTR, &attr) == 0;
}

}  // namespace vmm::kvm::detail
