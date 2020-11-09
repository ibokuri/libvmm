//
// vcpu.hpp - vcpu ioctls
//

#pragma once

#include <cstddef> // size_t
#include <cstdint> // uint32_t, uint64_t
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
#if defined(__i386__) || defined(__x86_64__)  || \
    defined(__arm__)  || defined(__aarch64__) || \
    defined(__s390__)
        auto mp_state() const -> kvm_mp_state;
        auto set_mp_state(const kvm_mp_state&) const -> void;
#endif

#if defined(__i386__) || defined(__x86_64__)  || \
    defined(__arm__)  || defined(__aarch64__)
        auto vcpu_events() const -> kvm_vcpu_events;
        auto set_vcpu_events(const kvm_vcpu_events&) const -> void;
#endif

#if defined(__i386__) || defined(__x86_64__) || \
    defined(__ppc__)  || defined(__ppc64__)
        auto sregs() const -> kvm_sregs;
        auto set_sregs(const kvm_sregs&) const -> void;
#endif

#if defined(__i386__) || defined(__x86_64__)
        auto fpu() const -> kvm_fpu;
        auto set_fpu(const kvm_fpu&) const -> void;
        auto lapic() const -> kvm_lapic_state;
        auto set_lapic(const kvm_lapic_state&) const -> void;
        auto xsave() const -> kvm_xsave;
        auto set_xsave(const kvm_xsave&) const -> void;
        auto xcrs() const -> kvm_xcrs;
        auto set_xcrs(const kvm_xcrs&) const -> void;
        auto debug_regs() const -> kvm_debugregs;
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
#endif

#if defined(__arm__) || defined(__aarch64__)
        auto init(const kvm_vcpu_init&) const -> void;
#else
        auto regs() const -> kvm_regs;
        auto set_regs(const kvm_regs&) const -> void;
#endif
};

}  // namespace vmm::kvm::detail
