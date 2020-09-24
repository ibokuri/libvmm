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
      if host_machine.cpu_family() == 'x86' or host_machine.cpu_family() == 'x86_64'
        test(n_test, executable(n_test,
                                f,
                                dependencies : [project_dep, test_dep],
                                install : false),
                                args : ['[all],[x86]'])
      elif host_machine.cpu_family() == 'arm'
        test(n_test, executable(n_test,
                                f,
                                dependencies : [project_dep, test_dep],
                                install : false),
                                args : ['[all],[arm]'])
      elif host_machine.cpu_family() == 'aarch64'
        test(n_test, executable(n_test,
                                f,
                                dependencies : [project_dep, test_dep],
                                install : false),
                                args : ['[all],[aarch64]'])
      elif host_machine.cpu_family() == 'mips'
        test(n_test, executable(n_test,
                                f,
                                dependencies : [project_dep, test_dep],
                                install : false),
                                args : ['[all],[mips]'])
      elif host_machine.cpu_family() == 'ppc' # or host_machine.cpu_family() == 'ppc64'
        test(n_test, executable(n_test,
                                f,
                                dependencies : [project_dep, test_dep],
                                install : false),
                                args : ['[all],[ppc]'])
      elif host_machine.cpu_family() == 's390'
        test(n_test, executable(n_test,
                                f,
                                dependencies : [project_dep, test_dep],
                                install : false),
                                args : ['[all],[s390]'])
      else
        error('MESON_SKIP_TEST: skipping test on this platform')
      endif
    endforeach
  endforeach
endif
