//
// vcpu.cpp - vCPU ioctls
//

#include "vmm/kvm/detail/ioctls/vcpu.hpp"

namespace vmm::kvm::detail {
    vcpu::vcpu(int fd, std::size_t mmap_size)
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

#if defined(__i386__) || defined(__x86_64__)  || \
    defined(__arm__)  || defined(__aarch64__) || \
    defined(__s390__)
        auto vcpu::mp_state() const -> kvm_mp_state {
            auto mp_state = kvm_mp_state{};
            m_fd.ioctl(KVM_GET_MP_STATE, &mp_state);
            return mp_state;
        }

        auto vcpu::set_mp_state(const kvm_mp_state& mp_state) const -> void {
            m_fd.ioctl(KVM_SET_MP_STATE, &mp_state);
        }
#endif

#if defined(__i386__) || defined(__x86_64__)  || \
    defined(__arm__)  || defined(__aarch64__)
        auto vcpu::vcpu_events() const -> kvm_vcpu_events {
            auto vcpu_events = kvm_vcpu_events{};
            m_fd.ioctl(KVM_GET_VCPU_EVENTS, &vcpu_events);
            return vcpu_events;
        }

        auto vcpu::set_vcpu_events(const kvm_vcpu_events& vcpu_events) const -> void {
            m_fd.ioctl(KVM_SET_VCPU_EVENTS, &vcpu_events);
        }
#endif

#if defined(__i386__) || defined(__x86_64__) || \
    defined(__ppc__)  || defined(__ppc64__)
        auto vcpu::sregs() const -> kvm_sregs {
            auto sregs = kvm_sregs{};
            m_fd.ioctl(KVM_GET_SREGS, &sregs);
            return sregs;
        }

        auto vcpu::set_sregs(const kvm_sregs& sregs) const -> void {
            m_fd.ioctl(KVM_SET_SREGS, &sregs);
        }
#endif

#if defined(__i386__) || defined(__x86_64__)
        auto vcpu::fpu() const -> kvm_fpu {
            auto fpu = kvm_fpu{};
            m_fd.ioctl(KVM_GET_FPU, &fpu);
            return fpu;
        }

        auto vcpu::set_fpu(const kvm_fpu& fpu) const -> void {
            m_fd.ioctl(KVM_SET_FPU, &fpu);
        }

        auto vcpu::lapic() const -> kvm_lapic_state {
            auto lapic = kvm_lapic_state{};
            m_fd.ioctl(KVM_GET_LAPIC, &lapic);
            return lapic;
        }

        auto vcpu::set_lapic(const kvm_lapic_state& lapic) const -> void {
            m_fd.ioctl(KVM_SET_LAPIC, &lapic);
        }

        auto vcpu::xsave() const -> kvm_xsave {
            auto xsave = kvm_xsave{};
            m_fd.ioctl(KVM_GET_XSAVE, &xsave);
            return xsave;
        }

        auto vcpu::set_xsave(const kvm_xsave& xsave) const -> void {
            m_fd.ioctl(KVM_SET_XSAVE, &xsave);
        }

        auto vcpu::xcrs() const -> kvm_xcrs {
            auto xcrs = kvm_xcrs{};
            m_fd.ioctl(KVM_GET_XCRS, &xcrs);
            return xcrs;
        }

        auto vcpu::set_xcrs(const kvm_xcrs& xcrs) const -> void {
            m_fd.ioctl(KVM_SET_XCRS, &xcrs);
        }

        auto vcpu::debug_regs() const -> kvm_debugregs {
            auto debug_regs = kvm_debugregs{};
            m_fd.ioctl(KVM_GET_DEBUGREGS, &debug_regs);
            return debug_regs;
        }

        auto vcpu::set_debug_regs(const kvm_debugregs& debug_regs) const -> void {
            m_fd.ioctl(KVM_SET_DEBUGREGS, &debug_regs);
        }
#endif

#if defined(__arm__) || defined(__aarch64__)
        auto vcpu::init(const kvm_vcpu_init& init) const -> void {
            m_fd.ioctl(KVM_ARM_VCPU_INIT, init);
        }
#endif

#if !defined(__arm__) && !defined(__aarch64__)
        auto vcpu::regs() const -> kvm_regs {
            auto regs = kvm_regs{};
            m_fd.ioctl(KVM_GET_REGS, &regs);
            return regs;
        }

        auto vcpu::set_regs(const kvm_regs& regs) const -> void {
            m_fd.ioctl(KVM_SET_REGS, &regs);
        }
#endif

}  // namespace vmm::kvm::detail
