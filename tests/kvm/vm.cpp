#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include "vmm/kvm/kvm.hpp"

namespace kvm = vmm::kvm;

TEST_CASE("VM creation", "[api]") {
    kvm::system kvm;
    kvm::vm vm {kvm.vm()};
}

TEST_CASE("vcpu and memory slots", "[api]") {
    kvm::system kvm;
    kvm::vm vm {kvm.vm()};

    REQUIRE(vm.nr_vcpus() >= 4);
    REQUIRE(vm.max_vcpus() >= vm.nr_vcpus());
    REQUIRE(vm.nr_memslots() >= 32);
}

TEST_CASE("Invalid memory slot", "[api]") {
    vmm::kvm::system kvm;
    vmm::kvm::vm vm {kvm.vm()};
    kvm_userspace_memory_region mem_region {
        .slot = 0,
        .flags = 0,
        .guest_phys_addr = 0,
        .memory_size = 0,
        .userspace_addr = 0,
    };

    REQUIRE_THROWS(vm.user_memory_region(mem_region));
}

TEST_CASE("IRQ Chip (x86)", "[api]") {
    vmm::kvm::system kvm;
    vmm::kvm::vm vm {kvm.vm()};

    REQUIRE_NOTHROW(vm.check_extension(KVM_CAP_IRQCHIP));
    REQUIRE_NOTHROW(vm.irqchip());

    kvm_irqchip irqchip1 {
        .chip_id = KVM_IRQCHIP_PIC_MASTER,
        .chip = { .pic = {.irq_base = 10} }
    };
    kvm_irqchip irqchip2 {
        .chip_id = KVM_IRQCHIP_PIC_MASTER
    };

    REQUIRE_NOTHROW(vm.set_irqchip(&irqchip1));
    REQUIRE_NOTHROW(vm.get_irqchip(&irqchip2));

    REQUIRE(irqchip1.chip.pic.irq_base == irqchip2.chip.pic.irq_base);
}
