/*
 * system.cpp - KVM ioctls
 */

#include "vmm/kvm/detail/system.hpp"
#include "vmm/kvm/detail/vm.hpp"
#include "vmm/utility/utility.hpp"

#include <linux/kvm.h>
#include <sys/stat.h>

namespace vmm::kvm::detail {

/**
 * Returns the kvm API version.
 *
 * Applications should refuse to run if a value other than 12 is returned.
 *
 * Examples
 * ========
 * ```
 * #include <cassert>
 * #include <vmm/kvm.hpp>
 *
 * auto kvm = vmm::kvm::system{};
 * assert(kvm.api_version() == KVM_API_VERSION))
 * ```
 */
auto system::api_version() -> unsigned int {
    return fd_.ioctl(KVM_GET_API_VERSION);
}

/**
 * Returns a file descriptor associated with a newly created VM.
 *
 * This method should only be used by `system::vm()`.
 */
auto system::create_vm(unsigned int machine_type) -> unsigned int {
     return fd_.ioctl(KVM_CREATE_VM, machine_type);
}

/**
 * Creates and returns a virtual machine (with a custom machine type).
 *
 * The VM's vcpu mmap area will be initialized with the KVM_GET_VCPU_MMAP_SIZE
 * ioctl's result.
 *
 * By default, the physical address size for a VM (IPA Size limit) on AArch64
 * is limited to 40-bits. However, this limit can be configured if the host
 * supports the KVM_CAP_ARM_VM_IPA_SIZE extension. When supported, use
 * KVM_VM_TYPE_ARM_IPA_SIZE(IPA_Bits) to set the size in the machine type
 * identifier, where IPA_Bits is the maximum width of any physical address used
 * by the VM.
 *
 * Examples
 * ========
 * ```
 * #include <vmm/kvm.hpp>
 *
 * auto kvm = vmm::kvm::system{};
 * auto vm = kvm.vm(KVM_VM_TYPE_ARM_IPA_SIZE(48);
 * ```
 */
auto system::vm(unsigned int machine_type) -> vmm::kvm::detail::vm {
    return vmm::kvm::detail::vm{create_vm(machine_type), vcpu_mmap_size()};
}

/**
 * Returns a positive integer if a KVM extension is available; 0 otherwise.
 *
 * Based on their initialization, VMs may have different capabilities. Thus,
 * `kvm::vm::check_extension()` is preferred when querying for most
 * capabilities.
 *
 * Examples
 * ========
 * ```
 * #include <cassert>
 * #include <vmm/kvm.hpp>
 *
 * auto kvm = vmm::kvm::system{};
 * assert(kvm.check_extension(KVM_CAP_ARM_VM_IPA_SIZE) >= 32);
 * ```
 */
auto system::check_extension(unsigned int cap) -> unsigned int {
    return fd_.ioctl(KVM_CHECK_EXTENSION, cap);
}

/**
 * Returns the size of the memory region used by the KVM_RUN ioctl to
 * communicate CPU information to userspace.
 *
 * Each vcpu has an associated `kvm_run` struct for communicating information
 * about the CPU between kernel and userspace. In particular, whenever hardware
 * virtualization stops (called a VM-exit), the `kvm_run` struct will contain
 * information about why it stopped. We map this structure into userspace via
 * mmap(), but we need to know beforehand how much memory to map. We get that
 * information with the KVM_GET_VCPU_MMAP_SIZE ioctl.
 *
 * Note that the mmap size typically exceeds that of the `kvm_run` struct since
 * the kernel will also use that space to store other transient structures that
 * kvm_run may point to.
 *
 * Examples
 * ========
 * ```
 * #include <vmm/kvm.hpp>
 *
 * auto kvm = vmm::kvm::system{};
 * auto mmap_size = kvm.vcpu_mmap_size();
 * ```
 */
auto system::vcpu_mmap_size() -> unsigned int {
    return fd_.ioctl(KVM_GET_VCPU_MMAP_SIZE);
}

/* Returns the IPA size for a VM; 0 if the capability isn't available.
 *
 * On AArch64, a guest OS has a set of translation tables that map from the
 * virtual address space to what it thinks is the physical address space, also
 * called the Intermediate Physical Address (IPA) space. However, addresses in
 * the IPA space undergo a second translation into the real physical address
 * space by the hypervisor.
 *
 * Architectures
 * =============
 * aarch64
 *
 * Examples
 * ========
 * ```
 * #include <vmm/kvm.hpp>
 *
 * auto kvm = vmm::kvm::system{}};
 * auto ipa_size {kvm.host_ipa_limit()};
 * ```
 */
auto system::host_ipa_limit() -> unsigned int {
    return check_extension(KVM_CAP_ARM_VM_IPA_SIZE);
}

/**
 * Returns a list of host- and kvm-supported x86 cpuid features.
 *
 * In x86, the CPU identification (CPUID) instruction is a supplementary
 * instruction allowing software to discover details of the processor. A
 * program can use the CPUID to determine processor type and whether certain
 * features are implemented.
 *
 * Architectures
 * =============
 * x86
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
auto system::supported_cpuids() -> CpuidList {
    auto cpuids = CpuidList{MAX_CPUID_ENTRIES};
    supported_cpuids(cpuids);
    return cpuids;
}

/**
 * Obtains a list of host- and kvm-supported x86 cpuid features.
 *
 * Architectures
 * =============
 * x86
 *
 * Examples
 * ========
 * ```
 * #include <vmm/kvm.hpp>
 *
 * auto kvm = vmm::kvm::system{};
 * auto cpuids = vmm::kvm::CpuidList{MAX_CPUID_ENTRIES};
 * kvm.supported_cpuids(cpuids);
 * ```
 */
auto system::supported_cpuids(CpuidList& cpuids) -> void {
    fd_.ioctl(KVM_GET_SUPPORTED_CPUID, cpuids.get());
}

/**
 * Returns a list of kvm-emulated x86 cpuid features.
 *
 * Architectures
 * =============
 * x86
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
auto system::emulated_cpuids() -> CpuidList {
    auto cpuids = CpuidList{MAX_CPUID_ENTRIES};
    emulated_cpuids(cpuids);
    return cpuids;
}

/**
 * Obtains a list of kvm-emulated x86 cpuid features.
 *
 * Architectures
 * =============
 * x86
 *
 * Examples
 * ========
 * ```
 * #include <vmm/kvm.hpp>
 *
 * auto kvm = vmm::kvm::system{};
 * auto cpuids = vmm::kvm::CpuidList{MAX_CPUID_ENTRIES};
 * kvm.emulated_cpuids(cpuids);
 * ```
 */
auto system::emulated_cpuids(CpuidList& cpuids) -> void {
    fd_.ioctl(KVM_GET_EMULATED_CPUID, cpuids.get());
}

/**
 * Returns a list of host-supported and kvm-specific MSRs.
 *
 * Examples
 * ========
 * ```
 * #include <vmm/kvm.hpp>
 *
 * auto kvm = vmm::kvm::system{};
 * auto msr_list = kvm.msr_index_list();
 * ```
 */
auto system::msr_index_list() -> MsrIndexList {
    auto msrs = MsrIndexList{MAX_IO_MSRS};
    msr_index_list(msrs);
    return msrs;
}

/**
 * Obtains a list of host-supported and kvm-specific MSRs.
 *
 * Examples
 * ========
 * ```
 * #include <vmm/kvm.hpp>
 *
 * auto kvm = vmm::kvm::system{};
 * auto msr_list = vmm::kvm::MsrIndexList{MAX_IO_MSRS};
 * kvm.msr_index_list(msr_list);
 * ```
 */
auto system::msr_index_list(MsrIndexList& msrs) -> void {
    fd_.ioctl(KVM_GET_MSR_INDEX_LIST, msrs.get());
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
 * ```
 */
auto system::msr_feature_list() -> MsrFeatureList {
    auto msrs = MsrFeatureList{MAX_IO_MSRS_FEATURES};
    msr_feature_list(msrs);
    return msrs;
}

/**
 * Obtains a list of MSRs exposing MSR-based CPU features.
 *
 * Examples
 * ========
 * ```
 * #include <vmm/kvm.hpp>
 *
 * auto kvm = vmm::kvm::system{};
 * auto msr_list = vmm::kvm::MsrFeatureList{MAX_IO_MSRS_FEATURES};
 * kvm.msr_feature_list(msr_list);
 * ```
 */
auto system::msr_feature_list(MsrFeatureList& msrs) -> void {
    fd_.ioctl(KVM_GET_MSR_FEATURE_INDEX_LIST, msrs.get());
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
 * auto msrs = vmm::kvm::MsrList{entry};
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
 * auto msrs = vmm::kvm::MsrList{entries};
 * auto nmsrs = kvm.get_msrs(msrs);
 * ```
 */
auto system::get_msrs(MsrList& msrs) -> unsigned int {
    return fd_.ioctl(KVM_GET_MSRS, msrs.get());
}

}  // namespace vmm::kvm::detail
