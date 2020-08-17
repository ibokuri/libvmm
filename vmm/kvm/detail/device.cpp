#include "vmm/kvm/detail/device.hpp"

namespace vmm::kvm::detail {

auto device::get_attr(kvm_device_attr &attr) -> void {
    utility::ioctl(fd_, KVM_GET_DEVICE_ATTR, &attr);
}

auto device::set_attr(kvm_device_attr &attr) -> void {
    utility::ioctl(fd_, KVM_SET_DEVICE_ATTR, &attr);
}

auto device::has_attr(kvm_device_attr &attr) -> void {
    utility::ioctl(fd_, KVM_HAS_DEVICE_ATTR, &attr);
}

}  // namespace vmm::kvm::detail
