kvm_internal_sources = files(
    'device.cpp',
    'types.cpp',
    'system.cpp',
    'vcpu.cpp',
    'vm.cpp',
)

kvm_internal_headers = files(
  'base.hpp',
  'device.hpp',
  'macros.hpp',
  'types.hpp',
  'system.hpp',
  'vcpu.hpp',
  'vm.hpp',
)

sources += kvm_internal_sources

install_headers(kvm_internal_headers, subdir: 'vmm/kvm/detail')
