/*
 * system.hpp - KVM ioctls
 */

#pragma once

#include "vmm/kvm/detail/base.hpp"
#include "vmm/kvm/detail/types.hpp"

#include <filesystem>
#include <memory>
#include <fcntl.h>
#include <unistd.h>

namespace vmm::kvm::detail {

class vm;

class system : public KvmIoctl {
    private:
        auto create_vm(unsigned int machine_type) -> unsigned int;
    public:
        system() : KvmIoctl(open()) {}

        /**
         * Constructs a kvm object from a file descriptor.
         *
         * The passed file descriptor should have O_RDWR permissions for things
         * to work. It is also encouraged to have O_CLOEXEC set, though it may
         * be omitted as needed.
         *
         * Note that the passed file descriptor is of type unsigned int. As
         * such, users are encouraged use `kvm::system::open()` to create a
         * system object unless they're willing to do casts on a C-style
         * open(). This provides a bit more assurance that the handle used
         * byt the system object will be a valid, proper kvm handle.
         *
         * Safety
         * ======
         * Ownership of `fd` is transferred over to the created Kvm object.
         *
         * Examples
         * ========
         * See kvm::system::open().
         */
        explicit system(unsigned int fd) noexcept : KvmIoctl(fd) {};

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
        static auto open(const bool cloexec=true) -> unsigned int {
            const auto fd {::open("/dev/kvm", cloexec ? O_RDWR | O_CLOEXEC : O_RDWR)};
            if (fd < 0)
                throw std::filesystem::filesystem_error{
                    "open()",
                    "/dev/kvm",
                    std::error_code{errno, std::system_category()}
                };
            return fd;
        }

        // General routines
        auto api_version() -> unsigned int;

        // Creation routines
        auto vm(unsigned int machine_type = 0) -> vmm::kvm::detail::vm;

        // Control routines
        auto check_extension(const unsigned int cap) -> unsigned int;
        auto vcpu_mmap_size() -> unsigned int;
        auto host_ipa_limit() -> unsigned int;
        auto supported_cpuids() -> CpuidList;
        auto emulated_cpuids() -> CpuidList;
        auto supported_cpuids(CpuidList& cpuids) -> void;
        auto emulated_cpuids(CpuidList& cpuids) -> void;
        auto msr_index_list() -> MsrIndexList;
        auto msr_feature_list() -> MsrFeatureList;
        auto msr_index_list(MsrIndexList& msrs) -> void;
        auto msr_feature_list(MsrFeatureList& msrs) -> void;
        auto get_msrs(MsrList& msrs) -> unsigned int;
};

}  // namespace vmm::kvm::detail
