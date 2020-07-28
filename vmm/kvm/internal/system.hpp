/*
 * system.hpp - KVM ioctls
 */

#pragma once

#include <filesystem>
#include <memory>

#include "vmm/kvm/internal/types.hpp"
#include "vmm/utility/utility.hpp"

namespace vmm::kvm_internal {
    class vm;

    class system final {
        private:
            unsigned int fd_;
            bool closed_;

            auto create_vm() -> unsigned int;
        public:
            system() : fd_{open()}, closed_{false} {}

            /**
             * Constructs a kvm object from a file descriptor.
             *
             * The passed file descriptor must have O_RDWR permissions for
             * things to work. It is also encouraged to have O_CLOEXEC set,
             * on the descriptor, though the flag may be omitted as needed.
             *
             * Note that the passed file descriptor is of type unsigned int.
             * As such, users must use kvm::system::open() to create a `kvm`
             * object unless they're willing to do cast's on C's open(). This
             * ensures that `fd` is both a valid handle and one that contains a
             * proper amount of permissions for subsequent KVM operations.
             *
             * # Safety
             *
             * Ownership of `fd` is transferred over to the created Kvm object.
             *
             * # Examples
             *
             * See kvm::system::open().
             */
            explicit system(unsigned int fd) noexcept : fd_{fd}, closed_{false} {};
            ~system() noexcept;

            system(const system& other) = default;
            system(system&& other) = default;
            system& operator=(const system& other) = default;
            system& operator=(system& other) = default;

            /**
            * Opens /dev/kvm and returns a file descriptor.
            *
            * Use cases for opening /dev/kvm without O_CLOEXEC typically
            * involve using or passing the resulting file handle to another
            * process. For example, a program may open /dev/kvm only to
            * exec() into another program with seccomp filters that blacklist
            * certain syscalls.
            *
            * # Examples
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
            auto supported_cpuid() -> Cpuid;
            auto msr_index_list() -> MsrIndexList;
            auto msr_feature_list() -> MsrFeatureList;
            auto msr_feature_index_list() -> MsrFeatureList;

            auto msrs(Msrs&) -> unsigned int;

            auto vm() -> vm;
    };
}  // namespace vmm::kvm_internal