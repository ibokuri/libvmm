/*
 * system.hpp - KVM ioctls
 */

#pragma once

#include <system_error> // error_code, system_category, system_error
#include <fcntl.h> // open
#include <sys/types.h> // open
#include <sys/stat.h> // open

#include "vmm/kvm/detail/base.hpp"
#include "vmm/kvm/detail/types.hpp"

namespace vmm::kvm::detail {

class vm;

class system {
    private:
        KvmFd fd_;

        [[nodiscard]] auto create_vm(unsigned int machine_type) -> unsigned int;
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
        system(unsigned int fd) : fd_{fd} {}

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
        [[nodiscard]] static auto open(bool cloexec=true) -> unsigned int {
            const auto fd = ::open("/dev/kvm", cloexec ? O_RDWR | O_CLOEXEC : O_RDWR);

            if (fd < 0) {
                VMM_THROW(std::system_error(errno, std::system_category()));
            }

            return fd;
        }

        // General routines
        [[nodiscard]] auto api_version() -> unsigned int;

        // Creation routines
        [[nodiscard]] auto vm(unsigned int machine_type = 0) -> vmm::kvm::detail::vm;

        // Control routines
        [[nodiscard]] auto check_extension(unsigned int cap) -> unsigned int;
        [[nodiscard]] auto vcpu_mmap_size() -> unsigned int;
        [[nodiscard]] auto host_ipa_limit() -> unsigned int;
        [[nodiscard]] auto supported_cpuids() -> CpuidList;
        [[nodiscard]] auto emulated_cpuids() -> CpuidList;
        auto supported_cpuids(CpuidList& cpuids) -> void;
        auto emulated_cpuids(CpuidList& cpuids) -> void;
        [[nodiscard]] auto msr_index_list() -> MsrIndexList;
        [[nodiscard]] auto msr_feature_list() -> MsrFeatureList;
        auto msr_index_list(MsrIndexList& msrs) -> void;
        auto msr_feature_list(MsrFeatureList& msrs) -> void;
        auto get_msrs(MsrList& msrs) -> unsigned int;
};

}  // namespace vmm::kvm::detail
