/*
 * system.hpp - KVM ioctls
 */

#pragma once

#include <filesystem>
#include <memory>

#include "vmm/kvm/detail/types.hpp"
#include "vmm/utility/utility.hpp"

namespace vmm::kvm::detail {

class vm;

class system {
    private:
        unsigned int fd_;
        bool closed_;

        auto create_vm(unsigned int machine_type) -> unsigned int;
    public:
        system() : fd_{open()}, closed_{false} {}

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
         *
         * Ownership of `fd` is transferred over to the created Kvm object.
         *
         * Examples
         * ========
         *
         * See kvm::system::open().
         */
        explicit system(unsigned int fd) noexcept : fd_{fd}, closed_{false} {};
        ~system() noexcept;

        system(const system& other) = delete;
        system(system&& other) = default;
        system& operator=(const system& other) = delete;
        system& operator=(system& other) = default;

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
         *
         * ```
         * #include <vmm/kvm.hpp>
         *
         * auto fd {kvm::system::open(false)};
         * kvm::system kvm{fd};
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
        auto close() -> void;

        auto api_version() -> unsigned int;
        auto check_extension(unsigned int cap) -> unsigned int;
        auto vcpu_mmap_size() -> unsigned int;
        auto host_ipa_limit() -> unsigned int;
        auto supported_cpuid() -> Cpuid;
        auto emulated_cpuid() -> Cpuid;
        auto msr_index_list() -> MsrIndexList;
        auto msr_feature_list() -> MsrFeatureList;
        auto msr_feature_index_list() -> MsrFeatureList;
        auto read_msrs(Msrs& msrs) -> unsigned int;
        auto vm(unsigned int machine_type) -> vmm::kvm::detail::vm;
        auto vm() -> vmm::kvm::detail::vm;
};

}  // namespace vmm::kvm::detail
