//
// system.cpp - KVM ioctls
//

#include "vmm/kvm/detail/ioctls/system.hpp"
#include "vmm/kvm/detail/ioctls/vm.hpp"

namespace vmm::kvm::detail {

auto system::api_version() const -> unsigned
{
    return m_fd.ioctl(KVM_GET_API_VERSION);
}

auto system::create_vm(unsigned machine_type) const -> int
{
     return m_fd.ioctl(KVM_CREATE_VM, machine_type);
}

auto system::vm(unsigned machine_type) const -> vmm::kvm::detail::vm
{
    return vmm::kvm::detail::vm{create_vm(machine_type), vcpu_mmap_size()};
}

auto system::check_extension(unsigned cap) const -> unsigned
{
    return m_fd.ioctl(KVM_CHECK_EXTENSION, cap);
}

auto system::vcpu_mmap_size() const -> std::size_t
{
    return m_fd.ioctl(KVM_GET_VCPU_MMAP_SIZE);
}

#if defined(__arm__)  || defined(__aarch64__)
auto system::host_ipa_limit() const -> unsigned
{
    return check_extension(KVM_CAP_ARM_VM_IPA_SIZE);
}
#endif

}  // namespace vmm::kvm::detail
