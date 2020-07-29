#
# General
#
project(
  'libvmm',
  'cpp',
  version : '0.1.0',
  license : 'MIT',
  default_options : [
  'cpp_std=c++17',
  'werror=true',
  ],
)

project_description = 'Modular VMM and hypervisor components'
sources = []

warnings = [
  # extra
  '-Wduplicated-cond',
  '-Wrestrict',
  '-Wnull-dereference',
  '-Wundef',
  '-Wcast-align=strict',
  '-Wlogical-op',

  # clang
  '-Wno-missing-braces',
]

cc = meson.get_compiler('cpp')

add_project_arguments(
    cc.get_supported_arguments(warnings),
    language: 'cpp',
)

add_project_link_arguments(
  '-lstdc++fs',
  language: 'cpp',
)


#
# Target
#
subdir('vmm')

target = shared_library(
  'vmm',
  sources,
  install: true,
  include_directories : public_headers,
)


#
# Project
#
pkgc = import('pkgconfig')
pkgc.generate(
  name : meson.project_name(),
  version : meson.project_version(),
  description : project_description,
  subdirs : meson.project_name(),
  libraries : target,
)

# Make the library usable as a dependency Meson subproject
project_dep = declare_dependency(
  include_directories: public_headers,
  link_with : target
)


#
# Unit Tests
#
if not meson.is_subproject()
  test_suites = {}

  subdir('tests')

  foreach n_suite, s : test_suites
    foreach n_test, f : s
      test(n_test, executable(n_test,
                              f,
                              dependencies : [project_dep, test_dep],
                              install : false))
    endforeach
  endforeach
endif
