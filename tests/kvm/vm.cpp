#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include "vmm/kvm/kvm.hpp"

TEST_CASE("VM creation") {
    REQUIRE_NOTHROW(vmm::kvm::system{}.vm());
}

TEST_CASE("Querying vCPU and memory slot information") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();

    REQUIRE(vm.num_vcpus() >= 4);
    REQUIRE(vm.max_vcpus() >= vm.num_vcpus());
    REQUIRE(vm.num_memslots() >= 32);
}

TEST_CASE("Set invalid memory slot") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();
    auto mem_region = kvm_userspace_memory_region{0};

    REQUIRE_THROWS(vm.memslot(mem_region));
}

// TODO: TEST_CASE("vCPU creation");
// TODO: TEST_CASE("Device creation");

TEST_CASE("Attach ioevent") {
    using EventFd = vmm::types::EventFd;
    using IoEventAddress = vmm::types::IoEventAddress;

    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();
    auto eventfd = EventFd{EFD_NONBLOCK};

    if (vm.check_extension(KVM_CAP_IOEVENTFD) > 0) {
        REQUIRE_NOTHROW(vm.attach_ioevent<IoEventAddress::Mmio>(eventfd, 0x1000));
        REQUIRE_NOTHROW(vm.attach_ioevent<IoEventAddress::Pio>(eventfd, 0xf4));
        REQUIRE_NOTHROW(vm.attach_ioevent<IoEventAddress::Pio>(eventfd, 0xc1, 0x7f));
        REQUIRE_NOTHROW(vm.attach_ioevent<IoEventAddress::Pio>(eventfd, 0xc2, 0x1337));
        REQUIRE_NOTHROW(vm.attach_ioevent<IoEventAddress::Pio>(eventfd, 0xc4, 0xdead'beef));
        REQUIRE_NOTHROW(vm.attach_ioevent<IoEventAddress::Pio>(eventfd, 0xc8, 0xdead'beef'dead'beef));
    }
}

TEST_CASE("Detach ioevent") {
    using EventFd = vmm::types::EventFd;
    using IoEventAddress = vmm::types::IoEventAddress;

    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();

    if (vm.check_extension(KVM_CAP_IOEVENTFD) > 0) {
        auto eventfd = EventFd{EFD_NONBLOCK};
        auto pio_addr = 0xf4;
        auto mmio_addr = 0x1000;

        REQUIRE_THROWS(vm.detach_ioevent<IoEventAddress::Pio>(eventfd, pio_addr));
        REQUIRE_THROWS(vm.detach_ioevent<IoEventAddress::Pio>(eventfd, mmio_addr));

        REQUIRE_NOTHROW(vm.attach_ioevent<IoEventAddress::Pio>(eventfd, pio_addr));
        REQUIRE_NOTHROW(vm.attach_ioevent<IoEventAddress::Pio>(eventfd, mmio_addr, 0x1337));
        REQUIRE_NOTHROW(vm.detach_ioevent<IoEventAddress::Pio>(eventfd, pio_addr));
        REQUIRE_NOTHROW(vm.detach_ioevent<IoEventAddress::Pio>(eventfd, mmio_addr, 0x1337));
    }
}

#if defined(__i386__) || defined(__x86_64__) || \
    defined(__arm__)  || defined(__arch64__)
TEST_CASE("IRQ Chip creation") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();

    if (vm.check_extension(KVM_CAP_IRQCHIP) > 0) {
        REQUIRE_NOTHROW(vm.irqchip());
    }
}
#endif

#if defined(__i386__) || defined(__x86_64__)
TEST_CASE("IRQ Chip") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();

    if (vm.check_extension(KVM_CAP_IRQCHIP) > 0) {
        REQUIRE_NOTHROW(vm.irqchip());

        auto irqchip1 = kvm_irqchip{};
        irqchip1.chip_id = KVM_IRQCHIP_PIC_MASTER;
        irqchip1.chip.pic.irq_base = 10;

        auto irqchip2 = kvm_irqchip{};
        irqchip2.chip_id = KVM_IRQCHIP_PIC_MASTER;

        REQUIRE_NOTHROW(vm.set_irqchip(irqchip1));
        REQUIRE_NOTHROW(vm.get_irqchip(irqchip2));

        REQUIRE(irqchip1.chip.pic.irq_base == irqchip2.chip.pic.irq_base);
    }
}

TEST_CASE("Clock") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();

    if (vm.check_extension(KVM_CAP_ADJUST_CLOCK) > 0) {
        auto orig = vm.get_clock();
        auto other = kvm_clock_data{10};

        vm.set_clock(other);
        auto newtime = vm.get_clock();

        REQUIRE(orig.clock > newtime.clock);
        REQUIRE(newtime.clock > other.clock);
    }
}

TEST_CASE("Bootstrap Processor (BSP)") {
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

TEST_CASE("GSI routing") {
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

TEST_CASE("IRQ Line") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();

    REQUIRE_NOTHROW(vm.irqchip());
    REQUIRE_NOTHROW(vm.set_irq_line(4, true));
    REQUIRE_NOTHROW(vm.set_irq_line(4, false));
    REQUIRE_NOTHROW(vm.set_irq_line(4, true));
}

TEST_CASE("TSS address") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();

    REQUIRE_NOTHROW(vm.set_tss_address(0xfffb'd000));
}
#endif

#if defined(__s390__)
TEST_CASE("IRQ Chip creation") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();

    if (vm.check_extension(KVM_CAP_S390_IRQCHIP) > 0) {
        REQUIRE_NOTHROW(vm.irqchip());
    }
}

TEST_CASE("IRQ File Descriptor") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();
    auto eventfd1 = vmm::types::EventFd{EFD_NONBLOCK};
    auto eventfd2 = vmm::types::EventFd{EFD_NONBLOCK};
    auto eventfd3 = vmm::types::EventFd{EFD_NONBLOCK};

    REQUIRE_NOTHROW(vm.irqchip());

    REQUIRE_NOTHROW(vm.register_irqfd(eventfd1, 4));
    REQUIRE_NOTHROW(vm.register_irqfd(eventfd2, 8));
    REQUIRE_NOTHROW(vm.register_irqfd(eventfd3, 4));

    // Duplicate registrations
    REQUIRE_THROWS(vm.register_irqfd(eventfd3, 4));

    // NOTE: KVM doesn't report duplicate unregisters as errors
    REQUIRE_NOTHROW(vm.unregister_irqfd(eventfd2, 8));
    REQUIRE_NOTHROW(vm.unregister_irqfd(eventfd2, 8));

    // NOTE: KVM doesn't report unregisters with different levels as errors
    REQUIRE_NOTHROW(vm.unregister_irqfd(eventfd3, 5));
}

#endif

#if defined(__arm__) || defined(__aarch64__) || defined(__s390__)
TEST_CASE("GSI routing") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();

    if (vm.check_extension(KVM_CAP_IRQ_ROUTING) > 0) {
        auto entry = kvm_irq_routing_entry{};
        auto routing_list = vmm::kvm::IrqRouting<1>{entry};
        REQUIRE_NOTHROW(vm.gsi_routing(routing_list));
    }
}
#endif

#if defined(__arm__) || defined(__aarch64__)
//TEST_CASE("IRQ line") {
    //auto kvm = vmm::kvm::system{};
    //auto vm = kvm.vm();
    //auto vcpu = vm.vcpu(0);
    //
    // TODO: requires dummy GIC device
//}

TEST_CASE("Preferred target") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();
    REQUIRE_NOTHROW(vm.preferred_target());
}
#endif

//#if defined(__aarch64__)
//TEST_CASE("IRQ File Descriptor") {
    //auto kvm = vmm::kvm::system{};
    //auto vm = kvm.vm();
    //auto eventfd1 = vmm::types::EventFd{EFD_NONBLOCK};
    //auto eventfd2 = vmm::types::EventFd{EFD_NONBLOCK};
    //auto eventfd3 = vmm::types::EventFd{EFD_NONBLOCK};

    //// TODO: Create vGIC device, set supported # of IRQs, and request
    ////       initialization of the vGIC.

    //REQUIRE_NOTHROW(vm.register_irqfd(eventfd1, 4));
    //REQUIRE_NOTHROW(vm.register_irqfd(eventfd2, 8));
    //REQUIRE_NOTHROW(vm.register_irqfd(eventfd3, 4));

    //// Duplicate registrations
    //REQUIRE_THROWS(vm.register_irqfd(eventfd3, 4));

    //// NOTE: KVM doesn't report duplicate unregisters as errors
    //REQUIRE_NOTHROW(vm.unregister_irqfd(eventfd2, 8));
    //REQUIRE_NOTHROW(vm.unregister_irqfd(eventfd2, 8));

    //// NOTE: KVM doesn't report unregisters with different levels as errors
    //REQUIRE_NOTHROW(vm.unregister_irqfd(eventfd3, 5));
//}
//#endif
