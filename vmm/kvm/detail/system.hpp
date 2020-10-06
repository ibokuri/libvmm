/*
 * system.hpp - KVM ioctls
 */

#pragma once

#include <cstddef> // size_t
#include <system_error> // error_code, system_category, system_error
#include <type_traits> // enable_if_t, is_same_v

#include <fcntl.h> // open
#include <sys/stat.h> // open
#include <sys/types.h> // open
#include <linux/kvm.h> // kvm_*, KVM_*

#include "vmm/kvm/detail/macros.hpp"
#include "vmm/kvm/detail/types.hpp"
#include "vmm/types/detail/exceptions.hpp"

namespace vmm::kvm::detail {

class vm;

class system {
    public:
        system()
            : fd_{open()} {}

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
        explicit system(int fd)
            : fd_{fd} {}

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
            const auto fd = ::open("/dev/kvm",
                                   cloexec ? O_RDWR | O_CLOEXEC : O_RDWR);

            if (fd < 0)
                VMM_THROW(std::system_error(errno, std::system_category()));

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

        // TODO: Allocator variants for FAM struct methods

        /**
         * Returns a list of host-supported and kvm-specific MSRs.
         *
         * Examples
         * ========
         * ```
         * #include <iostream>
         * #include <vmm/kvm.hpp>
         *
         * auto kvm = vmm::kvm::system{};
         * auto msr_list = kvm.msr_index_list();
         *
         * for (auto idx : msr_list)
         *     std::cout << "index: " << idx << std::endl;
         * ```
         */
        template<std::size_t N=MAX_IO_MSRS>
        [[nodiscard]] auto msr_index_list() const -> MsrList<N> {
            auto msrs = MsrList<N>{};
            fd_.ioctl(KVM_GET_MSR_INDEX_LIST, msrs.data());
            return msrs;
        }

        /**
         * Returns a list of MSRs exposing MSR-based CPU features.
         *
         * Examples
         * ========
         * ```
         * #include <iostream>
         * #include <vmm/kvm.hpp>
         *
         * auto kvm = vmm::kvm::system{};
         * auto msr_list = kvm.msr_feature_list();
         *
         * for (auto idx : msr_list)
         *     std::cout << "index: " << idx << std::endl;
         * ```
         */
        template<std::size_t N=MAX_IO_MSRS_FEATURES>
        [[nodiscard]] auto msr_feature_list() const -> MsrList<N> {
            auto msrs = MsrList<N>{};
            fd_.ioctl(KVM_GET_MSR_FEATURE_INDEX_LIST, msrs.data());
            return msrs;
        }

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
         *     entries.push_back(kvm_msr_entry{entry});
         * }
         *
         * auto msrs = vmm::kvm::Msrs<MAX_IO_MSRS_FEATURES>{entries};
         * auto nmsrs = kvm.read_msrs(msrs);
         * ```
         */
        template<typename T,
                 typename = std::enable_if_t<std::is_same_v<typename T::value_type,
                                                            kvm_msr_entry>>>
        auto read_msrs(T &msrs) const -> unsigned int {
            return fd_.ioctl(KVM_GET_MSRS, msrs.data());
        }

        /**
         * Returns a list of host- and kvm-supported x86 cpuid features.
         *
         * In x86, the CPU identification (CPUID) instruction is a supplementary
         * instruction allowing software to discover details of the processor. A
         * program can use the CPUID to determine processor type and whether certain
         * features are implemented.
         *
         * Examples
         * ========
         * ```
         * #include <vmm/kvm.hpp>
         *
         * auto kvm = vmm::kvm::system{};
         * auto cpuids = kvm.supported_cpuids();
         *
         * // Print CPU's manufacturer ID string
         * TODO
         * ```
         */
        template<std::size_t N=MAX_CPUID_ENTRIES>
        [[nodiscard]] auto supported_cpuids() const -> Cpuids<N> {
            auto cpuids = Cpuids<N>{};
            fd_.ioctl(KVM_GET_SUPPORTED_CPUID, cpuids.data());
            return cpuids;
        }

        /**
         * Returns a list of kvm-emulated x86 cpuid features.
         *
         * Examples
         * ========
         * ```
         * #include <vmm/kvm.hpp>
         *
         * auto kvm = vmm::kvm::system{};
         * auto cpuids = kvm.emulated_cpuids();
         * ```
         */
        template<std::size_t N=MAX_CPUID_ENTRIES>
        [[nodiscard]] auto emulated_cpuids() const -> Cpuids<N> {
            auto cpuids = Cpuids<N>{};
            fd_.ioctl(KVM_GET_EMULATED_CPUID, cpuids.data());
            return cpuids;
        }
    private:
        KvmFd fd_;

        [[nodiscard]] auto create_vm(unsigned int machine_type) const -> int;
};

}  // namespace vmm::kvm::detail
