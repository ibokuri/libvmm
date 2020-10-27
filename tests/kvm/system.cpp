#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include "vmm/kvm/kvm.hpp"

TEST_CASE("KVM system creation (fd)") {
    SECTION("Normal") {
        REQUIRE_NOTHROW(vmm::kvm::system{});
    }

    SECTION("Good file descriptor") {
        REQUIRE_NOTHROW(vmm::kvm::system{vmm::kvm::system::open()});
    }

    SECTION("Bad file descriptor") {
        auto kvm = vmm::kvm::system{999};

        REQUIRE_THROWS(kvm.api_version());
        REQUIRE_THROWS(kvm.vm());
        REQUIRE_THROWS(kvm.check_extension(KVM_CAP_EXT_CPUID));
        REQUIRE_THROWS(kvm.vcpu_mmap_size());
    }
}

TEST_CASE("API version") {
    auto kvm = vmm::kvm::system{};
    REQUIRE(kvm.api_version() == KVM_API_VERSION);
}

TEST_CASE("VM creation") {
    auto kvm = vmm::kvm::system{};
    REQUIRE(kvm.vm().mmap_size() == kvm.vcpu_mmap_size());
}

TEST_CASE("vCPU mmap size") {
    auto kvm = vmm::kvm::system{};
    REQUIRE(kvm.vcpu_mmap_size() > 0);
}

#if defined(__i386__) || defined(__x86_64__)
TEST_CASE("MSR lists") {
    auto kvm = vmm::kvm::system{};

    SECTION("Index list") {
        auto msrs = kvm.msr_index_list();
        REQUIRE(msrs.size() <= MAX_IO_MSRS);
    }

    SECTION("Feature list") {
        if (kvm.check_extension(KVM_CAP_GET_MSR_FEATURES) > 0) {
            auto features = kvm.msr_feature_list();
            REQUIRE(features.size() <= MAX_IO_MSRS_FEATURES);
        }
    }
}

TEST_CASE("Read MSR features") {
    auto kvm = vmm::kvm::system{};
    auto entries = std::vector<kvm_msr_entry>{};

    SECTION("Feature list") {
        auto indices = kvm.msr_feature_list();

        for (auto index : indices)
            entries.push_back(kvm_msr_entry{index});

        auto msrs = vmm::kvm::Msrs<MAX_IO_MSRS_FEATURES>{entries.begin(),
                                                         entries.end()};

        REQUIRE_NOTHROW(kvm.get_msr_features(msrs));
    }
}

TEST_CASE("Cpuids") {
    auto kvm = vmm::kvm::system{};

    SECTION("Supported cpuids") {
        if (kvm.check_extension(KVM_CAP_EXT_CPUID) > 0) {
            auto cpuids = kvm.supported_cpuids();
            REQUIRE(cpuids.size() <= MAX_CPUID_ENTRIES);
        }
    }

    SECTION("Emulated cpuids") {
        if (kvm.check_extension(KVM_CAP_EXT_EMUL_CPUID) > 0) {
            auto cpuids = kvm.emulated_cpuids();
            REQUIRE(cpuids.size() <= MAX_CPUID_ENTRIES);
        }
    }

    // TODO: Implement comparison operators? Or maybe just compare everything.
    //SECTION("Copied cpuids") {
        //if (kvm.check_extension(KVM_CAP_EXT_CPUID) > 0) {
            //auto cpuids1 = kvm.supported_cpuids();
            //auto cpuids2{cpuids1};
            //auto cpuids3 = cpuids1;
        //}
    //}
}
#endif

#if defined(__arm__) || defined(__aarch64__)
TEST_CASE("Host IPA limit") {
    auto kvm = vmm::kvm::system{};
    auto ipa_limit = kvm.host_ipa_limit();

    if (ipa_limit > 0)
        REQUIRE(ipa_limit >= 32);
    else
        REQUIRE(ipa_limit == 0);
}

TEST_CASE("VM creation (with IPA size)") {
    auto kvm = vmm::kvm::system{};

    if (kvm.check_extension(KVM_CAP_ARM_VM_IPA_SIZE) > 0) {
        auto host_ipa_limit = kvm.host_ipa_limit();

        SECTION("Successful creation with max IPA size") {
            REQUIRE_NOTHROW(kvm.vm(host_ipa_limit));
        }

        // invalid values
        REQUIRE_THROWS(kvm.vm(31));
        REQUIRE_THROWS(kvm.vm(host_ipa_limit + 1));
    }
    else {
        // default size
        REQUIRE_THROWS(kvm.vm(40));
    }
}
#endif
