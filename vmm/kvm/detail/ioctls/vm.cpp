//
// vm.cpp - VM ioctls
//

#include <unistd.h> // sysconf, _SC_PAGESIZE

#include "vmm/kvm/detail/ioctls/vm.hpp"
#include "vmm/kvm/detail/ioctls/vcpu.hpp"
#include "vmm/kvm/detail/ioctls/device.hpp"

namespace vmm::kvm::detail {

auto Vm::vcpu(int vcpu_id) const -> vmm::kvm::detail::Vcpu
{
    return vmm::kvm::detail::Vcpu{m_fd.ioctl(KVM_CREATE_VCPU, vcpu_id),
                                  m_mmap_size};
}

auto Vm::device(uint32_t type, uint32_t flags) const -> vmm::kvm::detail::device
{
    auto dev = kvm_create_device{};
    dev.type = type;
    dev.flags = flags;

    m_fd.ioctl(KVM_CREATE_DEVICE, &dev);

    return vmm::kvm::detail::device{dev};
}

auto Vm::check_extension(int cap) const -> int
{
    return m_fd.ioctl(KVM_CHECK_EXTENSION, cap);
}

auto Vm::set_num_mmu_pages(int n) const -> void {
    m_fd.ioctl(KVM_SET_NR_MMU_PAGES, n);
}

auto Vm::num_mmu_pages() const -> std::size_t {
    return static_cast<std::size_t>(m_fd.ioctl(KVM_GET_NR_MMU_PAGES));
}

auto Vm::set_memslot(kvm_userspace_memory_region &region) const -> void
{
    m_fd.ioctl(KVM_SET_USER_MEMORY_REGION, &region);
}

auto Vm::set_memslot(uint32_t slot, uintptr_t guest_addr,
                     uint64_t mem_size, uintptr_t user_addr,
                     uint32_t flags) const -> void
{
    auto mem_region = kvm_userspace_memory_region {
        slot,
        flags,
        guest_addr,
        mem_size,
        user_addr,
    };

    set_memslot(mem_region);
}

auto Vm::dirty_log(uint32_t slot,
                   unsigned long mem_size) const -> std::vector<uint64_t> {
    auto page_size = sysconf(_SC_PAGESIZE);

    if (page_size == -1)
        VMM_THROW(std::system_error(errno, std::system_category()));

    // Ensure all dirty pages are counted even when memory_size isn't a
    // multiple of page_size * 64.
    const auto aligned_size = static_cast<unsigned long>(page_size) * 64;
    auto bitmap = std::vector<uint64_t>((mem_size + aligned_size - 1) / aligned_size);

    auto dirtylog = kvm_dirty_log{};
    dirtylog.slot = slot;
    dirtylog.dirty_bitmap = bitmap.data();

    m_fd.ioctl(KVM_GET_DIRTY_LOG, &dirtylog);

    return bitmap;
}

auto Vm::mmap_size() const -> std::size_t
{
    return m_mmap_size;
}

auto Vm::num_vcpus() const -> int
{
    auto ret = check_extension(KVM_CAP_NR_VCPUS);
    return ret > 0 ? ret : 4;
}

auto Vm::max_vcpus() const -> int
{
    auto ret = check_extension(KVM_CAP_MAX_VCPUS);
    return ret > 0 ? ret : num_vcpus();
}

auto Vm::num_memslots() const -> int
{
    auto ret = check_extension(KVM_CAP_NR_MEMSLOTS);
    return ret > 0 ? ret : 32;
}

#if defined(__i386__) || defined(__x86_64__)  || \
    defined(__arm__)  || defined(__aarch64__)
auto Vm::irqchip() const -> void
{
    m_fd.ioctl(KVM_CREATE_IRQCHIP);
}

auto Vm::register_irqfd(vmm::types::EventFd eventfd, uint32_t gsi) const -> void
{
    auto irqfd = kvm_irqfd {
        static_cast<uint32_t>(eventfd.fd()),
        gsi
    };

    m_fd.ioctl(KVM_IRQFD, &irqfd);
}

auto Vm::unregister_irqfd(vmm::types::EventFd eventfd, uint32_t gsi) const -> void
{
    auto irqfd = kvm_irqfd {
        static_cast<uint32_t>(eventfd.fd()),
        gsi,
        KVM_IRQFD_FLAG_DEASSIGN
    };

    m_fd.ioctl(KVM_IRQFD, &irqfd);
}
#endif

#if defined(__i386__) || defined(__x86_64__)  || \
    defined(__arm__)  || defined(__aarch64__)
auto Vm::set_irq_line(const uint32_t irq, IrqLevel level) const -> void
{
    auto irq_level = kvm_irq_level {
        { irq },
        static_cast<uint32_t>(level)
    };

    m_fd.ioctl(KVM_IRQ_LINE, &irq_level);
}

auto Vm::signal_msi(const kvm_msi &msi) const -> int {
    return m_fd.ioctl(KVM_SIGNAL_MSI, &msi);
}
#endif

#if defined(__i386__) || defined(__x86_64__)
auto Vm::set_bsp(int vcpu_id) const -> void
{
    m_fd.ioctl(KVM_SET_BOOT_CPU_ID, vcpu_id);
}

auto Vm::get_irqchip(kvm_irqchip &irqchip_p) const -> void
{
    m_fd.ioctl(KVM_GET_IRQCHIP, &irqchip_p);
}

auto Vm::set_irqchip(const kvm_irqchip &irqchip_p) const -> void
{
    m_fd.ioctl(KVM_SET_IRQCHIP, &irqchip_p);
}

auto Vm::get_clock() const -> kvm_clock_data
{
    auto clock = kvm_clock_data{};
    m_fd.ioctl(KVM_GET_CLOCK, &clock);
    return clock;
}

auto Vm::set_clock(const kvm_clock_data &clock) const -> void
{
    m_fd.ioctl(KVM_SET_CLOCK, &clock);
}

auto Vm::set_tss_address(unsigned long address) const -> void {
    m_fd.ioctl(KVM_SET_TSS_ADDR, address);
}

auto Vm::create_pit2(uint32_t flags) const -> void {
    auto config = kvm_pit_config{};
    config.flags = flags;
    m_fd.ioctl(KVM_CREATE_PIT2, &config);
}

auto Vm::pit2() const -> kvm_pit_state2 {
    auto state = kvm_pit_state2{};
    m_fd.ioctl(KVM_GET_PIT2, &state);
    return state;
}

auto Vm::set_pit2(const kvm_pit_state2 &state) const -> void {
    m_fd.ioctl(KVM_SET_PIT2, &state);
}
#endif

#if defined(__arm__) || defined(__aarch64__)
auto Vm::preferred_target() const -> kvm_vcpu_init
{
    auto kvi = kvm_vcpu_init{};
    m_fd.ioctl(KVM_ARM_PREFERRED_TARGET, &kvi);
    return kvi;
}
#endif

}  // namespace vmm::kvm::detail
