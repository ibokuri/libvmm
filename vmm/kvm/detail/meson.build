kvm_internal_sources = files(
    'device.cpp',
    'system.cpp',
    'vcpu.cpp',
    'vm.cpp',
)

kvm_internal_headers = files(
  'device.hpp',
  'macros.hpp',
  'types.hpp',
  'system.hpp',
  'vcpu.hpp',
  'vm.hpp',
)

sources += kvm_internal_sources

install_headers(kvm_internal_headers, subdir: 'vmm/kvm/detail')
