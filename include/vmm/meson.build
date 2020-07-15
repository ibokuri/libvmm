subdir('utils')
subdir('kvm')

public_headers = include_directories('.', 'utils', 'kvm')

headers = [
    'kvm.hpp',
    'utils.hpp',
]

install_headers(headers, subdir: 'vmm')
