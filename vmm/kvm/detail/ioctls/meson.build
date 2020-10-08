kvm_internal_ioctl_sources = files(
    'device.cpp',
    'system.cpp',
    'vcpu.cpp',
    'vm.cpp',
)

kvm_internal_ioctl_headers = files(
  'device.hpp',
  'system.hpp',
  'vcpu.hpp',
  'vm.hpp',
)

sources += kvm_internal_ioctl_sources

install_headers(kvm_internal_ioctl_headers, subdir: 'vmm/kvm/detail/ioctls')
