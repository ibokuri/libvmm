/*
 * vm.hpp - VM ioctls
 */

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

class vm {
    friend vm system::vm(unsigned machine_type) const;

    public:
        vm(const vm& other) = delete;
        vm(vm&& other) = default;
        auto operator=(const vm& other) -> vm& = delete;
        auto operator=(vm&& other) -> vm& = default;

        // Creation routines
        [[nodiscard]] auto vcpu(unsigned vcpu_id) const -> vmm::kvm::detail::vcpu;
        [[nodiscard]] auto device(uint32_t type, uint32_t flags=0) const -> vmm::kvm::detail::device;

        // Control routines
        [[nodiscard]] auto check_extension(unsigned cap) const -> unsigned;
        auto set_bsp(unsigned vcpu_id) const -> void;
        auto memslot(kvm_userspace_memory_region) const -> void;
        auto irqchip() const -> void;
        auto get_irqchip(kvm_irqchip&) const -> void;
        auto set_irqchip(kvm_irqchip const&) const -> void;
        [[nodiscard]] auto get_clock() const -> kvm_clock_data;
        auto set_clock(kvm_clock_data&) const -> void;

        /**
         * Sets the GSI routing table entries, overwriting previous entries.
         *
         * See the documentation for `KVM_SET_GSI_ROUTING`.
         */
        template<typename T,
                 typename=std::enable_if_t<std::is_same_v<typename T::value_type,
                                                          kvm_irq_routing_entry>>>
        auto gsi_routing(T &table) const -> void {
            m_fd.ioctl(KVM_SET_GSI_ROUTING, table.data());
        }

        /**
         * Attaches an ioeventfd to a legal pio/mmio address within the guest.
         *
         * A guest write in the registered address will signal the provided
         * event instead of triggering an exit.
         *
         * Examples
         * ========
         * ```
         * #include "vmm/kvm.hpp"
         * #include "vmm/types.hpp"
         *
         * using Pio = vmm::types::IoEventAddress::Pio;
         * using Mmio = vmm::types::IoEventAddress::Mmio;
         * using EventFd = vmm::types::EventFd;
         *
         * auto kvm = vmm::kvm::system{};
         * auto vm = kvm.vm();
         * auto eventfd = EventFd{Em_fdNONBLOCK};
         *
         * vm.attach_ioevent<Pio>(eventfd, 0xf4);
         * vm.attach_ioevent<Mmio>(eventfd, 0x1000);
         * ```
         */
        template<vmm::types::IoEventAddress T>
        auto attach_ioevent(vmm::types::EventFd eventfd, uint64_t addr,
                            uint64_t datamatch=0) const -> void {
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

        /**
         * Detaches an ioeventfd to a legal pio/mmio address within the guest.
         *
         * Examples
         * ========
         * ```
         * #include "vmm/kvm.hpp"
         * #include "vmm/types.hpp"
         *
         * using Pio = vmm::types::IoEventAddress::Pio;
         * using Mmio = vmm::types::IoEventAddress::Mmio;
         * using EventFd = vmm::types::EventFd;
         *
         * auto kvm = vmm::kvm::system{};
         * auto vm = kvm.vm();
         * auto eventfd = EventFd{Em_fdNONBLOCK};
         *
         * vm.attach_ioevent<Pio>(eventfd, 0xf4);
         * vm.attach_ioevent<Mmio>(eventfd, 0x1000, 0x1234);
         *
         * vm.detach_ioevent<Pio>(eventfd, 0xf4);
         * vm.detach_ioevent<Mmio>(eventfd, 0x1000, 0x1234);
         * ```
         */
        template<vmm::types::IoEventAddress T>
        auto detach_ioevent(vmm::types::EventFd eventfd, uint64_t addr,
                            uint64_t datamatch=0) const -> void {
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

        // Convenient routines
        [[nodiscard]] auto mmap_size() const -> std::size_t;
        [[nodiscard]] auto num_vcpus() const -> unsigned;
        [[nodiscard]] auto max_vcpus() const -> unsigned;
        [[nodiscard]] auto num_memslots() const -> unsigned;
    private:
        KvmFd m_fd;
        size_t m_mmap_size;

        vm(int fd, std::size_t mmap_size) noexcept
                : m_fd{fd}, m_mmap_size{mmap_size} {}
};

}  // namespace vmm::kvm::detail
