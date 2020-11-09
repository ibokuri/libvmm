# General
project(
  'libvmm',
  'cpp',
  version : '0.1.0',
  license : 'MIT',
  default_options : [
  'cpp_std=c++17',
  'warning_level=3',
  'werror=true',
  ],
)

warnings = [
  '-Wcast-align', # if there are potential performance problem casts
  '-Wconversion', # if there are type conversions that may lose data
  '-Wno-missing-field-initializers', # disable missing-field-initializers warnings
  '-Wsign-conversion', # sign conversions
  '-Wshadow', # if a variable declaration shadows one from a parent context
  '-Wunused', # If anything is unused
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

# Target
sources = []

subdir('vmm')

target = shared_library(
  'vmm',
  sources,
  install: true,
  include_directories : public_headers,
)

# Project
project_description = 'Modular VMM and hypervisor components'
pkgc = import('pkgconfig')

pkgc.generate(
  name : meson.project_name(),
  version : meson.project_version(),
  description : project_description,
  subdirs : meson.project_name(),
  libraries : target,
)

project_dep = declare_dependency(
  include_directories: public_headers,
  link_with : target
)

# Unit Tests
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
