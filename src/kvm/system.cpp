/*
 * kvm.cpp - KVM ioctls
 */

#include "../../include/vmm/kvm/system.hpp"
#include "../../include/vmm/kvm/vm.hpp"
#include "../../include/vmm/kvm/types.hpp"
#include "../../include/vmm/utils/utils.hpp"

#include <linux/kvm.h>
#include <sys/stat.h>

namespace vmm::kvm {
    /**
     * Creates a virtual machine and returns a file descriptor.
     *
     * This should only be used indirectly through system::vm().
     */
    auto system::create_vm() -> unsigned int {
        return utils::ioctl(fd_, KVM_CREATE_VM);
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
     * if (kvm.api_version() != KVM_API_VERSION)
     *     throw;
     * ```
     */
    auto system::api_version() -> unsigned int {
        return utils::ioctl(fd_, KVM_GET_API_VERSION);
    }

    /**
     * Returns the size of the shared memory region used to communicate with
     * userspace by the KVM_RUN ioctl.
     *
     * # Examples
     *
     * ```
     * #include <vmm/kvm.hpp>
     *
     * kvm::system kvm;
     * auto mmap_size {kvm.vcpu_mmap_size()};
     * TODO
     * ```
     */
    auto system::vcpu_mmap_size() -> unsigned int {
        return utils::ioctl(fd_, KVM_GET_VCPU_MMAP_SIZE);
    }

    /**
     * Returns a list of supported MSRs (host & KVM-specific).
     *
     * # Examples
     *
     * ```
     * #include <vmm/kvm.hpp>
     *
     * kvm::system kvm;
     * kvm::MsrList msr_list {kvm.msr_index_list()};
     * ```
     */
    auto system::msr_index_list() -> MsrList {
        MsrList msr_list;
        utils::ioctl(fd_, KVM_GET_MSR_INDEX_LIST, msr_list.get());
        return msr_list;
    }

    /**
     * Returns a list of MSRs exposing MSR-based CPU features.
     *
     * This can be used, for instance, by a hypervisor to validate requested
     * CPU features.
     *
     * # Examples
     *
     * ```
     * #include <vmm/kvm.hpp>
     *
     * kvm::system kvm;
     * kvm::MsrFeatureList msr_feature_list {kvm.msr_feature_index_list()};
     * ```
     */
    auto system::msr_feature_index_list() -> MsrFeatureList {
        MsrFeatureList msr_feature_list;
        utils::ioctl(fd_, KVM_GET_MSR_FEATURE_INDEX_LIST, msr_feature_list.get());
        return msr_feature_list;
    }

    /**
     * Reads the values of MSR-based features available for a VM.
     *
     * # Examples
     *
     * ```
     * #include <vmm/kvm.hpp>
     *
     * kvm::system kvm;
     * kvm::msrs msrs{TODO};
     * auto nmsrs {kvm.msrs(msrs)};
     * TODO
     * ```
     */
    auto system::msrs() -> unsigned int {
        auto nmsrs {0};

        //utils::ioctl(fd_, KVM_GET_MSRS, );

        return nmsrs;
    }

    /**
     * Creates a virtual machine.
     *
     * This function will also initialize the size of the vCPU mmap area with
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
