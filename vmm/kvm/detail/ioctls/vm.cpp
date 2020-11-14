//
// vm.cpp - VM ioctls
//

#include <unistd.h> // sysconf, _SC_PAGESIZE

#include "vmm/kvm/detail/ioctls/vm.hpp"
#include "vmm/kvm/detail/ioctls/vcpu.hpp"
#include "vmm/kvm/detail/ioctls/device.hpp"

namespace vmm::kvm::detail {

auto vm::vcpu(int vcpu_id) const -> vmm::kvm::detail::vcpu
{
    return vmm::kvm::detail::vcpu{m_fd.ioctl(KVM_CREATE_VCPU, vcpu_id),
                                  m_mmap_size};
}

auto vm::device(uint32_t type, uint32_t flags) const -> vmm::kvm::detail::device
{
    auto dev = kvm_create_device{};
    dev.type = type;
    dev.flags = flags;

    m_fd.ioctl(KVM_CREATE_DEVICE, &dev);

    return vmm::kvm::detail::device{dev};
}

auto vm::check_extension(int cap) const -> int
{
    return m_fd.ioctl(KVM_CHECK_EXTENSION, cap);
}

auto vm::memslot(kvm_userspace_memory_region region) const -> void
{
    m_fd.ioctl(KVM_SET_USER_MEMORY_REGION, &region);
}

auto vm::dirty_log(uint32_t slot,
                   unsigned long memory_size) const -> std::vector<uint64_t> {
    // Compute length of bitmap needed for all dirty pages in one memory slot.
    // One memory page is `page_size` bytes and KVM_GET_DIRTY_LOG returns one
    // dirty bit for each page.
    auto page_size = sysconf(_SC_PAGESIZE);

    if (page_size == -1)
        VMM_THROW(std::system_error(errno, std::system_category()));

    // For ease of access we are saving the bitmap in a u64 vector.
    auto bitmap = std::vector<uint64_t>{};

    // Ceiling division to ensure all dirty pages are counted even when
    // memory_size isn't a multiple of page_size * 64.
    bitmap.reserve((memory_size + static_cast<unsigned long>(page_size) * 64 - 1) / static_cast<unsigned long>(page_size) * 64);

    auto dirtylog = kvm_dirty_log{};
    dirtylog.slot = slot;
    dirtylog.dirty_bitmap = bitmap.data();

    m_fd.ioctl(KVM_GET_DIRTY_LOG, &dirtylog);
    return bitmap;
}

auto vm::mmap_size() const -> std::size_t
{
    return m_mmap_size;
}

auto vm::num_vcpus() const -> int
{
    auto ret = check_extension(KVM_CAP_NR_VCPUS);
    return ret > 0 ? ret : 4;
}

auto vm::max_vcpus() const -> int
{
    auto ret = check_extension(KVM_CAP_MAX_VCPUS);
    return ret > 0 ? ret : num_vcpus();
}

auto vm::num_memslots() const -> int
{
    auto ret = check_extension(KVM_CAP_NR_MEMSLOTS);
    return ret > 0 ? ret : 32;
}

#if defined(__i386__) || defined(__x86_64__)  || \
    defined(__arm__)  || defined(__aarch64__)
auto vm::irqchip() const -> void
{
    m_fd.ioctl(KVM_CREATE_IRQCHIP);
}

auto vm::register_irqfd(vmm::types::EventFd eventfd, uint32_t gsi) const -> void
{
    auto irqfd = kvm_irqfd {
        static_cast<uint32_t>(eventfd.fd()),
        gsi
    };

    m_fd.ioctl(KVM_IRQFD, &irqfd);
}

auto vm::unregister_irqfd(vmm::types::EventFd eventfd, uint32_t gsi) const -> void
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
auto vm::set_irq_line(const uint32_t irq, IrqLevel level) const -> void
{
    auto irq_level = kvm_irq_level {
        { irq },
        static_cast<uint32_t>(level)
    };

    m_fd.ioctl(KVM_IRQ_LINE, &irq_level);
}

auto vm::signal_msi(const kvm_msi &msi) const -> int {
    return m_fd.ioctl(KVM_SIGNAL_MSI, &msi);
}
#endif

#if defined(__i386__) || defined(__x86_64__)
auto vm::set_bsp(int vcpu_id) const -> void
{
    m_fd.ioctl(KVM_SET_BOOT_CPU_ID, vcpu_id);
}

auto vm::get_irqchip(kvm_irqchip &irqchip_p) const -> void
{
    m_fd.ioctl(KVM_GET_IRQCHIP, &irqchip_p);
}

auto vm::set_irqchip(const kvm_irqchip &irqchip_p) const -> void
{
    m_fd.ioctl(KVM_SET_IRQCHIP, &irqchip_p);
}

auto vm::get_clock() const -> kvm_clock_data
{
    auto clock = kvm_clock_data{};
    m_fd.ioctl(KVM_GET_CLOCK, &clock);
    return clock;
}

auto vm::set_clock(const kvm_clock_data &clock) const -> void
{
    m_fd.ioctl(KVM_SET_CLOCK, &clock);
}

auto vm::set_tss_address(unsigned long address) const -> void {
    m_fd.ioctl(KVM_SET_TSS_ADDR, address);
}

auto vm::create_pit2(uint32_t flags) const -> void {
    auto config = kvm_pit_config{};
    config.flags = flags;
    m_fd.ioctl(KVM_CREATE_PIT2, &config);
}

auto vm::pit2() const -> kvm_pit_state2 {
    auto state = kvm_pit_state2{};
    m_fd.ioctl(KVM_GET_PIT2, &state);
    return state;
}

auto vm::set_pit2(const kvm_pit_state2 &state) const -> void {
    m_fd.ioctl(KVM_SET_PIT2, &state);
}
#endif

#if defined(__arm__) || defined(__aarch64__)
auto vm::preferred_target() const -> kvm_vcpu_init
{
    auto kvi = kvm_vcpu_init{};
    m_fd.ioctl(KVM_GET_CLOCK, &kvi);
    return kvi;
}
#endif

}  // namespace vmm::kvm::detail
