#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include "vmm/kvm/kvm.hpp"

namespace kvm = vmm::kvm;

TEST_CASE("VM creation", "[api]") {
    kvm::system kvm;
    kvm::vm vm {kvm.vm()};
}

TEST_CASE("VM vcpu and memory slots", "[api]") {
    kvm::system kvm;
    kvm::vm vm {kvm.vm()};

    REQUIRE(vm.nr_vcpus() >= 4);
    REQUIRE(vm.max_vcpus() >= vm.nr_vcpus());
    REQUIRE(vm.nr_memslots() >= 32);
}
