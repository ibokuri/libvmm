#define CATCH_CONFIG_MAIN

#include <iterator>
#include <catch2/catch.hpp>
#include "vmm/kvm/kvm.hpp"

TEST_CASE("KVM object creation", "[api]") {
    REQUIRE_NOTHROW(vmm::kvm::system{});
}

TEST_CASE("KVM object creation (via external fd)", "[api]") {
    REQUIRE_NOTHROW(vmm::kvm::system{vmm::kvm::system::open()});
}

TEST_CASE("KVM object creation (via bad fd)", "[api]") {
    auto kvm = vmm::kvm::system{999};

    REQUIRE_THROWS_AS(kvm.vcpu_mmap_size(), std::system_error);
    REQUIRE_THROWS_AS(kvm.supported_cpuids(), std::system_error);
    REQUIRE_THROWS_AS(kvm.emulated_cpuids(), std::system_error);
    REQUIRE_THROWS_AS(kvm.msr_index_list(), std::system_error);
    REQUIRE_THROWS_AS(kvm.msr_feature_list(), std::system_error);
    REQUIRE_THROWS_AS(kvm.vm(), std::system_error);
}

TEST_CASE("API version", "[api]") {
    auto kvm = vmm::kvm::system{};
    REQUIRE(kvm.api_version() == KVM_API_VERSION);
}

TEST_CASE("KVM mmap and IPA size", "[api]") {
    auto kvm = vmm::kvm::system{};
    auto ipa_limit = kvm.host_ipa_limit();

    REQUIRE(kvm.vcpu_mmap_size() > 0);

    if (ipa_limit > 0)
        REQUIRE(ipa_limit >= 32);
    else
        REQUIRE(ipa_limit == 0);
}

TEST_CASE("Host-supported x86 cpuid features", "[api]") {
    auto kvm = vmm::kvm::system{};
    auto cpuids = kvm.supported_cpuids();
    auto size = std::distance(cpuids.begin(), cpuids.end());

    REQUIRE(size != 0);
    REQUIRE(static_cast<std::size_t>(size) == cpuids.size());
    REQUIRE(size <= MAX_CPUID_ENTRIES);
}

TEST_CASE("KVM-emulated x86 cpuid features", "[api]") {
    auto kvm = vmm::kvm::system{};
    auto cpuids = kvm.emulated_cpuids();
    auto size = std::distance(cpuids.begin(), cpuids.end());

    REQUIRE(size != 0);
    REQUIRE(static_cast<std::size_t>(size) == cpuids.size());
    REQUIRE(size <= MAX_CPUID_ENTRIES);
}

TEST_CASE("Copying cpuid objects", "[api]") {
    auto kvm = vmm::kvm::system{};
    auto cpuids1 = kvm.supported_cpuids();
    auto cpuids2 = cpuids1;
}

TEST_CASE("MSR index list", "[api]") {
    auto kvm = vmm::kvm::system{};
    auto msr_list = kvm.msr_index_list();
    auto size = std::distance(msr_list.begin(), msr_list.end());

    REQUIRE(size > 1);
    REQUIRE(static_cast<std::size_t>(size) == msr_list.size());
}

TEST_CASE("MSR feature list", "[api]") {
    auto kvm = vmm::kvm::system{};
    auto msr_list = kvm.msr_feature_list();
    auto size = std::distance(msr_list.begin(), msr_list.end());

    REQUIRE(size > 1);
    REQUIRE(static_cast<std::size_t>(size) == msr_list.size());
}

TEST_CASE("VM creation (with IPA size)", "[api]") {
    auto kvm = vmm::kvm::system{};

    if (kvm.check_extension(KVM_CAP_ARM_VM_IPA_SIZE)) {
        auto host_ipa_limit = kvm.host_ipa_limit();

        // Test max value
        auto vm = kvm.vm(host_ipa_limit);

        // Test invalid values
        REQUIRE_THROWS_AS(kvm.vm(31), std::system_error);
        REQUIRE_THROWS_AS(kvm.vm(host_ipa_limit + 1), std::system_error);
    }
    else {
        // Test default size
        REQUIRE_THROWS_AS(kvm.vm(40), std::system_error);
    }
}
