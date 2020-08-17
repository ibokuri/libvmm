kvm_internal_sources = files(
    'base.cpp',
    'device.cpp',
    'types.cpp',
    'system.cpp',
    'vcpu.cpp',
    'vm.cpp',
)

sources += kvm_internal_sources

kvm_internal_headers = files(
  'base.hpp',
  'device.hpp',
  'types.hpp',
  'system.hpp',
  'vcpu.hpp',
  'vm.hpp',
)

install_headers(kvm_internal_headers, subdir: 'vmm/kvm/detail')
