//
// device.cpp - device ioctls
//

#include "vmm/kvm/detail/ioctls/device.hpp"

namespace vmm::kvm::detail {

auto Device::has_attr(kvm_device_attr &attr) -> bool
{
    return m_fd.ioctl(KVM_HAS_DEVICE_ATTR, &attr) == 0;
}

auto Device::get_attr(kvm_device_attr &attr) -> void
{
    m_fd.ioctl(KVM_GET_DEVICE_ATTR, &attr);
}

auto Device::set_attr(kvm_device_attr &attr) -> void
{
    m_fd.ioctl(KVM_SET_DEVICE_ATTR, &attr);
}

}  // namespace vmm::kvm::detail
