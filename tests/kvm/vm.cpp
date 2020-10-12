#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include "vmm/kvm/kvm.hpp"

TEST_CASE("VM creation", "[all]") {
    REQUIRE_NOTHROW(vmm::kvm::system{}.vm());
}

TEST_CASE("vcpu and memory slots", "[all]") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();

    REQUIRE(vm.num_vcpus() >= 4);
    REQUIRE(vm.max_vcpus() >= vm.num_vcpus());
    REQUIRE(vm.num_memslots() >= 32);
}

TEST_CASE("Invalid memory slot", "[all]") {
    auto kvm = vmm::kvm::system{};

    if (kvm.check_extension(KVM_CAP_USER_MEMORY) > 0) {
        auto vm = kvm.vm();
        auto mem_region = kvm_userspace_memory_region{
            .slot = 0,
            .flags = 0,
            .guest_phys_addr = 0,
            .memory_size = 0,
            .userspace_addr = 0,
        };

        REQUIRE_THROWS(vm.memslot(mem_region));
    }
}

TEST_CASE("IRQ Chip creation (x86)", "[.x86][.arm][.aarch64]") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();

    if (vm.check_extension(KVM_CAP_IRQCHIP) > 0) {
        REQUIRE_NOTHROW(vm.irqchip());
    }
}

TEST_CASE("IRQ Chip creation (s390)", "[.s390]") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();

    if (vm.check_extension(KVM_CAP_S390_IRQCHIP) > 0) {
        REQUIRE_NOTHROW(vm.irqchip());
    }
}

TEST_CASE("IRQ Chip (x86)", "[.x86]") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();

    if (vm.check_extension(KVM_CAP_IRQCHIP) > 0) {
        REQUIRE_NOTHROW(vm.irqchip());

        auto irqchip1 = kvm_irqchip{
            .chip_id = KVM_IRQCHIP_PIC_MASTER,
            .chip = { .pic = {.irq_base = 10} }
        };
        auto irqchip2 = kvm_irqchip{
            .chip_id = KVM_IRQCHIP_PIC_MASTER
        };

        REQUIRE_NOTHROW(vm.set_irqchip(irqchip1));
        REQUIRE_NOTHROW(vm.get_irqchip(irqchip2));

        REQUIRE(irqchip1.chip.pic.irq_base == irqchip2.chip.pic.irq_base);
    }
}

TEST_CASE("Clock", "[.x86]") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();

    if (vm.check_extension(KVM_CAP_ADJUST_CLOCK) > 0) {
        auto orig = kvm_clock_data{ vm.get_clock() };
        auto other = kvm_clock_data{ .clock = 10 };

        vm.set_clock(other);
        auto newtime = kvm_clock_data{ vm.get_clock() };

        REQUIRE(orig.clock > newtime.clock);
        REQUIRE(newtime.clock > other.clock);
    }
}

TEST_CASE("Bootstrap Processor (BSP)", "[.x86]") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();

    if (vm.check_extension(KVM_CAP_SET_BOOT_CPU_ID) > 0) {
        SECTION("No vcpu") {
            REQUIRE_NOTHROW(vm.set_bsp(0));
        }

        // TODO: Comment the logic behind the test. I think it was b/c you
        // can't set the BSP after a vcpu is already created.
        SECTION("Existing vcpu") {
            auto vcpu = vm.vcpu(0);
            REQUIRE_THROWS(vm.set_bsp(0));
        }
    }
}

TEST_CASE("Attach ioevent", "[all]") {
    using IoEventAddress = vmm::types::IoEventAddress;

    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();
    auto eventfd = vmm::types::EventFd{EFD_NONBLOCK};

    if (vm.check_extension(KVM_CAP_IOEVENTFD) > 0) {
        REQUIRE_NOTHROW(vm.attach_ioevent<IoEventAddress::Mmio>(eventfd, 0x1000));
        REQUIRE_NOTHROW(vm.attach_ioevent<IoEventAddress::Pio>(eventfd, 0xf4));
        REQUIRE_NOTHROW(vm.attach_ioevent<IoEventAddress::Pio>(eventfd, 0xc1, 0x7f));
        REQUIRE_NOTHROW(vm.attach_ioevent<IoEventAddress::Pio>(eventfd, 0xc2, 0x1337));
        REQUIRE_NOTHROW(vm.attach_ioevent<IoEventAddress::Pio>(eventfd, 0xc4, 0xdeadbeef));
        REQUIRE_NOTHROW(vm.attach_ioevent<IoEventAddress::Pio>(eventfd, 0xc8, 0xdeadbeefdeadbeef));
    }
}

TEST_CASE("Detach ioevent", "[all]") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();

    if (vm.check_extension(KVM_CAP_IOEVENTFD) > 0) {
        auto eventfd = vmm::types::EventFd{EFD_NONBLOCK};
        auto pio_addr = 0xf4;
        auto mmio_addr = 0x1000;

        REQUIRE_THROWS(vm.detach_ioevent<vmm::types::IoEventAddress::Pio>(eventfd, pio_addr));
        REQUIRE_THROWS(vm.detach_ioevent<vmm::types::IoEventAddress::Pio>(eventfd, mmio_addr));

        REQUIRE_NOTHROW(vm.attach_ioevent<vmm::types::IoEventAddress::Pio>(eventfd, pio_addr));
        REQUIRE_NOTHROW(vm.attach_ioevent<vmm::types::IoEventAddress::Pio>(eventfd, mmio_addr, 0x1337));
        REQUIRE_NOTHROW(vm.detach_ioevent<vmm::types::IoEventAddress::Pio>(eventfd, pio_addr));
        REQUIRE_NOTHROW(vm.detach_ioevent<vmm::types::IoEventAddress::Pio>(eventfd, mmio_addr, 0x1337));
    }
}

TEST_CASE("GSI routing (arm, aarch64, s390)", "[.arm][.arm64][.s390]") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();

    if (vm.check_extension(KVM_CAP_IRQ_ROUTING) > 0) {
        auto entry = kvm_irq_routing_entry{};
        auto routing_list = vmm::kvm::IrqRouting<1>{entry};
        REQUIRE_NOTHROW(vm.gsi_routing(routing_list));
    }
}

TEST_CASE("GSI routing (x86)", "[.x86]") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();

    if (vm.check_extension(KVM_CAP_IRQ_ROUTING) > 0) {
        auto table = vmm::kvm::IrqRouting<0>{};

        // No irqchip created yet
        REQUIRE_THROWS(vm.gsi_routing(table));

        vm.irqchip();
        REQUIRE_NOTHROW(vm.gsi_routing(table));
    }
}

TEST_CASE("IRQ Line (x86)", "[.x86]") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();

    REQUIRE_NOTHROW(vm.irqchip());
    REQUIRE_NOTHROW(vm.set_irq_line(4, true));
    REQUIRE_NOTHROW(vm.set_irq_line(4, false));
    REQUIRE_NOTHROW(vm.set_irq_line(4, true));
}

//TEST_CASE("IRQ Line (arm, arm64)", "[.arm][.arm64]") {
    //auto kvm = vmm::kvm::system{};
    //auto vm = kvm.vm();
    //auto vcpu = vm.vcpu(0);
    //
    // TODO: requires dummy GIC device
//}
