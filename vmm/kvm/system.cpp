/*
 * kvm.cpp - KVM ioctls
 */

#include "vmm/kvm/system.hpp"
#include "vmm/kvm/vm.hpp"
#include "vmm/kvm/types.hpp"
#include "vmm/utility/utility.hpp"

#include <linux/kvm.h>
#include <sys/stat.h>

namespace vmm::kvm {
    /**
     * Returns the KVM API version.
     *
     * # Examples
     *
     * ```
     * #include <cassert.
     * #include <vmm/kvm.hpp>
     *
     * kvm::system kvm;
     * assert(kvm.api_version() == KVM_API_VERSION))
     * ```
     */
    auto system::api_version() -> unsigned int {
        return utility::ioctl(fd_, KVM_GET_API_VERSION);
    }

    /**
     * Returns a positive integer if a KVM extension is available; 0 otherwise.
     *
     * Based on their initialization, different VMs may have different
     * capabilities. Thus, it's encouraged to use kvm::vm::check_extension() to
     * query for capabilities.
     *
     * # Examples
     *
     * ```
     * #include <cassert>
     * #include <vmm/kvm.hpp>
     *
     * kvm::system kvm;
     * assert(kvm.check_extension(KVM_CAP_XEN_HVM) > 0);
     * ```
     */
    auto system::check_extension(unsigned int cap) -> unsigned int {
        return utility::ioctl(fd_, KVM_CHECK_EXTENSION, cap);
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
     * ```
     */
    auto system::vcpu_mmap_size() -> unsigned int {
        return utility::ioctl(fd_, KVM_GET_VCPU_MMAP_SIZE);
    }

    /**
     * Returns a list of supported TODO
     */
    auto system::supported_cpuid() -> Cpuid {
        Cpuid cpuid;
        utility::ioctl(fd_, KVM_GET_SUPPORTED_CPUID, cpuid.get());
        return cpuid;
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
     * kvm::MsrIndexList msr_list {kvm.msr_index_list()};
     *
     * TODO
     * ```
     */
    auto system::msr_index_list() -> MsrIndexList {
        MsrIndexList msr_list;
        utility::ioctl(fd_, KVM_GET_MSR_INDEX_LIST, msr_list.get());
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
     * #include <iostream>
     * #include <vmm/kvm.hpp>
     *
     * kvm::system kvm;
     * kvm::MsrFeatureList msr_list {kvm.msr_feature_list()};
     *
     * TODO: Validate CPU feature requested by VM
     * ```
     */
    auto system::msr_feature_list() -> MsrFeatureList {
        MsrFeatureList msr_list;
        utility::ioctl(fd_, KVM_GET_MSR_FEATURE_INDEX_LIST, msr_list.get());
        return msr_list;
    }

    /**
     * Reads the values of MSR-based features available for VMs. Returns the
     * number of successfully read values.
     *
     * # Examples
     *
     * ```
     * #include <vmm/kvm.hpp>
     *
     * kvm::system kvm;
     * kvm_msr_entry entry{0x174};
     * kvm::Msrs msrs{entry};
     * auto nmsrs {kvm.msrs(msrs)};
     * ```
     *
     * ```
     * #include <vector>
     * #include <vmm/kvm.hpp>
     *
     * kvm::system kvm;
     * kvm::MsrFeatureList msr_list {kvm.msr_feature_list()};
     * std::vector<kvm_msr_entry> entries;
     *
     * for (auto msr : msr_list) {
     *     kvm_msr_entry entry{msr};
     *     entries.push_back(entry);
     * }
     *
     * kvm::Msrs msrs{entries};
     * auto nmsrs {kvm.msrs(msrs)};
     * ```
     */
    auto system::msrs(Msrs& msrs) -> unsigned int {
        return utility::ioctl(fd_, KVM_GET_MSRS, msrs.get());
    }

    /**
     * Creates a virtual machine and returns a file descriptor.
     *
     * This should only be used indirectly through system::vm().
     */
    auto system::create_vm() -> unsigned int {
        return utility::ioctl(fd_, KVM_CREATE_VM);
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

    system::~system() noexcept {
        if (!closed_) {
            try {
                utility::close(fd_);
            }
            catch (std::system_error& e) {
                /* TODO: log error */
            }
        }
    }

    /**
     * Closes the KVM subsystem handle.
     *
     * Use this if you'd like to handle possible failures of `utility::close()`.
     *
     * # Examples
     *
     * ```
     * #include <vmm/kvm.hpp>
     *
     * kvm::system kvm;
     *
     * try {
     *     kvm.close();
     * }
     * catch (std::system_error)
     *     throw;
     * ```
     */
    auto system::close() -> void {
        utility::close(fd_);
        closed_ = true;
    }
}
