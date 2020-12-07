//
// vcpu.cpp - vCPU ioctls
//

#include <stdexcept> // runtime_error

#include <sys/mman.h> // mmap, PROT_READ, PROT_WRITE, MAP_FAILED, MAP_SHARED

#include "vmm/kvm/detail/ioctls/vcpu.hpp"
#include "vmm/types/detail/exceptions.hpp"

namespace vmm::kvm::detail {
    Vcpu::Vcpu(int fd, std::size_t mmap_size)
        : m_fd{fd},
          m_mmap_size{mmap_size},
          m_run{static_cast<kvm_run*>(mmap(NULL, mmap_size,
                                           PROT_READ | PROT_WRITE, MAP_SHARED,
                                           fd, 0))}
    {
        // TODO: Check to see this actually works as expected
        // TODO: Do I need to close fd here? Or is it cleaned up elsewhere.
        if (m_run == MAP_FAILED)
            VMM_THROW(std::system_error(errno, std::system_category()));
    }

    // Runs the current vCPU. Returns an exit reason.
    //
    // See documentation for KVM_RUN.
    auto Vcpu::run() const -> VcpuExit
    {
        m_fd.ioctl(KVM_RUN);

        switch(m_run->exit_reason) {
            case KVM_EXIT_UNKNOWN:
            case KVM_EXIT_EXCEPTION:
            case KVM_EXIT_IO:
            case KVM_EXIT_HYPERCALL:
            case KVM_EXIT_DEBUG:
            case KVM_EXIT_HLT:
            case KVM_EXIT_MMIO:
            case KVM_EXIT_IRQ_WINDOW_OPEN:
            case KVM_EXIT_SHUTDOWN:
            case KVM_EXIT_FAIL_ENTRY:
            case KVM_EXIT_INTR:
            case KVM_EXIT_SET_TPR:
            case KVM_EXIT_TPR_ACCESS:
            case KVM_EXIT_S390_SIEIC:
            case KVM_EXIT_S390_RESET:
            case KVM_EXIT_DCR:
            case KVM_EXIT_NMI:
            case KVM_EXIT_INTERNAL_ERROR:
            case KVM_EXIT_OSI:
            case KVM_EXIT_PAPR_HCALL:
            case KVM_EXIT_S390_UCONTROL:
            case KVM_EXIT_WATCHDOG:
            case KVM_EXIT_S390_TSCH:
            case KVM_EXIT_EPR:
            case KVM_EXIT_SYSTEM_EVENT:
            case KVM_EXIT_S390_STSI:
            case KVM_EXIT_IOAPIC_EOI:
            case KVM_EXIT_HYPERV:
            case KVM_EXIT_ARM_NISV:
                return static_cast<VcpuExit>(m_run->exit_reason);
            default:
                VMM_THROW(std::runtime_error("Unexpected exit reason"));
        };
    }

    auto Vcpu::data() const noexcept -> kvm_run*
    {
        return m_run;
    }

    auto Vcpu::immediate_exit() const noexcept -> uint8_t
    {
        return m_run->immediate_exit;
    }

    auto Vcpu::set_immediate_exit(uint8_t val) noexcept -> void
    {
        m_run->immediate_exit = val;
    }

#if defined(__i386__) || defined(__x86_64__)  || \
    defined(__arm__)  || defined(__aarch64__)
        auto Vcpu::mp_state() const -> kvm_mp_state
        {
            auto mp_state = kvm_mp_state{};
            m_fd.ioctl(KVM_GET_MP_STATE, &mp_state);
            return mp_state;
        }

        auto Vcpu::set_mp_state(const kvm_mp_state& mp_state) const -> void
        {
            m_fd.ioctl(KVM_SET_MP_STATE, &mp_state);
        }
#endif

#if defined(__i386__) || defined(__x86_64__)  || \
    defined(__arm__)  || defined(__aarch64__)
        auto Vcpu::vcpu_events() const -> kvm_vcpu_events
        {
            auto vcpu_events = kvm_vcpu_events{};
            m_fd.ioctl(KVM_GET_VCPU_EVENTS, &vcpu_events);
            return vcpu_events;
        }

        auto Vcpu::set_vcpu_events(const kvm_vcpu_events& vcpu_events) const -> void
        {
            m_fd.ioctl(KVM_SET_VCPU_EVENTS, &vcpu_events);
        }
#endif

#if defined(__i386__) || defined(__x86_64__) || \
    defined(__ppc__)  || defined(__ppc64__)
        auto Vcpu::sregs() const -> kvm_sregs
        {
            auto sregs = kvm_sregs{};
            m_fd.ioctl(KVM_GET_SREGS, &sregs);
            return sregs;
        }

        auto Vcpu::set_sregs(const kvm_sregs& sregs) const -> void
        {
            m_fd.ioctl(KVM_SET_SREGS, &sregs);
        }
#endif

#if defined(__i386__) || defined(__x86_64__)
        auto Vcpu::fpu() const -> kvm_fpu
        {
            auto fpu = kvm_fpu{};
            m_fd.ioctl(KVM_GET_FPU, &fpu);
            return fpu;
        }

        auto Vcpu::set_fpu(const kvm_fpu& fpu) const -> void
        {
            m_fd.ioctl(KVM_SET_FPU, &fpu);
        }

        auto Vcpu::lapic() const -> kvm_lapic_state
        {
            auto lapic = kvm_lapic_state{};
            m_fd.ioctl(KVM_GET_LAPIC, &lapic);
            return lapic;
        }

        auto Vcpu::set_lapic(const kvm_lapic_state& lapic) const -> void
        {
            m_fd.ioctl(KVM_SET_LAPIC, &lapic);
        }

        auto Vcpu::xsave() const -> kvm_xsave
        {
            auto xsave = kvm_xsave{};
            m_fd.ioctl(KVM_GET_XSAVE, &xsave);
            return xsave;
        }

        auto Vcpu::set_xsave(const kvm_xsave& xsave) const -> void
        {
            m_fd.ioctl(KVM_SET_XSAVE, &xsave);
        }

        auto Vcpu::xcrs() const -> kvm_xcrs
        {
            auto xcrs = kvm_xcrs{};
            m_fd.ioctl(KVM_GET_XCRS, &xcrs);
            return xcrs;
        }

        auto Vcpu::set_xcrs(const kvm_xcrs& xcrs) const -> void
        {
            m_fd.ioctl(KVM_SET_XCRS, &xcrs);
        }

        auto Vcpu::debug_regs() const -> kvm_debugregs
        {
            auto debug_regs = kvm_debugregs{};
            m_fd.ioctl(KVM_GET_DEBUGREGS, &debug_regs);
            return debug_regs;
        }

        auto Vcpu::set_debug_regs(const kvm_debugregs& debug_regs) const -> void
        {
            m_fd.ioctl(KVM_SET_DEBUGREGS, &debug_regs);
        }
#endif

#if defined(__arm__) || defined(__aarch64__)
        auto Vcpu::init(const kvm_vcpu_init& init) const -> void
        {
            m_fd.ioctl(KVM_ARM_VCPU_INIT, init);
        }

        auto Vcpu::reg(uint64_t id) -> uint64_t {
            auto value = uint64_t{};
            auto reg = kvm_one_reg {
                id,
                reinterpret_cast<uintptr_t>(&value)
            };

            m_fd.ioctl(KVM_GET_ONE_REG, &reg);
            return value;
        }

        auto Vcpu::set_reg(uint64_t id, uint64_t data) -> void {
            auto reg = kvm_one_reg {
                id,
                reinterpret_cast<uintptr_t>(&data)
            };

            m_fd.ioctl(KVM_SET_ONE_REG, &reg);
        }
#endif

#if !defined(__arm__) && !defined(__aarch64__)
        auto Vcpu::regs() const -> kvm_regs
        {
            auto regs = kvm_regs{};
            m_fd.ioctl(KVM_GET_REGS, &regs);
            return regs;
        }

        auto Vcpu::set_regs(const kvm_regs& regs) const -> void
        {
            m_fd.ioctl(KVM_SET_REGS, &regs);
        }
#endif

}  // namespace vmm::kvm::detail
