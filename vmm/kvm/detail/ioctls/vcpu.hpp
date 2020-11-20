//
// vcpu.hpp - vcpu ioctls
//

#pragma once

#include <cstddef> // size_t
#include <cstdint> // uint*_t

#include <linux/kvm.h> // kvm_*, KVM_*

#include "vmm/kvm/detail/ioctls/vm.hpp"
#include "vmm/kvm/detail/types/file_descriptor.hpp"

namespace vmm::kvm::detail {

enum class VcpuExit: uint32_t {
    Unknown,       // KVM_EXIT_UNKNOWN
    Exception,     // KVM_EXIT_EXCEPTION
    Io,            // KVM_EXIT_IO
    Hypercall,     // KVM_EXIT_HYPERCALL
    Debug,         // KVM_EXIT_DEBUG
    Hlt,           // KVM_EXIT_HLT
    Mmio,          // KVM_EXIT_MMIO
    IrqWindowOpen, // KVM_EXIT_IRQ_WINDOW_OPEN
    Shutdown,      // KVM_EXIT_SHUTDOWN
    FailEntry,     // KVM_EXIT_FAIL_ENTRY
    Intr,          // KVM_EXIT_INTR
    SetTpr,        // KVM_EXIT_SET_TPR
    TprAccess,     // KVM_EXIT_TPR_ACCESS
    S390Sieic,     // KVM_EXIT_S390_SIEIC
    S390Reset,     // KVM_EXIT_S390_RESET
    Dcr,           // KVM_EXIT_DCR
    Nmi,           // KVM_EXIT_NMI
    InternalError, // KVM_EXIT_INTERNAL_ERROR
    Osi,           // KVM_EXIT_OSI
    PaprHcall,     // KVM_EXIT_PAPR_HCALL
    S390Ucontrol,  // KVM_EXIT_S390_UCONTROL
    Watchdog,      // KVM_EXIT_WATCHDOG
    S390Tsch,      // KVM_EXIT_S390_TSCH
    Epr,           // KVM_EXIT_EPR
    SystemEvent,   // KVM_EXIT_SYSTEM_EVENT
    S390Stsi,      // KVM_EXIT_S390_STSI
    IoapicEoi,     // KVM_EXIT_IOAPIC_EOI
    Hyperv,        // KVM_EXIT_HYPERV
    ArmNsiv,       // KVM_EXIT_HYPERV
};

class vcpu
{
    private:
        KvmFd m_fd;
        std::size_t m_mmap_size;
        kvm_run *m_run = nullptr;

        friend vcpu vm::vcpu(int vcpu_id) const;

        explicit vcpu(int fd, std::size_t mmap_size);
    public:
        [[nodiscard]] auto run() const -> VcpuExit;
        [[nodiscard]] auto data() const noexcept -> kvm_run*;

        // Returns the immediate_exit flag in m_run.
        [[nodiscard]] auto immediate_exit() const noexcept -> uint8_t;

        // Sets the immediate_exit flag in m_run.
        auto set_immediate_exit(uint8_t val) noexcept -> void;

#if !defined(__arm__) && !defined(__aarch64__)
        // Returns the vCPU general purpose registers.
        //
        // See documentation for KVM_GET_REGS.
        [[nodiscard]] auto regs() const -> kvm_regs;

        // Sets the vCPU's general purpose registers.
        //
        // See documentation for KVM_SET_REGS.
        auto set_regs(const kvm_regs&) const -> void;
#endif

#if defined(__i386__) || defined(__x86_64__)  || \
    defined(__arm__)  || defined(__aarch64__)
        // Returns the vCPU's current "multiprocessing state".
        //
        // See the documentation for KVM_GET_MP_STATE.
        [[nodiscard]] auto mp_state() const -> kvm_mp_state;

        // Sets the vCPU's current "multiprocessing state".
        //
        // See the documentation for KVM_SET_MP_STATE.
        auto set_mp_state(const kvm_mp_state&) const -> void;
#endif

#if defined(__i386__) || defined(__x86_64__)  || \
    defined(__arm__)  || defined(__aarch64__)
        // Returns currently pending exceptions, interrupts, and NMIs as well
        // as related states of the vCPU.
        //
        // See the documentation for KVM_GET_VCPU_EVENTS.
        [[nodiscard]] auto vcpu_events() const -> kvm_vcpu_events;

        // Sets pending exceptions, interrupts, and NMIs as well as related
        // states of the vcpu.
        //
        // See the documentation for KVM_SET_VCPU_EVENTS.
        auto set_vcpu_events(const kvm_vcpu_events&) const -> void;
#endif

#if defined(__i386__) || defined(__x86_64__) || \
    defined(__ppc__)  || defined(__ppc64__)
        // Returns special registers of the vCPU.
        //
        // See the documentation for KVM_GET_SREGS.
        [[nodiscard]] auto sregs() const -> kvm_sregs;

        // Sets special registers of the vCPU.
        //
        // See the documentation for KVM_SET_SREGS.
        auto set_sregs(const kvm_sregs&) const -> void;
#endif

#if defined(__i386__) || defined(__x86_64__)
        // Returns the floating point state (FPU) of the vCPU.
        //
        // See the documentation for KVM_GET_FPU.
        [[nodiscard]] auto fpu() const -> kvm_fpu;

        // Sets the floating point state (FPU) of the vCPU.
        //
        // See the documentation for KVM_SET_FPU.
        auto set_fpu(const kvm_fpu&) const -> void;

        // Returns the state of the Local Advanced Programmable Interrupt
        // Controller (LAPIC).
        //
        // See the documentation for KVM_GET_LAPIC.
        [[nodiscard]] auto lapic() const -> kvm_lapic_state;

        // Sets the state of the Local Advanced Programmable Interrupt
        // Controller (LAPIC).
        //
        // See the documentation for KVM_SET_LAPIC.
        auto set_lapic(const kvm_lapic_state&) const -> void;

        // Returns the vCPU's current xsave struct.
        //
        // See the documentation for KVM_GET_XSAVE.
        [[nodiscard]] auto xsave() const -> kvm_xsave;

        // Sets the vCPU's current xsave struct.
        //
        // See the documentation for KVM_SET_XSAVE.
        auto set_xsave(const kvm_xsave&) const -> void;

        // Returns the vCPU's current xcrs struct.
        //
        // See the documentation for KVM_GET_XCRS.
        [[nodiscard]] auto xcrs() const -> kvm_xcrs;

        // Sets the vCPU's current xcrs struct.
        //
        // See the documentation for KVM_SET_XCRS.
        auto set_xcrs(const kvm_xcrs&) const -> void;

        // Returns vCPU's current debug registers.
        //
        // See the documentation for KVM_GET_DEBUGREGS.
        [[nodiscard]] auto debug_regs() const -> kvm_debugregs;

        // Sets the vCPU's current debug registers.
        //
        // See the documentation for KVM_SET_DEBUGREGS.
        auto set_debug_regs(const kvm_debugregs&) const -> void;

        // Reads MSRs from the vcpu. Returns the number of successfully read
        // values.
        //
        // See the documentation for KVM_GET_MSRS.
        template<typename T,
                 typename=std::enable_if_t<std::is_same_v<typename T::value_type,
                                                          kvm_msr_entry>>>
        auto get_msrs(T &msrs) const -> std::size_t
        {
            return static_cast<std::size_t>(m_fd.ioctl(KVM_GET_MSRS, msrs.data()));
        }

        // Writes MSRs to the vcpu. Returns the number of successfully written
        // values.
        //
        // See the documentation for KVM_SET_MSRS.
        template<typename T,
                 typename=std::enable_if_t<std::is_same_v<typename T::value_type,
                                                          kvm_msr_entry>>>
        auto set_msrs(T &msrs) const -> std::size_t
        {
            return static_cast<std::size_t>(m_fd.ioctl(KVM_SET_MSRS, msrs.data()));
        }

        // Returns CPUID registers of a vCPU.
        //
        // See the documentation for KVM_GET_CPUID2.
        template<uint32_t N>
        [[nodiscard]] auto cpuid2() const -> vmm::kvm::detail::Cpuids<N>
        {
            auto cpuids = vmm::kvm::detail::Cpuids<N>{};
            m_fd.ioctl(KVM_GET_CPUID2, cpuids.data());
            return cpuids;
        }

        // Sets the vCPU's responses to the passed-in CPUID instruction.
        //
        // See the documentation for KVM_SET_CPUID2.
        template<typename T,
                 typename=std::enable_if_t<std::is_same_v<typename T::value_type,
                                                          kvm_cpuid_entry2>>>
        auto set_cpuid2(T &cpuids) const -> void
        {
            m_fd.ioctl(KVM_SET_CPUID2, cpuids.data());
        }
#endif

#if defined(__arm__) || defined(__aarch64__)
        // Initializes an ARM vCPU to the specified type with the specified
        // features and resets the values of all of its registers to defaults.
        //
        // See the documentation for KVM_ARM_VCPU_INIT.
        auto init(const kvm_vcpu_init&) const -> void;

        // Returns the value of the specified vCPU register.
        //
        // See the documentation for KVM_GET_ONE_REG.
        [[nodiscard]] auto reg(uint64_t id) -> uint64_t;

        // Sets the value of one register for this vCPU.
        //
        // See the documentation for KVM_SET_ONE_REG.
        auto set_reg(uint64_t id, uint64_t data) -> void;
#endif
};

}  // namespace vmm::kvm::detail
