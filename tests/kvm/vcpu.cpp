#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include "vmm/kvm/kvm.hpp"

TEST_CASE("vCPU creation") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();

    REQUIRE_NOTHROW(vm.vcpu(0));
}

#if defined(__i386__) || defined(__x86_64__) || \
    defined(__arm__) || defined(__aarch64__) || \
    defined(__s390__)
TEST_CASE("Multi-processing state") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();
    auto vcpu = vm.vcpu(0);
    auto mp_state = vcpu.mp_state();

    REQUIRE_NOTHROW(vcpu.set_mp_state(mp_state));
    auto other = vcpu.mp_state();
    REQUIRE(mp_state.mp_state == other.mp_state);
}
#endif

#if defined(__i386__) || defined(__x86_64__) || \
    defined(__arm__) || defined(__aarch64__)
//TEST_CASE("vCPU events") {
    //auto kvm = vmm::kvm::system{};
    //auto vm = kvm.vm();
    //auto vcpu = vm.vcpu(0);
    //auto events = vcpu.vcpu_events();

    //REQUIRE_NOTHROW(vcpu.set_vcpu_events(events));
    //auto other = vcpu.vcpu_events();
//}
#endif

#if defined(__i386__) || defined(__x86_64__)
TEST_CASE("CPUID2") {
    auto kvm = vmm::kvm::system{};

    if (kvm.check_extension(KVM_CAP_EXT_CPUID)) {
        auto vm = kvm.vm();
        auto num_vcpus = vm.num_vcpus();
        auto supported_cpuids = kvm.supported_cpuids();

        REQUIRE(supported_cpuids.size() <= MAX_CPUID_ENTRIES);

        for (int id = 0; id < num_vcpus; id++) {
            auto vcpu = vm.vcpu(id);
            vcpu.set_cpuid2(supported_cpuids);
            auto cpuids = vcpu.cpuid2<MAX_CPUID_ENTRIES>();

            // Check the first few leafs since some (e.g. 13) are reserved.
            for (std::size_t i = 0; i < 3; i++) {
                REQUIRE(supported_cpuids[i].function == cpuids[i].function);
                REQUIRE(supported_cpuids[i].index == cpuids[i].index);
                REQUIRE(supported_cpuids[i].flags == cpuids[i].flags);
                REQUIRE(supported_cpuids[i].eax == cpuids[i].eax);
                REQUIRE(supported_cpuids[i].ebx == cpuids[i].ebx);
                REQUIRE(supported_cpuids[i].ecx == cpuids[i].ecx);
                REQUIRE(supported_cpuids[i].edx == cpuids[i].edx);
            }
        }
    }
}

TEST_CASE("FPU") {
    // From https://github.com/torvalds/linux/blob/master/arch/x86/include/asm/fpu/internal.h

    auto KVM_FPU_CWD = uint16_t{0x37f};
    auto KVM_FPU_MXCSR = uint32_t{0x1f80};
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();
    auto vcpu = vm.vcpu(0);
    auto fpu = kvm_fpu{};
    fpu.fcw = KVM_FPU_CWD;
    fpu.mxcsr = KVM_FPU_MXCSR;

    REQUIRE_NOTHROW(vcpu.set_fpu(fpu));
    REQUIRE(vcpu.fpu().fcw == KVM_FPU_CWD);
}

TEST_CASE("Xsave") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();
    auto vcpu = vm.vcpu(0);
    auto xsave = vcpu.xsave();

    REQUIRE_NOTHROW(vcpu.set_xsave(xsave));
    auto other = vcpu.xsave();

    for (int i = 0; i < 1024; i++)
        REQUIRE(xsave.region[i] == other.region[i]);
}

TEST_CASE("Xcrs") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();
    auto vcpu = vm.vcpu(0);
    auto xcrs = vcpu.xcrs();

    REQUIRE_NOTHROW(vcpu.set_xcrs(xcrs));
    auto other = vcpu.xcrs();

    REQUIRE(xcrs.nr_xcrs == other.nr_xcrs);
    REQUIRE(xcrs.flags == other.flags);

    for (int i = 0; i < KVM_MAX_XCRS; i++) {
        REQUIRE(xcrs.xcrs[i].xcr == other.xcrs[i].xcr);
        REQUIRE(xcrs.xcrs[i].value == other.xcrs[i].value);
    }
}

TEST_CASE("Debug registers") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();
    auto vcpu = vm.vcpu(0);
    auto regs = vcpu.debug_regs();

    REQUIRE_NOTHROW(vcpu.set_debug_regs(regs));
    auto other = vcpu.debug_regs();

    for (int i = 0; i < 4; i++)
        REQUIRE(regs.db[i] == other.db[i]);

    REQUIRE(regs.dr6 == other.dr6);
    REQUIRE(regs.dr7 == other.dr7);
    REQUIRE(regs.flags == other.flags);
}

TEST_CASE("MSRs") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();
    auto vcpu = vm.vcpu(0);
    auto entries = std::array<kvm_msr_entry, 2>{{
        {0x0000'0174},
        {0x0000'0175, 0, 1}
    }};

    auto msrs_to_set = vmm::kvm::Msrs<2>{entries.begin(), entries.end()};
    REQUIRE_NOTHROW(vcpu.set_msrs(msrs_to_set));

    auto msrs_to_read = vmm::kvm::Msrs<2>{{
        kvm_msr_entry{0x0000'0174},
        kvm_msr_entry{0x0000'0175}
    }};
    auto nmsrs = vcpu.get_msrs(msrs_to_read);

    REQUIRE(nmsrs == msrs_to_set.size());
    REQUIRE(nmsrs == msrs_to_read.size());

    for (std::size_t i = 0; i < msrs_to_read.size(); i++) {
        REQUIRE(msrs_to_read[i].index == entries[i].index);
        REQUIRE(msrs_to_read[i].data == entries[i].data);
    }
}
#endif

#if defined(__arm__) || defined(__aarch64__)
TEST_CASE("Preferred target initialization") {
    auto kvm = vmm::kvm::system{};
    auto vm = kvm.vm();
    auto vcpu = vm.vcpu(0);
    auto kvi = kvm_vcpu_init{};

    REQUIRE_THROWS(vcpu.init(kvi));
    kvi = vm.preferred_target();
    REQUIRE_NOTHROW(vcpu.init(kvi));
}

//TEST_CASE("Register") {
    //auto kvm = vmm::kvm::system{};
    //auto vm = kvm.vm();
    //auto vcpu = vm.vcpu(0);
    //auto kvi = vm.preferred_target(kvi);
    //vcpu.init(kvi);

    //SECTION("Setting") {
        //auto data = uint64_t{};
        //auto id = uint64_t{};

        //REQUIRE_THROWS(vcpu.set_one_reg(id, data));

        //// Exercising KVM_SET_ONE_REG by trying to alter the data inside the
        //// PSTATE register (which is a specific aarch64 register).
        //const auto PSTATE_REG_ID = uint64_t{0x6030'0000'0010'0042};
        //REQUIRE_NOTHROW(vcpu.set_one_reg(PSTATE_REG_ID, data));
    //}

    //SECTION("Getting") {
        //// PSR (Processor State Register) bits. Taken from
        //// arch/arm64/include/uapi/asm/ptrace.h.
        //const auto PSR_MODE_EL1H = uint64_t{0x0000'0005};
        //const auto PSR_F_BIT = uint64_t{0x0000'0040};
        //const auto PSR_I_BIT = uint64_t{0x0000'0080};
        //const auto PSR_A_BIT = uint64_t{0x0000'0100};
        //const auto PSR_D_BIT = uint64_t{0x0000'0200};
        //const auto PSTATE_FAULT_BITS_64 = uint64_t{(PSR_MODE_EL1H |
                                                    //PSR_A_BIT |
                                                    //PSR_F_BIT |
                                                    //PSR_I_BIT |
                                                    //PSR_D_BIT)};
        //auto data = uint64_t{PSTATE_FAULT_BITS_64};
        //const auto PSTATE_REG_ID = uint64_t{0x6030'0000'0010'0042};

        //REQUIRE_NOTHROW(vcpu.set_one_reg(PSTATE_REG_ID, data));
        //REQUIRE(vcpu.get_one_reg(PSTATE_REG_ID) == PSTATE_FAULT_BITS_64);
    //}
//}
#endif
