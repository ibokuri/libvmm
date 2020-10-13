//
// vm.cpp - VM ioctls
//

#include "vmm/kvm/detail/ioctls/vm.hpp"
#include "vmm/kvm/detail/ioctls/vcpu.hpp"
#include "vmm/kvm/detail/ioctls/device.hpp"

namespace vmm::kvm::detail {

auto vm::vcpu(unsigned vcpu_id) const -> vmm::kvm::detail::vcpu
{
    return vmm::kvm::detail::vcpu{m_fd.ioctl(KVM_CREATE_VCPU, vcpu_id)};
}

auto vm::device(uint32_t type, uint32_t flags) const -> vmm::kvm::detail::device
{
    auto dev = kvm_create_device{ .type = type, .flags = flags };
    return vmm::kvm::detail::device{dev};
}

auto vm::check_extension(unsigned cap) const -> unsigned
{
    return m_fd.ioctl(KVM_CHECK_EXTENSION, cap);
}

auto vm::memslot(kvm_userspace_memory_region region) const -> void
{
    m_fd.ioctl(KVM_SET_USER_MEMORY_REGION, &region);
}

auto vm::mmap_size() const -> std::size_t
{
    return m_mmap_size;
}

auto vm::num_vcpus() const -> unsigned
{
    auto ret = check_extension(KVM_CAP_NR_VCPUS);
    return ret > 0 ? ret : 4;
}

auto vm::max_vcpus() const -> unsigned
{
    auto ret = check_extension(KVM_CAP_MAX_VCPUS);
    return ret > 0 ? ret : num_vcpus();
}

auto vm::num_memslots() const -> unsigned
{
    auto ret = check_extension(KVM_CAP_NR_MEMSLOTS);
    return ret > 0 ? ret : 32;
}

#if defined(__i386__) || defined(__x86_64__)
auto vm::set_bsp(unsigned vcpu_id) const -> void
{
    m_fd.ioctl(KVM_SET_BOOT_CPU_ID, vcpu_id);
}

auto vm::get_irqchip(kvm_irqchip &irqchip_p) const -> void
{
    m_fd.ioctl(KVM_GET_IRQCHIP, &irqchip_p);
}

auto vm::set_irqchip(kvm_irqchip const &irqchip_p) const -> void
{
    m_fd.ioctl(KVM_SET_IRQCHIP, &irqchip_p);
}

auto vm::get_clock() const -> kvm_clock_data
{
    auto clock = kvm_clock_data{};
    m_fd.ioctl(KVM_GET_CLOCK, &clock);
    return clock;
}

auto vm::set_clock(kvm_clock_data &clock) const -> void
{
    m_fd.ioctl(KVM_SET_CLOCK, &clock);
}
#endif

#if defined(__i386__) || defined(__x86_64__)  || \
    defined(__arm__)  || defined(__aarch64__)
auto vm::set_irq_line(const uint32_t irq, bool active) const -> void
{
    auto irq_level = kvm_irq_level {
        .irq = irq,
        .level = active ? uint32_t{1} : uint32_t{0}
    };

    m_fd.ioctl(KVM_IRQ_LINE, &irq_level);
}
#endif

#if defined(__i386__) || defined(__x86_64__)  || \
    defined(__arm__)  || defined(__aarch64__) || \
    defined(__s390__)
auto vm::irqchip() const -> void
{
    m_fd.ioctl(KVM_CREATE_IRQCHIP);
}

auto vm::register_irqfd(vmm::types::EventFd eventfd, uint32_t gsi) const -> void
{
    auto irqfd = kvm_irqfd {
        .fd = static_cast<uint32_t>(eventfd.fd()),
        .gsi = gsi
    };

    m_fd.ioctl(KVM_IRQFD, &irqfd);
}

auto vm::unregister_irqfd(vmm::types::EventFd eventfd, uint32_t gsi) const -> void
{
    auto irqfd = kvm_irqfd {
        .fd = static_cast<uint32_t>(eventfd.fd()),
        .gsi = gsi,
        .flags = KVM_IRQFD_FLAG_DEASSIGN
    };

    m_fd.ioctl(KVM_IRQFD, &irqfd);
}
#endif

}  // namespace vmm::kvm::detail
