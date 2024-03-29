#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include "vmm/kvm/kvm.hpp"

TEST_CASE("VM creation") {
    REQUIRE_NOTHROW(vmm::kvm::System{}.vm());
}

TEST_CASE("vCPU creation") {
    auto kvm = vmm::kvm::System{};
    auto vm = kvm.vm();

    const auto kvm_max_vcpus = kvm.check_extension(KVM_CAP_MAX_VCPUS);
    const auto kvm_max_vcpu_id = kvm.check_extension(KVM_CAP_MAX_VCPU_ID);

    REQUIRE(kvm_max_vcpu_id >= kvm_max_vcpus);

    SECTION("Max # of vCPUs") {
        for (auto id = 0; id < kvm_max_vcpus; id++)
            REQUIRE_NOTHROW(vm.vcpu(id));

        REQUIRE_THROWS(vm.vcpu(kvm_max_vcpus));
    }

    if (kvm_max_vcpu_id > kvm_max_vcpus) {
        SECTION("Max IDs") {
            for (auto id = kvm_max_vcpu_id - kvm_max_vcpus; id < kvm_max_vcpu_id; id++)
                REQUIRE_NOTHROW(vm.vcpu(id));

            REQUIRE_THROWS(vm.vcpu(kvm_max_vcpu_id));
        }
    }
}

TEST_CASE("Empty memory region") {
    auto kvm = vmm::kvm::System{};
    auto vm = kvm.vm();
    auto mem_region = kvm_userspace_memory_region{};

    REQUIRE_THROWS(vm.set_memslot(mem_region));
}

TEST_CASE("vCPU and memory slot information") {
    auto kvm = vmm::kvm::System{};
    auto vm = kvm.vm();

    REQUIRE(vm.num_vcpus() >= 4);
    REQUIRE(vm.max_vcpus() >= vm.num_vcpus());
    REQUIRE(vm.num_memslots() >= 32);
}

TEST_CASE("IOEvent") {
    using EventFd = vmm::types::EventFd;
    using IoEventAddress = vmm::types::IoEventAddress;

    auto kvm = vmm::kvm::System{};
    auto vm = kvm.vm();
    auto eventfd = EventFd{EFD_NONBLOCK};

    SECTION("Attach") {
        if (vm.check_extension(KVM_CAP_IOEVENTFD) > 0) {
            REQUIRE_NOTHROW(vm.attach_ioevent<IoEventAddress::Mmio>(eventfd, 0x1000));
            REQUIRE_NOTHROW(vm.attach_ioevent<IoEventAddress::Pio>(eventfd, 0xf4));
            REQUIRE_NOTHROW(vm.attach_ioevent<IoEventAddress::Pio>(eventfd, 0xc1, 0x7f));
            REQUIRE_NOTHROW(vm.attach_ioevent<IoEventAddress::Pio>(eventfd, 0xc2, 0x1337));
            REQUIRE_NOTHROW(vm.attach_ioevent<IoEventAddress::Pio>(eventfd, 0xc4, 0xdead'beef));
            REQUIRE_NOTHROW(vm.attach_ioevent<IoEventAddress::Pio>(eventfd, 0xc8, 0xdead'beef'dead'beef));
        }
    }

    SECTION("Detach") {
        if (vm.check_extension(KVM_CAP_IOEVENTFD) > 0) {
            auto pio_addr = uint64_t{0xf4};
            auto mmio_addr = uint64_t{0x1000};

            REQUIRE_THROWS(vm.detach_ioevent<IoEventAddress::Pio>(eventfd, pio_addr));
            REQUIRE_THROWS(vm.detach_ioevent<IoEventAddress::Pio>(eventfd, mmio_addr));

            REQUIRE_NOTHROW(vm.attach_ioevent<IoEventAddress::Pio>(eventfd, pio_addr));
            REQUIRE_NOTHROW(vm.attach_ioevent<IoEventAddress::Pio>(eventfd, mmio_addr, 0x1337));
            REQUIRE_NOTHROW(vm.detach_ioevent<IoEventAddress::Pio>(eventfd, pio_addr));
            REQUIRE_NOTHROW(vm.detach_ioevent<IoEventAddress::Pio>(eventfd, mmio_addr, 0x1337));
        }
    }
}

#if defined(__i386__) || defined(__x86_64__) || \
    defined(__arm__)  || defined(__aarch64__)
TEST_CASE("IRQ Chip creation") {
    auto kvm = vmm::kvm::System{};
    auto vm = kvm.vm();

    if (vm.check_extension(KVM_CAP_IRQCHIP) > 0) {
        REQUIRE_NOTHROW(vm.irqchip());
    }
}

TEST_CASE("Fail MSI signal") {
    auto kvm = vmm::kvm::System{};
    auto vm = kvm.vm();
    auto msi = kvm_msi{};

    // This throws b/c MSI vectors aren't chosen from the HW side (VMM). The
    // guest OS (or anything that runs inside the VM) is supposed to allocate
    // the MSI vectors, and usually communicates back through PCI
    // configuration space. Sending a random MSI vector through signal_msi()
    // will always result in a failure.
    REQUIRE_THROWS(vm.signal_msi(msi));
}
#endif

#if defined(__i386__) || defined(__x86_64__)
// FIXME: In the kernel's KVM selftests, there is a FIXME for this test on
// aarch64 and s390x. On those platforms, KVM_RUN fails with ENOEXEC or EFAULT
// instead of successfully returning KVM_EXIT_INTERNAL_ERROR.
//
// Because of this, this test is currently x86 only. Once the fix is made in
// the kernel, the test should be made available for all platforms.
TEST_CASE("No memory region") {
    const auto N = 64;
    const auto VCPU_ID = 0;

    auto kvm = vmm::kvm::System{};
    auto vm = kvm.vm();
    auto vcpu = vm.vcpu(VCPU_ID);

    REQUIRE_NOTHROW(vm.set_num_mmu_pages(N));
    REQUIRE(vm.num_mmu_pages() == N);
    REQUIRE(static_cast<uint32_t>(vcpu.run()) == KVM_EXIT_INTERNAL_ERROR);
}

TEST_CASE("IRQ chip") {
    auto kvm = vmm::kvm::System{};
    auto vm = kvm.vm();

    if (vm.check_extension(KVM_CAP_IRQCHIP) > 0) {
        vm.irqchip();

        auto irqchip1 = kvm_irqchip{KVM_IRQCHIP_PIC_MASTER};
        irqchip1.chip.pic.irq_base = 10;

        auto irqchip2 = kvm_irqchip{};
        irqchip2.chip_id = KVM_IRQCHIP_PIC_MASTER;

        REQUIRE_NOTHROW(vm.set_irqchip(irqchip1));
        REQUIRE_NOTHROW(vm.get_irqchip(irqchip2));

        REQUIRE(irqchip1.chip.pic.irq_base == irqchip2.chip.pic.irq_base);
    }
}

TEST_CASE("Clock") {
    auto kvm = vmm::kvm::System{};
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

TEST_CASE("Bootstrap processor (BSP)") {
    auto kvm = vmm::kvm::System{};
    auto vm = kvm.vm();

    if (vm.check_extension(KVM_CAP_SET_BOOT_CPU_ID) > 0) {
        SECTION("No vCPU") {
            REQUIRE_NOTHROW(vm.set_bsp(0));
        }

        SECTION("vCPU") {
            auto vcpu = vm.vcpu(0);
            REQUIRE_THROWS(vm.set_bsp(0));
        }
    }
}

TEST_CASE("GSI routing") {
    auto kvm = vmm::kvm::System{};
    auto vm = kvm.vm();

    if (vm.check_extension(KVM_CAP_IRQ_ROUTING) > 0) {
        auto table = vmm::kvm::IrqRouting<0>{};

        SECTION("No IRQ chip") {
            REQUIRE_THROWS(vm.gsi_routing(table));
        }

        SECTION("IRQ chip") {
            vm.irqchip();
            REQUIRE_NOTHROW(vm.gsi_routing(table));
        }
    }
}

TEST_CASE("IRQ line") {
    auto kvm = vmm::kvm::System{};
    auto vm = kvm.vm();

    REQUIRE_NOTHROW(vm.irqchip());
    REQUIRE_NOTHROW(vm.set_irq_line(4, vmm::kvm::IrqLevel::Active));
    REQUIRE_NOTHROW(vm.set_irq_line(4, vmm::kvm::IrqLevel::Inactive));
    REQUIRE_NOTHROW(vm.set_irq_line(4, vmm::kvm::IrqLevel::Active));
}

TEST_CASE("IRQ file descriptor") {
    auto kvm = vmm::kvm::System{};
    auto vm = kvm.vm();
    auto eventfd1 = vmm::types::EventFd{EFD_NONBLOCK};
    auto eventfd2 = vmm::types::EventFd{EFD_NONBLOCK};
    auto eventfd3 = vmm::types::EventFd{EFD_NONBLOCK};

    REQUIRE_NOTHROW(vm.irqchip());

    REQUIRE_NOTHROW(vm.register_irqfd(eventfd1, 4));
    REQUIRE_NOTHROW(vm.register_irqfd(eventfd2, 8));
    REQUIRE_NOTHROW(vm.register_irqfd(eventfd3, 4));

    // NOTE: On x86_64, this fails as the event fd was already matched with
    // a GSI.
    REQUIRE_THROWS(vm.register_irqfd(eventfd3, 4));
    REQUIRE_THROWS(vm.register_irqfd(eventfd3, 5));

    // NOTE: KVM doesn't report the 2nd, duplicate unregister as an error
    REQUIRE_NOTHROW(vm.unregister_irqfd(eventfd2, 8));
    REQUIRE_NOTHROW(vm.unregister_irqfd(eventfd2, 8));

    // NOTE: KVM doesn't report unregisters with different levels as errors
    REQUIRE_NOTHROW(vm.unregister_irqfd(eventfd3, 5));
}

TEST_CASE("TSS address") {
    auto kvm = vmm::kvm::System{};
    auto vm = kvm.vm();

    REQUIRE_NOTHROW(vm.set_tss_address(0xfffb'd000));
}

TEST_CASE("PIT2") {
    auto kvm = vmm::kvm::System{};
    auto vm = kvm.vm();

    SECTION("Creation/get") {
        // NOTE: For some reason, create_pit2() doesn't throw even when irqchip
        // support isn't enabled via irqchip().
        REQUIRE_NOTHROW(vm.irqchip());
        REQUIRE_NOTHROW(vm.create_pit2());
        REQUIRE_NOTHROW(vm.pit2());
    }

    SECTION("Set") {
        REQUIRE_NOTHROW(vm.irqchip());
        REQUIRE_NOTHROW(vm.create_pit2());
        auto pit2 = vm.pit2();
        REQUIRE_NOTHROW(vm.set_pit2(pit2));
        auto other = vm.pit2();

        // Overwrite load times as they may differ
        other.channels[0].count_load_time = pit2.channels[0].count_load_time;
        other.channels[1].count_load_time = pit2.channels[1].count_load_time;
        other.channels[2].count_load_time = pit2.channels[2].count_load_time;

        for (std::size_t i = 0; i < 3; i++) {
            REQUIRE(pit2.channels[i].count == other.channels[i].count);
            REQUIRE(pit2.channels[i].latched_count == other.channels[i].latched_count);
            REQUIRE(pit2.channels[i].count_latched == other.channels[i].count_latched);
            REQUIRE(pit2.channels[i].status_latched == other.channels[i].status_latched);
            REQUIRE(pit2.channels[i].status == other.channels[i].status);
            REQUIRE(pit2.channels[i].read_state == other.channels[i].read_state);
            REQUIRE(pit2.channels[i].write_state == other.channels[i].write_state);
            REQUIRE(pit2.channels[i].write_latch == other.channels[i].write_latch);
            REQUIRE(pit2.channels[i].rw_mode == other.channels[i].rw_mode);
            REQUIRE(pit2.channels[i].mode == other.channels[i].mode);
            REQUIRE(pit2.channels[i].bcd == other.channels[i].bcd);
            REQUIRE(pit2.channels[i].gate == other.channels[i].gate);
        }
    }
}
#endif

#if defined(__arm__) || defined(__aarch64__)
TEST_CASE("IRQ chip") {
    auto kvm = vmm::kvm::System{};
    auto vm = kvm.vm();

    if (vm.check_extension(KVM_CAP_IRQCHIP)) {
        REQUIRE_NOTHROW(vm.device(KVM_DEV_TYPE_ARM_VGIC_V2, KVM_CREATE_DEVICE_TEST));
        REQUIRE_NOTHROW(vm.irqchip());
    }
}

TEST_CASE("GSI routing") {
    auto kvm = vmm::kvm::System{};
    auto vm = kvm.vm();

    if (vm.check_extension(KVM_CAP_IRQ_ROUTING) > 0) {
        auto entry = kvm_irq_routing_entry{};
        auto routing_list = vmm::kvm::IrqRouting<1>{entry};
        REQUIRE_NOTHROW(vm.gsi_routing(routing_list));
    }
}
#endif

#if defined(__arm__) || defined(__aarch64__)
TEST_CASE("Preferred target") {
    auto kvm = vmm::kvm::System{};
    auto vm = kvm.vm();
    REQUIRE_NOTHROW(vm.preferred_target());
}
#endif

#if defined(__aarch64__)
TEST_CASE("IRQ line") {
    auto kvm = vmm::kvm::System{};
    auto vm = kvm.vm();
    auto vcpu = vm.vcpu(0);
    auto vgic = vm.device(KVM_DEV_TYPE_ARM_VGIC_V3);

    // Set supported # of IRQs
    auto attributes = kvm_device_attr {
        0, // flags
        KVM_DEV_ARM_VGIC_GRP_NR_IRQS, // group
        0, // attr
        128, // addr
    };

    REQUIRE_NOTHROW(vgic.set_attr(attributes));

    // Request vGIC initialization
    attributes = kvm_device_attr {
        0, // flags
        KVM_DEV_ARM_VGIC_GRP_CTRL, // group
        KVM_DEV_ARM_VGIC_CTRL_INIT, // attr
        128, // addr
    };

    REQUIRE_NOTHROW(vgic.set_attr(attributes));

    // On arm/aarch64, irq field is interpreted like so:
    //
    //   bits:  | 31 ... 24 | 23  ... 16 | 15    ...    0 |
    //   field: | irq_type  | vcpu_index |     irq_id     |
    //
    // The irq_type field has the following values:
    //
    //   - irq_type[0]: out-of-kernel GIC: irq_id 0 is IRQ, irq_id 1 is FIQ
    //   - irq_type[1]: in-kernel GIC: SPI, irq_id between 32 and 1019 (incl.) (the vcpu_index field is ignored)
    //   - irq_type[2]: in-kernel GIC: PPI, irq_id between 16 and 31 (incl.)

    // Case 1: irq_type = 1, irq_id = 32 (decimal)
    REQUIRE_NOTHROW(vm.set_irq_line(0x01'00'0020, vmm::kvm::IrqLevel::Active));
    REQUIRE_NOTHROW(vm.set_irq_line(0x01'00'0020, vmm::kvm::IrqLevel::Inactive));
    REQUIRE_NOTHROW(vm.set_irq_line(0x01'00'0020, vmm::kvm::IrqLevel::Active));

    // Case 2: irq_type = 2, vcpu_index = 0, irq_id = 16 (decimal)
    REQUIRE_NOTHROW(vm.set_irq_line(0x02'00'0010, vmm::kvm::IrqLevel::Active));
    REQUIRE_NOTHROW(vm.set_irq_line(0x02'00'0010, vmm::kvm::IrqLevel::Inactive));
    REQUIRE_NOTHROW(vm.set_irq_line(0x02'00'0010, vmm::kvm::IrqLevel::Active));
}

TEST_CASE("IRQ file descriptor") {
    auto kvm = vmm::kvm::System{};
    auto vm = kvm.vm();
    auto vgic = vm.device(KVM_DEV_TYPE_ARM_VGIC_V3);
    auto eventfd1 = vmm::types::EventFd{EFD_NONBLOCK};
    auto eventfd2 = vmm::types::EventFd{EFD_NONBLOCK};
    auto eventfd3 = vmm::types::EventFd{EFD_NONBLOCK};

    // Set supported # of IRQs
    auto attributes = kvm_device_attr {
        0, // flags
        KVM_DEV_ARM_VGIC_GRP_NR_IRQS, // group
        0, // attr
        128, // addr
    };

    REQUIRE_NOTHROW(vgic.set_attr(attributes));

    // Request vGIC initialization
    attributes = kvm_device_attr {
        0, // flags
        KVM_DEV_ARM_VGIC_GRP_CTRL, // group
        KVM_DEV_ARM_VGIC_CTRL_INIT, // attr
        128, // addr
    };

    REQUIRE_NOTHROW(vgic.set_attr(attributes));

    REQUIRE_NOTHROW(vm.register_irqfd(eventfd1, 4));
    REQUIRE_NOTHROW(vm.register_irqfd(eventfd2, 8));
    REQUIRE_NOTHROW(vm.register_irqfd(eventfd3, 4));

    // NOTE: On aarch64, duplicate registrations fail b/c setting up the
    // interrupt controller is mandatory before any IRQ registration.
    REQUIRE_THROWS(vm.register_irqfd(eventfd3, 4));
    REQUIRE_THROWS(vm.register_irqfd(eventfd3, 5));

    // NOTE: KVM doesn't report the 2nd, duplicate unregister as an error
    REQUIRE_NOTHROW(vm.unregister_irqfd(eventfd2, 8));
    REQUIRE_NOTHROW(vm.unregister_irqfd(eventfd2, 8));

    // NOTE: KVM doesn't report unregisters with different levels as errors
    REQUIRE_NOTHROW(vm.unregister_irqfd(eventfd3, 5));
}
#endif
