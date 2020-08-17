kvm_internal_sources = files(
    'device.cpp',
    'types.cpp',
    'system.cpp',
    'vcpu.cpp',
    'vm.cpp',
)

sources += kvm_internal_sources

kvm_internal_headers = files(
  'device.hpp',
  'base.hpp',
  'types.hpp',
  'system.hpp',
  'vcpu.hpp',
  'vm.hpp',
)

install_headers(kvm_internal_headers, subdir: 'vmm/kvm/detail')
