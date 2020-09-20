subdir('detail')

type_sources = files(
  'event.cpp',
  'file_descriptor.cpp'
)

type_headers = files(
  'event.hpp',
  'file_descriptor.hpp'
)

sources += type_sources

install_headers(type_headers, subdir: 'vmm/types')
