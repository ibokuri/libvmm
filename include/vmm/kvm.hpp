/*
 * kvm.hpp - KVM ioctls
 */

#pragma once

#include <filesystem>
#include <memory>

#include "utils.hpp"

#include <fcntl.h>
#include <linux/kvm.h>
#include <sys/stat.h>
#include <unistd.h>

/**
 * Size of reserved custom MSR range (0x4b564d00 to 0x4b564dff)
 */
#define MAX_IO_MSRS 256

/**
 * Size of `msr_based_features_all` array in linux/arch/x86/kvm/x86.c
 */
#define MAX_IO_MSRS_FEATURES 22

namespace vmm::kvm {
    namespace fs = std::filesystem;

    class MsrList {
        private:
            std::unique_ptr<kvm_msr_list, void(*)(kvm_msr_list*)> list_;
        protected:
            /**
             * Constructs an MSR list with @size possible entries.
             *
             * The relevant struct is as follows:
             *
             *     struct kvm_msr_list {
             *         __u32 nmsrs;
             *         __u32 indices[0];
             *     };
             */
            MsrList(const std::size_t size) : list_{reinterpret_cast<kvm_msr_list*>(new uint32_t[size + 1]),
                                                    [](kvm_msr_list *l){ delete[] reinterpret_cast<uint32_t*>(l); }}
            {
                list_->nmsrs = size;
            }
        public:
            MsrList() : MsrList(MAX_IO_MSRS) {}
            MsrList(MsrList&&) = default;

            kvm_msr_list* data() { return list_.get(); }
            uint32_t nmsrs() { return list_->nmsrs; }
            uint32_t* indices() { return list_->indices; }
    };

    class MsrFeatureList : public MsrList {
        public:
            MsrFeatureList() : MsrList(MAX_IO_MSRS_FEATURES) {}
            MsrFeatureList(MsrFeatureList&&) = default;
    };

    class vm;

    class system final {
        private:
            unsigned int fd_;

            /**
             * Creates a virtual machine and returns a file descriptor.
             *
             * This should only be used indirectly through system::vm().
             */
            auto create_vm() -> unsigned int {
                return utils::ioctl(fd_, KVM_CREATE_VM);
            }
        public:
            system() : fd_{open()} {}

            /**
             * Constructs a kvm object from a file descriptor.
             *
             * The passed file descriptor must have O_RDWR permissions for
             * things to work. It is also encouraged to have O_CLOEXEC set,
             * on the descriptor, though the flag may be omitted as needed.
             *
             * Note that the passed file descriptor is of type unsigned int.
             * As such, users will have to use kvm::system::open() instead of
             * the C-style open() if they want to create a kvm object. This
             * ensures that `fd` is both a valid handle and one that contains
             * a proper amount of permissions for subsequent KVM operations.
             *
             * # Safety
             *
             * Ownership of `fd` is transferred over to the created Kvm object.
             *
             * # Examples
             *
             * See kvm::system::open().
             */
            explicit system(unsigned int fd) noexcept : fd_{fd} {};

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
             * auto fd {kvm::system::open()};
             * kvm::system kvm{fd};
             * ```
             */
            static auto open(const bool cloexec=true) -> unsigned int {
                const auto fd {::open("/dev/kvm", cloexec ? O_RDWR | O_CLOEXEC : O_RDWR)};
                if (fd < 0)
                    throw fs::filesystem_error{"open()", "/dev/kvm", std::error_code{errno, std::system_category()}};
                return fd;
            }

            /**
             * Returns the KVM API version.
             *
             * # Examples
             *
             * ```
             * #include <vmm/kvm.hpp>
             *
             * kvm::system kvm;
             * if (kvm.api_version() != 12) throw TODO;
             * ```
             */
            auto api_version() -> unsigned int {
                return utils::ioctl(fd_, KVM_GET_API_VERSION);
            }

            /**
             * Returns the size of the shared memory region used by the KVM_RUN
             * ioctl to communicate with userspace.
             *
             * # Examples
             *
             * ```
             * #include <vmm/kvm.hpp>
             *
             * kvm::system kvm;
             * TODO
             * ```
             */
            auto vcpu_mmap_size() -> unsigned int {
                return utils::ioctl(fd_, KVM_GET_VCPU_MMAP_SIZE);
            }

            /**
             * Returns an MSR list of supported guest MSRs.
             *
             * MSR values can be read by passing the MSR indices within the
             * returned MsrList to the KVM_GET_MSRS ioctl (vcpu).
             */
            auto msr_index_list() -> MsrList {
                MsrList msr_list;
                utils::ioctl(fd_, KVM_GET_MSR_INDEX_LIST, msr_list.data());
                return msr_list;
            }

            /**
             * Returns an MSR list containing available MSR-based features.
             *
             * The values of MSR features stored in the returned
             * MsrFeatureList can be read by passing the features to the
             * KVM_GET_MSRS ioctl (system).
             */
            auto msr_feature_index_list() -> MsrFeatureList {
                MsrFeatureList msr_feature_list;
                utils::ioctl(fd_, KVM_GET_MSR_FEATURE_INDEX_LIST, msr_feature_list.data());
                return msr_feature_list;
            }

            auto vm() -> vm;

            ~system() noexcept { close(fd_); }
    };

    class vm final {
        private:
            unsigned int fd_;
            unsigned int mmap_size_;

            /* To prevent users from constructing VMs using arbitrary file
             * descriptors and to restrict construction to existing KVM system
             * objects, system::vm() should be the only function that calls
             * this method.  */
            vm(const unsigned int fd, const unsigned int mmap_size) : fd_{fd}, mmap_size_{mmap_size} {}
            friend vm system::vm();
        public:
            ~vm() noexcept { close(fd_); }
    };

    /**
     * Create a virtual machine.
     *
     * This function will also initialize the size of the vcpu mmap area with
     * the KVM_GET_VCPU_MMAP_SIZE ioctl's result.
     *
     * # Examples
     *
     * ```
     * #include <vmm/kvm.hpp>
     *
     * kvm::system kvm;
     * kvm::vm {kvm.vm()};
     * ```
     */
    auto system::vm() -> vmm::kvm::vm {
        const auto mmap_size {vcpu_mmap_size()};
        const auto fd {create_vm()};
        return vmm::kvm::vm{fd, mmap_size};
    }
}
