#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include "vmm/kvm/kvm.hpp"
#include <iterator>

namespace kvm = vmm::kvm;

TEST_CASE("KVM object creation", "[api]") {
    kvm::system kvm;
}

TEST_CASE("KVM object creation (via external fd)", "[api]") {
    auto fd {kvm::system::open()};
    kvm::system kvm{fd};
}

TEST_CASE("KVM object creation (via bad fd)", "[api]") {
    kvm::system kvm{999};

    REQUIRE_THROWS_WITH(kvm.vcpu_mmap_size(), "Bad file descriptor");
    REQUIRE_THROWS_WITH(kvm.supported_cpuids(), "Bad file descriptor");
    REQUIRE_THROWS_WITH(kvm.emulated_cpuids(), "Bad file descriptor");
    REQUIRE_THROWS_WITH(kvm.msr_index_list(), "Bad file descriptor");
    REQUIRE_THROWS_WITH(kvm.msr_feature_list(), "Bad file descriptor");
    REQUIRE_THROWS_WITH(kvm.vm(), "Bad file descriptor");
}

TEST_CASE("API version", "[api]") {
    kvm::system kvm;
    REQUIRE(kvm.api_version() == KVM_API_VERSION);
}

TEST_CASE("KVM mmap and IPA size", "[api]") {
    kvm::system kvm;
    auto ipa_limit {kvm.host_ipa_limit()};

    REQUIRE(kvm.vcpu_mmap_size() > 0);

    if (ipa_limit > 0)
        REQUIRE(ipa_limit >= 32);
    else
        REQUIRE(ipa_limit == 0);
}

TEST_CASE("Host-supported x86 cpuid features", "[api]") {
    kvm::system kvm;
    auto cpuids {kvm.supported_cpuids()};
    auto size {std::distance(cpuids.begin(), cpuids.end())};

    REQUIRE(size != 0);
    REQUIRE(size == cpuids.nent());
    REQUIRE(size <= MAX_CPUID_ENTRIES);
}

TEST_CASE("KVM-emulated x86 cpuid features", "[api]") {
    kvm::system kvm;
    auto cpuids {kvm.emulated_cpuids()};
    auto size  {std::distance(cpuids.begin(), cpuids.end())};

    REQUIRE(size != 0);
    REQUIRE(size == cpuids.nent());
    REQUIRE(size <= MAX_CPUID_ENTRIES);
}

TEST_CASE("Copying cpuid objects", "[api]") {
    kvm::system kvm;
    auto cpuids1 {kvm.supported_cpuids()};
    auto cpuids2 {cpuids1};
}

TEST_CASE("MSR index list", "[api]") {
    kvm::system kvm;
    auto msr_list {kvm.msr_index_list()};
    auto size {std::distance(msr_list.begin(), msr_list.end())};

    REQUIRE(size > 1);
    REQUIRE(size == msr_list.nmsrs());
}

TEST_CASE("MSR feature list", "[api]") {
    kvm::system kvm;
    auto msr_list {kvm.msr_feature_list()};
    auto size {std::distance(msr_list.begin(), msr_list.end())};

    REQUIRE(size > 1);
    REQUIRE(size == msr_list.nmsrs());
}

TEST_CASE("VM creation (with IPA size)", "[api]") {
    kvm::system kvm;

    if (kvm.check_extension(KVM_CAP_ARM_VM_IPA_SIZE)) {
            auto host_ipa_limit {kvm.host_ipa_limit()};

            // Test max value
            kvm.vm(host_ipa_limit);

            // Test invalid values
            REQUIRE_THROWS(kvm.vm(31));
            REQUIRE_THROWS(kvm.vm(host_ipa_limit + 1));
    }
    else {
        // Test default size
        REQUIRE_THROWS(kvm.vm(40));
    }
}
