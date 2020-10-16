//
// vm.hpp - VM ioctls
//

#pragma once

#include <cstddef> // size_t
#include <cstdint> // uint32_t, uint64_t
#include <linux/kvm.h> // kvm_*, KVM_*

#include "vmm/kvm/detail/ioctls/system.hpp"
#include "vmm/kvm/detail/types/file_descriptor.hpp"
#include "vmm/kvm/detail/types/fam_struct.hpp"
#include "vmm/types/event.hpp"

namespace vmm::kvm::detail {

class vcpu;
class device;

class vm
{
    friend vm system::vm(unsigned machine_type) const;

    public:
        vm(const vm& other) = delete;
        vm(vm&& other) = default;
        auto operator=(const vm& other) -> vm& = delete;
        auto operator=(vm&& other) -> vm& = default;

        // Adds a vcpu to a virtual machine.
        //
        // See the documentation for KVM_CREATE_VCPU.
        //
        // Examples
        // ========
        // ```
        // #include <vmm/kvm.hpp>
        //
        // auto kvm = vmm::kvm::system{};
        // auto vm = kvm.vm();
        // auto vcpu = vm.vcpu(0);
        // ```
        [[nodiscard]] auto vcpu(unsigned vcpu_id) const -> vmm::kvm::detail::vcpu;

        // Adds a device to a virtual machine.
        //
        // See the documentation for KVM_CREATE_DEVICE.
        //
        // Examples
        // ========
        // ```
        // #include <vmm/kvm.hpp>
        //
        // auto kvm = vmm::kvm::system{};
        // auto vm = kvm.vm();
        // auto device = vm.device(KVM_DEV_TYPE_VFIO);
        // ```
        [[nodiscard]] auto device(uint32_t type, uint32_t flags=0) const -> vmm::kvm::detail::device;

        // Returns a positive integer if a KVM extension is available; 0
        // otherwise.
        //
        // Based on their initialization, VMs may have different capabilities.
        // Thus, `kvm::vm::check_extension()` is preferred when querying for
        // most capabilities.
        //
        // Examples
        // ========
        // ```
        // #include <cassert>
        // #include <vmm/kvm.hpp>
        //
        // auto kvm = vmm::kvm::system{};
        // auto vm = kvm.vm();
        // assert(vm.check_extension(KVM_CAP_ARM_VM_IPA_SIZE) >= 32);
        // ```
        [[nodiscard]] auto check_extension(unsigned cap) const -> unsigned;

        // Creates, modifies, or deletes a guest physical memory slot.
        //
        // See the documentation for KVM_SET_USER_MEMORY_REGION.
        //
        // Examples
        // ========
        // ```
        // #include <vmm/kvm.hpp>
        //
        // auto kvm = vmm::kvm::system{};
        // vmm::kvm::vm vm = kvm.vm();
        // auto mem_region = kvm_userspace_memory_region{
        //     .slot = 0,
        //     .flags = 0,
        //     .guest_phys_addr = 0x10000,
        //     .memory_size = 0x10000,
        //     .userspace_addr = 0,
        // };
        //
        // vm.memslot(mem_region);
        // ```
        auto memslot(kvm_userspace_memory_region) const -> void;

        // Attaches an ioeventfd to a legal pio/mmio address within the guest.
        //
        // A guest write in the registered address will signal the provided
        // event instead of triggering an exit.
        //
        // Examples
        // ========
        // ```
        // #include "vmm/kvm.hpp"
        // #include "vmm/types.hpp"
        //
        // using Pio = vmm::types::IoEventAddress::Pio;
        // using Mmio = vmm::types::IoEventAddress::Mmio;
        // using EventFd = vmm::types::EventFd;
        //
        // auto kvm = vmm::kvm::system{};
        // auto vm = kvm.vm();
        // auto eventfd = EventFd{EFD_NONBLOCK};
        //
        // vm.attach_ioevent<Pio>(eventfd, 0xf4);
        // vm.attach_ioevent<Mmio>(eventfd, 0x1000);
        // ```
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
                .datamatch = datamatch,
                .addr = addr,
                .len = sizeof(uint64_t),
                .fd = eventfd.fd(),
                .flags = flags
            };

            m_fd.ioctl(KVM_IOEVENTFD, &ioeventfd);
        }

        // Detaches an ioeventfd to a legal pio/mmio address within the guest.
        //
        // Examples
        // ========
        // ```
        // #include "vmm/kvm.hpp"
        // #include "vmm/types.hpp"
        //
        // using Pio = vmm::types::IoEventAddress::Pio;
        // using Mmio = vmm::types::IoEventAddress::Mmio;
        // using EventFd = vmm::types::EventFd;
        //
        // auto kvm = vmm::kvm::system{};
        // auto vm = kvm.vm();
        // auto eventfd = EventFd{EFD_NONBLOCK};
        //
        // vm.attach_ioevent<Pio>(eventfd, 0xf4);
        // vm.attach_ioevent<Mmio>(eventfd, 0x1000, 0x1234);
        //
        // vm.detach_ioevent<Pio>(eventfd, 0xf4);
        // vm.detach_ioevent<Mmio>(eventfd, 0x1000, 0x1234);
        // ```
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
                .datamatch = datamatch,
                .addr = addr,
                .len = sizeof(uint64_t),
                .fd = eventfd.fd(),
                .flags = flags
            };

            m_fd.ioctl(KVM_IOEVENTFD, &ioeventfd);
        }

        // Returns KVM_RUN's shared memory region size.
        [[nodiscard]] auto mmap_size() const -> std::size_t;

        // Returns the recommended number for max_vcpus.
        [[nodiscard]] auto num_vcpus() const -> unsigned;

        // Returns the maximum possible value for max_vcpus.
        [[nodiscard]] auto max_vcpus() const -> unsigned;

        // Returns the maximum number of allowed memory slots for a VM.
        [[nodiscard]] auto num_memslots() const -> unsigned;

#if defined(__i386__) || defined(__x86_64__)
        // Defines which vcpu is the Bootstrap Processor (BSP).
        //
        // The KVM_SET_BOOT_CPU_ID ioctl must be called before any vcpus are
        // created for a VM, otherwise the call will fail.
        //
        // See the documentation for KVM_SET_BOOT_CPU_ID.
        //
        // Examples
        // ========
        // ```
        // #include <vmm/kvm.hpp>
        //
        // auto kvm = vmm::kvm::system{};
        // auto vm = kvm.vm();
        //
        // if (vm.check_extension(KVM_CAP_SET_BOOT_CPU_ID) > 0)
        //     throw;
        //
        // vm.set_bsp(0);
        // ```
        auto set_bsp(unsigned vcpu_id) const -> void;

        // Reads the state of a kernel interrupt controller into a buffer
        // provided by the caller.
        //
        // See the documentation for `KVM_GET_IRQCHIP`.
        //
        // Examples
        // ========
        // ```
        // #include <vmm/kvm.hpp>
        //
        // auto kvm = vmm::kvm::system{};
        // auto vm = kvm.vm();
        // auto irqchip = kvm_irqchip{ .chip_id = KVM_IRQCHIP_PIC_MASTER };
        //
        // vm.irqchip();
        // vm.get_irqchip(irqchip);
        // ```
        auto get_irqchip(kvm_irqchip&) const -> void;

        // Sets the state of a kernel interrupt controller from a buffer
        // provided by the caller.
        //
        // See the documentation for `KVM_SET_IRQCHIP`.
        //
        //
        // Examples
        // ========
        // ```
        // #include <vmm/kvm.hpp>
        //
        // auto kvm = vmm::kvm::system{};
        // auto vm = kvm.vm();
        // auto irqchip = kvm_irqchip{
        //     .chip_id = KVM_IRQCHIP_PIC_MASTER,
        //     .chip.pic.irq_base = 99
        // };
        //
        // vm.irqchip();
        // vm.set_irqchip(irqchip);
        // ```
        auto set_irqchip(const kvm_irqchip&) const -> void;

        // Gets the current timestamp of kvmclock as seen by the current guest.
        //
        // See the documentation for `KVM_GET_CLOCK`.
        //
        // Examples
        // ========
        // ```
        // #include <vmm/kvm.hpp>
        //
        // auto kvm = vmm::kvm::system{};
        // auto vm = kvm.vm();
        // auto clock = vm.get_clock();
        // ```
        [[nodiscard]] auto get_clock() const -> kvm_clock_data;

        // Sets the current timestamp of kvmclock.
        //
        // See the documentation for `KVM_SET_CLOCK`.
        //
        // Examples
        // ========
        // ```
        // #include <vmm/kvm.hpp>
        //
        // auto kvm = vmm::kvm::system{};
        // auto vm = kvm.vm();
        // auto clock = kvm_clock_data{ .clock = 10 };
        //
        // vm.set_clock(&clock);
        // ```
        auto set_clock(kvm_clock_data&) const -> void;
#endif

#if defined(__i386__) || defined(__x86_64__)  || \
    defined(__arm__)  || defined(__aarch64__)
        auto set_irq_line(const uint32_t irq, bool active) const -> void;
#endif

#if defined(__i386__) || defined(__x86_64__)  || \
    defined(__arm__)  || defined(__aarch64__) || \
    defined(__s390__)
        // Creates an interrupt controller model in the kernel
        //
        // See the documentation for `KVM_CREATE_IRQCHIP`.
        //
        // Examples
        // ========
        // ```
        // #include <vmm/kvm.hpp>
        //
        // auto kvm = vmm::kvm::system{};
        // auto vm = kvm.vm();
        // vm.irqchip();
        // ```
        auto irqchip() const -> void;
        auto register_irqfd(vmm::types::EventFd, uint32_t gsi) const -> void;
        auto unregister_irqfd(vmm::types::EventFd, uint32_t gsi) const -> void;

        // Sets the GSI routing table entries, overwriting previous entries.
        //
        // See the documentation for `KVM_SET_GSI_ROUTING`.
        template<typename T,
                 typename=std::enable_if_t<std::is_same_v<typename T::value_type,
                                                          kvm_irq_routing_entry>>>
        auto gsi_routing(T &table) const -> void
        {
            m_fd.ioctl(KVM_SET_GSI_ROUTING, table.data());
        }
#endif
    private:
        KvmFd m_fd;
        size_t m_mmap_size;

        vm(int fd, std::size_t mmap_size) noexcept
            : m_fd{fd}, m_mmap_size{mmap_size} {}
};

}  // namespace vmm::kvm::detail
