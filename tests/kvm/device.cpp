#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include "vmm/kvm/kvm.hpp"

#if defined(__i386__) || defined(__x86_64__)
// FIXME: For some reason, making 2
//        `vm.device(KVM_DEV_TYPE_VFIO, KVM_CREATE_DEVICE_TEST)` calls (and only
//        binding the second one to a variable) causes `has_attr()` and co to
//        fail with "Invalid argument" instead of "Inappropriate ioctl." This is
//        true even if the calls were made in separate test cases... Because of
//        this, we can't exactly test just device creation.
TEST_CASE("Device creation") {
    auto kvm = vmm::kvm::System{};
    auto vm = kvm.vm();

    // Fails b/c there's no VGIC on x86_64.
    REQUIRE_THROWS(vm.device(KVM_DEV_TYPE_ARM_VGIC_V3, KVM_CREATE_DEVICE_TEST));

    // NOTE: Creating a real device would make our CI dependent on
    //       host-specific settings (e.g., having /dev/vfio). So, this is just
    //       a test device which we expect to fail.
    auto device = vm.device(KVM_DEV_TYPE_VFIO, KVM_CREATE_DEVICE_TEST);

    auto attr = kvm_device_attr {
        0,
        KVM_DEV_VFIO_GROUP,
        KVM_DEV_VFIO_GROUP_ADD,
        0x0,
    };

    REQUIRE_THROWS_WITH(device.has_attr(attr), "Inappropriate ioctl for device");
    REQUIRE_THROWS_WITH(device.get_attr(attr), "Inappropriate ioctl for device");
    REQUIRE_THROWS_WITH(device.set_attr(attr), "Inappropriate ioctl for device");
}
#endif

#if defined(__aarch64__)
// Helper function for the aarch64 test case "Device creation".
//
// The test case tries to create a vGIC2 device if creating a vGIC3 device
// failed. To avoid duplicating logic in both try/catch sections, we call out
// to this function.
auto test_device_creation(vmm::kvm::device&& device) {
    // set attribute which don't apply to VGIC (i.e., expected to fail)
    auto attr = kvm_device_attr {
        0,
        KVM_DEV_VFIO_GROUP,
        KVM_DEV_VFIO_GROUP_ADD,
        0,
    };

    REQUIRE_THROWS(device.has_attr(attr));

    // set some attributes
    const auto irqs = uint32_t{128};
    attr.group = KVM_DEV_ARM_VGIC_GRP_NR_IRQS;
    attr.addr = reinterpret_cast<uintptr_t>(&irqs);

    REQUIRE_NOTHROW(device.set_attr(attr));

    attr.group = KVM_DEV_ARM_VGIC_GRP_CTRL;
    attr.attr = KVM_DEV_ARM_VGIC_CTRL_INIT;
    attr.addr = 0;

    REQUIRE_NOTHROW(device.has_attr(attr));
    REQUIRE_NOTHROW(device.set_attr(attr));

    // extract max supported # of IRQs
    auto data = uint32_t{};

    attr.group = KVM_DEV_ARM_VGIC_GRP_NR_IRQS;
    attr.attr = 0;
    attr.addr = static_cast<uint64_t>(data); // bad address

    REQUIRE_THROWS_WITH(device.get_attr(attr), "Bad address");

    attr.addr = reinterpret_cast<uintptr_t>(&data);
    REQUIRE_NOTHROW(device.get_attr(attr));

    REQUIRE(data == 128);
}

TEST_CASE("Device creation") {
    auto kvm = vmm::kvm::System{};
    auto vm = kvm.vm();

    // Fails on aarch64 since it doesn't use MPIC, it uses the VGIC.
    REQUIRE_THROWS(vm.device(KVM_DEV_TYPE_FSL_MPIC_20, KVM_CREATE_DEVICE_TEST));

    try {
        auto device = vm.device(KVM_DEV_TYPE_ARM_VGIC_V3);
        test_device_creation(std::move(device));
    } catch (std::system_error& err) {
        auto device = vm.device(KVM_DEV_TYPE_ARM_VGIC_V2);
        test_device_creation(std::move(device));
    }
}
#endif
