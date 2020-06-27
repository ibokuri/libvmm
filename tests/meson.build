test_deps = [
  dependency('catch2'),
]

tkvm = executable('kvm', 'kvm.cpp', include_directories : inc, dependencies : test_deps)
test('kvm test', tkvm)
