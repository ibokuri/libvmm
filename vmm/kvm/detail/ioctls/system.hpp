//
// system.hpp - KVM ioctls
//

#pragma once

#include <cstddef> // size_t
#include <system_error> // error_code, system_category, system_error
#include <type_traits> // enable_if_t, is_same_v
#include <fcntl.h> // open
#include <sys/stat.h> // open
#include <sys/types.h> // open
#include <linux/kvm.h> // kvm_*, KVM_*

#include "vmm/kvm/detail/macros.hpp"
#include "vmm/kvm/detail/types/file_descriptor.hpp"
#include "vmm/kvm/detail/types/fam_struct.hpp"
#include "vmm/types/detail/exceptions.hpp"

namespace vmm::kvm::detail {

class vm;

class system
{
    public:
        system() : m_fd{open()} {}

        // Constructs a kvm object from a file descriptor.
        //
        // The passed file descriptor should have O_RDWR permissions for things
        // to work. It is also encouraged to have O_CLOEXEC set, though it may
        // be omitted as needed.
        explicit system(int fd) : m_fd{fd} {}

        system(const system& other) = delete;
        system(system&& other) = default;
        auto operator=(const system& other) -> system& = delete;
        auto operator=(system&& other) -> system& = default;

        // Opens /dev/kvm and returns a file descriptor.
        //
        // Use cases for opening /dev/kvm without O_CLOEXEC typically involve
        // using or passing the resulting file handle to another process. For
        // example, a program may open /dev/kvm only to exec() into another
        // program with seccomp filters.
        [[nodiscard]] static auto open(bool cloexec=true) -> int
        {
            const auto fd = ::open("/dev/kvm",
                                   cloexec ? O_RDWR | O_CLOEXEC : O_RDWR);

            if (fd < 0)
                VMM_THROW(std::system_error(errno, std::system_category()));

            return fd;
        }

        // Returns the kvm API version.
        //
        // Applications should refuse to run if a value other than 12 is
        // returned.
        [[nodiscard]] auto api_version() const -> unsigned;

        // Creates and returns a virtual machine (with a custom machine type).
        //
        // The VM's vcpu mmap area will be initialized with the KVM_GET_VCPU_MMAP_SIZE
        // ioctl's result.
        //
        // By default, the physical address size for a VM (IPA Size limit) on AArch64
        // is limited to 40-bits. However, this limit can be configured if the host
        // supports the KVM_CAP_ARM_VM_IPA_SIZE extension. When supported, use
        // KVM_VM_TYPE_ARM_IPA_SIZE(IPA_Bits) to set the size in the machine type
        // identifier, where IPA_Bits is the maximum width of any physical address used
        // by the VM.
        [[nodiscard]] auto vm(unsigned machine_type=0) const -> vmm::kvm::detail::vm;

        // Returns a positive integer if a KVM extension is available; 0 otherwise.
        //
        // Based on their initialization, VMs may have different capabilities. Thus,
        // `kvm::vm::check_extension()` is preferred when querying for most
        // capabilities.
        //
        // TODO: No test. rust-vmm doesn't have any for this either.
        [[nodiscard]] auto check_extension(unsigned cap) const -> unsigned;

        // Returns the size of the memory region used by the KVM_RUN ioctl to
        // communicate CPU information to userspace.
        //
        // Each vcpu has an associated `kvm_run` struct for communicating information
        // about the CPU between kernel and userspace. In particular, whenever hardware
        // virtualization stops (called a VM-exit), the `kvm_run` struct will contain
        // information about why it stopped. We map this structure into userspace via
        // mmap(), but we need to know beforehand how much memory to map. We get that
        // information with the KVM_GET_VCPU_MMAP_SIZE ioctl.
        //
        // Note that the mmap size typically exceeds that of the `kvm_run` struct since
        // the kernel will also use that space to store other transient structures that
        // kvm_run may point to.
        [[nodiscard]] auto vcpu_mmap_size() const -> std::size_t;

#if defined(__i386__) || defined(__x86_64__)
        // TODO: Allocator variants for FAM struct methods

        // Returns a list of host-supported and kvm-specific MSRs.
        template<std::size_t N=MAX_IO_MSRS>
        [[nodiscard]] auto msr_index_list() const -> MsrList<N>
        {
            auto msrs = MsrList<N>{};
            m_fd.ioctl(KVM_GET_MSR_INDEX_LIST, msrs.data());
            return msrs;
        }

        // Returns a list of MSRs exposing MSR-based CPU features.
        template<std::size_t N=MAX_IO_MSRS_FEATURES>
        [[nodiscard]] auto msr_feature_list() const -> MsrList<N>
        {
            auto msrs = MsrList<N>{};
            m_fd.ioctl(KVM_GET_MSR_FEATURE_INDEX_LIST, msrs.data());
            return msrs;
        }

        // Reads the values of MSR-based features available for VMs. Returns
        // the number of successfully read values.
        template<typename T,
                 typename=std::enable_if_t<std::is_same_v<typename T::value_type,
                                                          kvm_msr_entry>>>
        auto get_msr_features(T &msrs) const -> unsigned
        {
            return m_fd.ioctl(KVM_GET_MSRS, msrs.data());
        }

        // Returns a list of host- and kvm-supported x86 cpuid features.
        //
        // In x86, the CPU identification (CPUID) instruction is a
        // supplementary instruction allowing software to discover details of
        // the processor. A program can use the CPUID to determine processor
        // type and whether certain features are implemented.
        template<std::size_t N=MAX_CPUID_ENTRIES>
        [[nodiscard]] auto supported_cpuids() const -> Cpuids<N>
        {
            auto cpuids = Cpuids<N>{};
            m_fd.ioctl(KVM_GET_SUPPORTED_CPUID, cpuids.data());
            return cpuids;
        }

        // Returns a list of kvm-emulated x86 cpuid features.
        template<std::size_t N=MAX_CPUID_ENTRIES>
        [[nodiscard]] auto emulated_cpuids() const -> Cpuids<N>
        {
            auto cpuids = Cpuids<N>{};
            m_fd.ioctl(KVM_GET_EMULATED_CPUID, cpuids.data());
            return cpuids;
        }
#endif

#if defined(__arm__)  || defined(__aarch64__)
        // Returns the IPA size for a VM; 0 if the capability isn't available.
        //
        // On AArch64, a guest OS has a set of translation tables that map
        // from the virtual address space to what it thinks is the physical
        // address space, also called the Intermediate Physical Address (IPA)
        // space. However, addresses in the IPA space undergo a second
        // translation into the real physical address space by the hypervisor.
        [[nodiscard]] auto host_ipa_limit() const -> unsigned;
#endif
    private:
        KvmFd m_fd;

        // Returns a file descriptor associated with a newly created VM.
        //
        // This method should only be used by `system::vm()`.
        [[nodiscard]] auto create_vm(unsigned machine_type) const -> int;
};

}  // namespace vmm::kvm::detail
