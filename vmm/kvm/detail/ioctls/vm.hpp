//
// vm.hpp - VM ioctls
//

#pragma once

#include <cstddef> // size_t
#include <cstdint> // uint32_t, uint64_t
#include <linux/kvm.h> // kvm_*, KVM_*
#include <vector> // vector

#include "vmm/kvm/detail/ioctls/system.hpp"
#include "vmm/kvm/detail/types/file_descriptor.hpp"
#include "vmm/kvm/detail/types/fam_struct.hpp"
#include "vmm/types/event.hpp"

namespace vmm::kvm::detail {

enum class IrqLevel: uint32_t {
    Inactive,
    Active
};

class vcpu;
class device;

class Vm
{
    private:
        KvmFd m_fd;
        std::size_t m_mmap_size;

        friend Vm System::vm(std::size_t machine_type) const;

        Vm(int fd, std::size_t mmap_size) noexcept
            : m_fd{fd}, m_mmap_size{mmap_size} {}
    public:
        Vm(const Vm& other) = delete;
        Vm(Vm&& other) = default;
        auto operator=(const Vm& other) -> Vm& = delete;
        auto operator=(Vm&& other) -> Vm& = default;

        // Returns the VM's kvm_run's shared memory region size.
        [[nodiscard]] auto mmap_size() const -> std::size_t;

        // Adds a vcpu to a virtual machine.
        //
        // See the documentation for KVM_CREATE_VCPU.
        [[nodiscard]] auto vcpu(int vcpu_id) const -> vmm::kvm::detail::vcpu;

        // Adds a device to a virtual machine.
        //
        // See the documentation for KVM_CREATE_DEVICE.
        [[nodiscard]] auto device(uint32_t type, uint32_t flags=0) const -> vmm::kvm::detail::device;

        // Returns a positive integer if a KVM extension is available; 0
        // otherwise.
        //
        // Based on their initialization, VMs may have different capabilities.
        // Therefore, `kvm::Vm::check_extension()` is preferred when querying
        // for most capabilities.
        [[nodiscard]] auto check_extension(int cap) const -> int;

        // Sets the number of MMU pages for a virtual machine.
        //
        // See the documentation for KVM_SET_NR_MMU_PAGES.
        auto set_num_mmu_pages(int n) const -> void;

        // Returns the number of MMU pages for a virtual machine.
        //
        // See the documentation for KVM_GET_NR_MMU_PAGES.
        [[nodiscard]] auto num_mmu_pages() const -> std::size_t;

        // Creates, modifies, or deletes a guest physical memory slot.
        //
        // See the documentation for KVM_SET_USER_MEMORY_REGION.
        auto set_memslot(kvm_userspace_memory_region&) const -> void;

        auto set_memslot(uint32_t slot, uintptr_t guest_addr,
                         uint64_t mem_size, uintptr_t user_addr,
                         uint32_t flags) const -> void;

        // Given a memory slot, returns a vector containing any pages dirtied
        // since the last call to this ioctl.
        //
        // See the documentation for KVM_GET_DIRTY_LOG.
        auto dirty_log(uint32_t slot,
                       unsigned long mem_size) const -> std::vector<uint64_t>;

        // Attaches an ioeventfd to a legal pio/mmio address within the guest.
        //
        // See the documentation for KVM_IOEVENTFD.
        template<vmm::types::IoEventAddress T>
        auto attach_ioevent(vmm::types::EventFd eventfd, uint64_t addr,
                            uint64_t datamatch=0) const -> void
        {
            auto flags = uint32_t{};

            if (datamatch > 0) {
                flags |= KVM_IOEVENTFD_FLAG_DATAMATCH;
            }

            if constexpr (T == vmm::types::IoEventAddress::Pio) {
                flags |= KVM_IOEVENTFD_FLAG_PIO;
            }

            auto ioeventfd = kvm_ioeventfd {
                datamatch,
                addr,
                sizeof(uint64_t),
                eventfd.fd(),
                flags
            };

            m_fd.ioctl(KVM_IOEVENTFD, &ioeventfd);
        }

        // Detaches an ioeventfd from a legal pio/mmio address within the
        // guest.
        //
        // See the documentation for KVM_IOEVENTFD.
        template<vmm::types::IoEventAddress T>
        auto detach_ioevent(vmm::types::EventFd eventfd, uint64_t addr,
                            uint64_t datamatch=0) const -> void
        {
            auto flags = uint32_t{KVM_IOEVENTFD_FLAG_DEASSIGN};

            if (datamatch > 0) {
                flags |= KVM_IOEVENTFD_FLAG_DATAMATCH;
            }

            if constexpr (T == vmm::types::IoEventAddress::Pio) {
                flags |= KVM_IOEVENTFD_FLAG_PIO;
            }

            auto ioeventfd = kvm_ioeventfd {
                datamatch,
                addr,
                sizeof(uint64_t),
                eventfd.fd(),
                flags
            };

            m_fd.ioctl(KVM_IOEVENTFD, &ioeventfd);
        }

        // Returns the recommended number for max_vcpus.
        [[nodiscard]] auto num_vcpus() const -> int;

        // Returns the maximum possible value for max_vcpus.
        [[nodiscard]] auto max_vcpus() const -> int;

        // Returns the maximum number of allowed memory slots for a VM.
        [[nodiscard]] auto num_memslots() const -> int;

#if defined(__i386__) || defined(__x86_64__)  || \
    defined(__arm__)  || defined(__aarch64__)
        // Creates an interrupt controller model in the kernel.
        //
        // See the documentation for KVM_CREATE_IRQCHIP.
        auto irqchip() const -> void;

        // Registers an event that will, when signaled, trigger the gsi IRQ.
        auto register_irqfd(vmm::types::EventFd, uint32_t gsi) const -> void;

        // Unregisters an event that will, when signaled, trigger the gsi IRQ.
        auto unregister_irqfd(vmm::types::EventFd, uint32_t gsi) const -> void;

        // Sets the GSI routing table entries, overwriting previous entries.
        //
        // See the documentation for KVM_SET_GSI_ROUTING.
        template<typename T,
                 typename=std::enable_if_t<std::is_same_v<typename T::value_type,
                                                          kvm_irq_routing_entry>>>
        auto gsi_routing(T &table) const -> void
        {
            m_fd.ioctl(KVM_SET_GSI_ROUTING, table.data());
        }

        // Sets the level of a GSI input to the interrupt controller model in
        // the kernel.
        //
        // See the documentation for KVM_IRQ_LINE.
        auto set_irq_line(const uint32_t irq, IrqLevel) const -> void;

        // Directly injects a MSI message. Returns > 0 when the MSI is
        // delivered and 0 when the guest blocked the MSI.
        //
        // See the documentation for KVM_SIGNAL_MSI.
        auto signal_msi(const kvm_msi&) const -> int;
#endif

#if defined(__i386__) || defined(__x86_64__)
        // Defines which vcpu is the Bootstrap Processor (BSP).
        //
        // If a vCPU exists for a VM before this is called, the call will fail.
        //
        // See the documentation for KVM_SET_BOOT_CPU_ID.
        auto set_bsp(int vcpu_id) const -> void;

        // Reads the state of a kernel interrupt controller into a buffer
        // provided by the caller.
        //
        // See the documentation for KVM_GET_IRQCHIP.
        auto get_irqchip(kvm_irqchip&) const -> void;

        // Sets the state of a kernel interrupt controller from a buffer
        // provided by the caller.
        //
        // See the documentation for KVM_SET_IRQCHIP.
        auto set_irqchip(const kvm_irqchip&) const -> void;

        // Gets the current timestamp of kvmclock as seen by the current guest.
        //
        // See the documentation for KVM_GET_CLOCK.
        [[nodiscard]] auto get_clock() const -> kvm_clock_data;

        // Sets the current timestamp of kvmclock.
        //
        // See the documentation for KVM_SET_CLOCK.
        auto set_clock(const kvm_clock_data&) const -> void;

        // Sets the address of a three-page region in a VM's address space.
        //
        // See the documentation for KVM_SET_TSS_ADDR.
        auto set_tss_address(unsigned long address) const -> void;

        // Creates an in-kernel device model for the i8254 PIT.
        //
        // See the documentation for KVM_CREATE_PIT2.
        auto create_pit2(uint32_t flags=0) const -> void;

        // Retrieves the state of the in-kernel PIT model.
        //
        // See the documentation for KVM_GET_PIT2.
        auto pit2() const -> kvm_pit_state2;

        // Sets the state of the in-kernel PIT model.
        //
        // See the documentation for KVM_SET_PIT2.
        auto set_pit2(const kvm_pit_state2&) const -> void;
#endif

#if defined(__arm__) || defined(__aarch64__)
        // Returns the preferred CPU target which can be emulated by KVM
        // on underlying host.
        //
        // See documentation for KVM_ARM_PREFERRED_TARGET.
        [[nodiscard]] auto preferred_target() const -> kvm_vcpu_init;
#endif
};

}  // namespace vmm::kvm::detail
