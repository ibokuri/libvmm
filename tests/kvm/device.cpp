#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include "vmm/kvm/kvm.hpp"

#if defined(__i386__) || defined(__x86_64__)
// NOTE: For some unknown reason, making 2
//       `vm.device(KVM_DEV_TYPE_VFIO, KVM_CREATE_DEVICE_TEST)` calls (and only
//       binding the second one to a variable) causes `has_attr()` and co to
//       fail with "Invalid argument" instead of "Inappropriate ioctl." This is
//       true even if the calls were made in separate test cases... Because of
//       this, we can't exactly test just device creation.
TEST_CASE("Fake device attributes") {
    auto kvm = vmm::kvm::system{};
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
