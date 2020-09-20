#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include "vmm/kvm/kvm.hpp"

TEST_CASE("VM creation", "[api]") {
    REQUIRE_NOTHROW(vmm::kvm::system{}.vm());
}

TEST_CASE("vcpu and memory slots", "[api]") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();

    REQUIRE(vm.num_vcpus() >= 4);
    REQUIRE(vm.max_vcpus() >= vm.num_vcpus());
    REQUIRE(vm.num_memslots() >= 32);
}

TEST_CASE("Invalid memory slot", "[api]") {
    auto kvm = vmm::kvm::system{};
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

TEST_CASE("IRQ Chip (x86)", "[api]") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();

    REQUIRE(vm.check_extension(KVM_CAP_IRQCHIP) > 0);
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

TEST_CASE("Clock", "[api]") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();
    auto other = kvm_clock_data{ .clock = 10 };

    REQUIRE(vm.check_extension(KVM_CAP_ADJUST_CLOCK) > 0);

    auto orig = kvm_clock_data{ vm.get_clock() };
    vm.set_clock(other);
    auto newtime = kvm_clock_data{ vm.get_clock() };

    REQUIRE(orig.clock > newtime.clock);
    REQUIRE(newtime.clock > other.clock);
}

TEST_CASE("Bootstrap Processor (BSP)", "[api]") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();

    REQUIRE(vm.check_extension(KVM_CAP_SET_BOOT_CPU_ID) > 0);

    REQUIRE_NOTHROW(vm.set_bsp(0));
    auto vcpu = vm.vcpu(0);
    REQUIRE_THROWS(vm.set_bsp(0));
}

TEST_CASE("Attach ioevent", "[api]") {
    using IoEventAddress = vmm::types::IoEventAddress;

    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();
    auto eventfd = vmm::types::EventFd{EFD_NONBLOCK};

    REQUIRE(vm.check_extension(KVM_CAP_IOEVENTFD) > 0);

    REQUIRE_NOTHROW(vm.attach_ioevent<IoEventAddress::Mmio>(eventfd, 0x1000));
    REQUIRE_NOTHROW(vm.attach_ioevent<IoEventAddress::Pio>(eventfd, 0xf4));
    REQUIRE_NOTHROW(vm.attach_ioevent<IoEventAddress::Pio>(eventfd, 0xc1, 0x7f));
    REQUIRE_NOTHROW(vm.attach_ioevent<IoEventAddress::Pio>(eventfd, 0xc2, 0x1337));
    REQUIRE_NOTHROW(vm.attach_ioevent<IoEventAddress::Pio>(eventfd, 0xc4, 0xdeadbeef));
    REQUIRE_NOTHROW(vm.attach_ioevent<IoEventAddress::Pio>(eventfd, 0xc8, 0xdeadbeefdeadbeef));
}

TEST_CASE("Detach ioevent", "[api]") {
    using IoEventAddress = vmm::types::IoEventAddress;

    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();
    auto eventfd = vmm::types::EventFd{EFD_NONBLOCK};
    auto pio_addr = 0xf4;
    auto mmio_addr = 0x1000;

    REQUIRE(vm.check_extension(KVM_CAP_IOEVENTFD) > 0);

    REQUIRE_THROWS(vm.detach_ioevent<IoEventAddress::Pio>(eventfd, pio_addr));
    REQUIRE_THROWS(vm.detach_ioevent<IoEventAddress::Pio>(eventfd, mmio_addr));

    REQUIRE_NOTHROW(vm.attach_ioevent<IoEventAddress::Pio>(eventfd, pio_addr));
    REQUIRE_NOTHROW(vm.attach_ioevent<IoEventAddress::Pio>(eventfd, mmio_addr, 0x1337));
    REQUIRE_NOTHROW(vm.detach_ioevent<IoEventAddress::Pio>(eventfd, pio_addr));
    REQUIRE_NOTHROW(vm.detach_ioevent<IoEventAddress::Pio>(eventfd, mmio_addr, 0x1337));
}
