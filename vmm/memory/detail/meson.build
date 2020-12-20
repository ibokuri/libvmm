memory_internal_headers = files(
  'guest.hpp',
  'address.hpp',
)

memory_internal_sources = files(
  'guest.cpp',
  #'address.cpp',
)

sources += memory_internal_sources

install_headers(memory_internal_headers, subdir: 'vmm/memory/detail')
