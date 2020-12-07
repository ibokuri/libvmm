//
// system.cpp - KVM ioctls
//

#include "vmm/kvm/detail/ioctls/system.hpp"
#include "vmm/kvm/detail/ioctls/vm.hpp"

namespace vmm::kvm::detail {

auto System::api_version() const -> int
{
    return m_fd.ioctl(KVM_GET_API_VERSION);
}

auto System::vm(std::size_t machine_type) const -> vmm::kvm::detail::vm
{
    return vmm::kvm::detail::vm{m_fd.ioctl(KVM_CREATE_VM, machine_type),
                                vcpu_mmap_size()};
}

auto System::check_extension(int cap) const -> int
{
    return m_fd.ioctl(KVM_CHECK_EXTENSION, cap);
}

auto System::vcpu_mmap_size() const -> std::size_t
{
    return static_cast<std::size_t>(m_fd.ioctl(KVM_GET_VCPU_MMAP_SIZE));
}

#if defined(__arm__)  || defined(__aarch64__)
auto System::host_ipa_limit() const -> std::size_t
{
    return static_cast<std::size_t>(m_fd.ioctl(KVM_CAP_ARM_VM_IPA_SIZE));
}
#endif

}  // namespace vmm::kvm::detail
