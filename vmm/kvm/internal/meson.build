sources += files(
    'types.cpp',
    'system.cpp',
    #'vcpu.cpp',
    'vm.cpp',
)

kvm_internal_headers = files(
  'kvm.hpp',
  'types.hpp',
  'system.hpp',
  'vcpu.hpp',
  'vm.hpp',
)

install_headers(kvm_internal_headers, subdir: 'vmm/kvm/internal')
