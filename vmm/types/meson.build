subdir('detail')

type_sources = files(
  'eventfd.cpp',
  'file_descriptor.cpp'
)

type_headers = files(
  'eventfd.hpp',
  'file_descriptor.hpp'
)

sources += type_sources

install_headers(type_headers, subdir: 'vmm/types')
