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

class vcpu
{
    private:
        KvmFd m_fd;
        std::size_t m_mmap_size;
        kvm_run *m_run = nullptr;

        friend vcpu vm::vcpu(int vcpu_id) const;

        explicit vcpu(int fd, std::size_t mmap_size);
    public:
        // Returns the immediate_exit flag in m_run.
        auto immediate_exit() -> uint8_t;

        // Sets the immediate_exit flag in m_run.
        auto set_immediate_exit(uint8_t val) -> void;

#if defined(__i386__) || defined(__x86_64__)  || \
    defined(__arm__)  || defined(__aarch64__) || \
    defined(__s390__)
        // Returns the vCPU's current "multiprocessing state".
        //
        // See the documentation for KVM_GET_MP_STATE.
        auto mp_state() const -> kvm_mp_state;

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
        auto vcpu_events() const -> kvm_vcpu_events;

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
        auto sregs() const -> kvm_sregs;

        // Sets special registers of the vCPU.
        //
        // See the documentation for KVM_SET_SREGS.
        auto set_sregs(const kvm_sregs&) const -> void;
#endif

#if defined(__i386__) || defined(__x86_64__)
        // Returns the floating point state (FPU) of the vCPU.
        //
        // See the documentation for KVM_GET_FPU.
        auto fpu() const -> kvm_fpu;

        // Sets the floating point state (FPU) of the vCPU.
        //
        // See the documentation for KVM_SET_FPU.
        auto set_fpu(const kvm_fpu&) const -> void;

        // Returns the state of the Local Advanced Programmable Interrupt
        // Controller (LAPIC).
        //
        // See the documentation for KVM_GET_LAPIC.
        auto lapic() const -> kvm_lapic_state;

        // Sets the state of the Local Advanced Programmable Interrupt
        // Controller (LAPIC).
        //
        // See the documentation for KVM_SET_LAPIC.
        auto set_lapic(const kvm_lapic_state&) const -> void;

        // Returns the vCPU's current xsave struct.
        //
        // See the documentation for KVM_GET_XSAVE.
        auto xsave() const -> kvm_xsave;

        // Sets the vCPU's current xsave struct.
        //
        // See the documentation for KVM_SET_XSAVE.
        auto set_xsave(const kvm_xsave&) const -> void;

        // Returns the vCPU's current xcrs struct.
        //
        // See the documentation for KVM_GET_XCRS.
        auto xcrs() const -> kvm_xcrs;

        // Sets the vCPU's current xcrs struct.
        //
        // See the documentation for KVM_SET_XCRS.
        auto set_xcrs(const kvm_xcrs&) const -> void;

        // Returns vCPU's current debug registers.
        //
        // See the documentation for KVM_GET_DEBUGREGS.
        auto debug_regs() const -> kvm_debugregs;

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
        auto cpuid2() const -> vmm::kvm::detail::Cpuids<N>
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
        auto reg(uint64_t id) -> uint64_t;

        // Sets the value of one register for this vCPU.
        //
        // See the documentation for KVM_SET_ONE_REG.
        auto set_reg(uint64_t id, uint64_t data) -> void;
#endif

#if !defined(__arm__) && !defined(__aarch64__)
        // Returns the vCPU general purpose registers.
        //
        // See documentation for KVM_GET_REGS.
        auto regs() const -> kvm_regs;

        // Sets the vCPU's general purpose registers.
        //
        // See documentation for KVM_SET_REGS.
        auto set_regs(const kvm_regs&) const -> void;
#endif
};

}  // namespace vmm::kvm::detail
