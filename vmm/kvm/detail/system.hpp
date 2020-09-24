/*
 * system.hpp - KVM ioctls
 */

#pragma once

#include <system_error> // error_code, system_category, system_error
#include <type_traits> // enable_if_t, is_same_v

#include <fcntl.h> // open
#include <sys/types.h> // open
#include <sys/stat.h> // open

#include "vmm/kvm/detail/base.hpp"
#include "vmm/kvm/detail/macros.hpp"
#include "vmm/kvm/detail/types.hpp"

namespace vmm::kvm::detail {

class vm;

class system {
    private:
        KvmFd fd_;

        [[nodiscard]] auto create_vm(unsigned int machine_type) const -> int;

        /**
         * Creates and returns a populated KVM list (e.g., MSR features, cpuids).
         */
        template<template<std::size_t N> typename List, std::size_t N, unsigned int Ioctl>
        [[nodiscard]] auto get_list() const -> List<N> {
            auto l = List<N>{};
            fd_.ioctl(Ioctl, l.data());
            return l;
        }
    public:
        system() : fd_{open()} {}

        /**
         * Constructs a kvm object from a file descriptor.
         *
         * The passed file descriptor should have O_RDWR permissions for things
         * to work. It is also encouraged to have O_CLOEXEC set, though it may
         * be omitted as needed.
         *
         * Examples
         * ========
         * See kvm::system::open().
         */
        system(int fd) : fd_{fd} {}

        system(const system& other) = delete;
        system(system&& other) = default;
        auto operator=(const system& other) -> system& = delete;
        auto operator=(system&& other) -> system& = default;

        /**
         * Opens /dev/kvm and returns a file descriptor.
         *
         * Use cases for opening /dev/kvm without O_CLOEXEC typically involve
         * using or passing the resulting file handle to another process. For
         * example, a program may open /dev/kvm only to exec() into another
         * program with seccomp filters.
         *
         * Examples
         * ========
         * ```
         * #include <vmm/kvm.hpp>
         *
         * auto fd = vmm::kvm::system::open(false);
         * auto kvm = vmm::kvm::system{fd};
         * ```
         */
        [[nodiscard]] static auto open(bool cloexec=true) -> int {
            const auto fd = ::open("/dev/kvm", cloexec ? O_RDWR | O_CLOEXEC : O_RDWR);

            if (fd < 0) {
                VMM_THROW(std::system_error(errno, std::system_category()));
            }

            return fd;
        }

        // General routines
        [[nodiscard]] auto api_version() const -> unsigned int;

        // Creation routines
        [[nodiscard]] auto vm(unsigned int machine_type=0) const -> vmm::kvm::detail::vm;

        // Control routines
        [[nodiscard]] auto check_extension(unsigned int cap) const -> unsigned int;
        [[nodiscard]] auto vcpu_mmap_size() const -> std::size_t;
        [[nodiscard]] auto host_ipa_limit() const -> unsigned int;

        [[nodiscard]] auto msr_index_list() const -> MsrList<MAX_IO_MSRS>;
        [[nodiscard]] auto msr_feature_list() const -> MsrList<MAX_IO_MSRS_FEATURES>;

        /**
         * Reads the values of MSR-based features available for VMs. Returns the
         * number of successfully read values.
         *
         * Examples
         * ========
         * ```
         * #include <vmm/kvm.hpp>
         *
         * auto kvm = vmm::kvm::system{};
         * auto entry = kvm_msr_entry{0x174};
         * auto msrs = vmm::kvm::Msrs<1>{entry};
         * auto nmsrs = kvm.get_msrs(msrs);
         * ```
         *
         * ```
         * #include <vector>
         * #include <vmm/kvm.hpp>
         *
         * auto kvm = vmm::kvm::system{};
         * auto msr_list = kvm.msr_feature_list();
         * auto entries = std::vector<kvm_msr_entry>{};
         *
         * for (auto msr : msr_list) {
         *     auto entry = kvm_msr_entry{msr};
         *     entries.push_back(entry);
         * }
         *
         * auto msrs = vmm::kvm::Msrs{entries};
         * auto nmsrs = kvm.read_msrs(msrs);
         * ```
         */
        //template<typename T, typename = std::enable_if_t<std::is_same_v<T, Msrs>>>
        template<typename T, typename = std::enable_if_t<std::is_same_v<typename T::value_type, kvm_msr_entry>>>
        auto read_msrs(T &msrs) const -> unsigned int {
            return fd_.ioctl(KVM_GET_MSRS, msrs.data());
        }

        [[nodiscard]] auto supported_cpuids() const -> Cpuids<MAX_CPUID_ENTRIES>;
        [[nodiscard]] auto emulated_cpuids() const -> Cpuids<MAX_CPUID_ENTRIES>;
};

}  // namespace vmm::kvm::detail
