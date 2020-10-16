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
        // See the documentation for KVM_GET_API_VERSION.
        [[nodiscard]] auto api_version() const -> unsigned;

        // Creates and returns a virtual machine (with a custom machine type).
        //
        // The VM's vcpu mmap area will be initialized with the KVM_GET_VCPU_MMAP_SIZE
        // ioctl's result.
        //
        // See the documentation for KVM_CREATE_VM.
        [[nodiscard]] auto vm(unsigned machine_type=0) const -> vmm::kvm::detail::vm;

        // Returns a positive integer if a KVM extension is available; 0 otherwise.
        //
        // Based on their initialization, VMs may have different capabilities.
        // Therefore, `kvm::vm::check_extension()` is preferred when querying
        // for most capabilities.
        [[nodiscard]] auto check_extension(unsigned cap) const -> unsigned;

        // Returns the size of the memory region used by the KVM_RUN ioctl to
        // communicate CPU information to userspace.
        [[nodiscard]] auto vcpu_mmap_size() const -> std::size_t;

#if defined(__i386__) || defined(__x86_64__)
        // TODO: Allocator variants for FAM struct methods

        // Returns a list of host-supported and kvm-specific MSRs.
        //
        // See the documentation for KVM_GET_MSR_INDEX_LIST.
        template<std::size_t N=MAX_IO_MSRS>
        [[nodiscard]] auto msr_index_list() const -> MsrList<N>
        {
            auto msrs = MsrList<N>{};
            m_fd.ioctl(KVM_GET_MSR_INDEX_LIST, msrs.data());
            return msrs;
        }

        // Returns a list of MSRs exposing MSR-based CPU features.
        //
        // See the documentation for KVM_GET_MSR_FEATURE_INDEX_LIST.
        template<std::size_t N=MAX_IO_MSRS_FEATURES>
        [[nodiscard]] auto msr_feature_list() const -> MsrList<N>
        {
            auto msrs = MsrList<N>{};
            m_fd.ioctl(KVM_GET_MSR_FEATURE_INDEX_LIST, msrs.data());
            return msrs;
        }

        // Reads the values of MSR-based features available for VMs. Returns
        // the number of successfully read values.
        //
        // See the documentation for KVM_GET_MSRS.
        template<typename T,
                 typename=std::enable_if_t<std::is_same_v<typename T::value_type,
                                                          kvm_msr_entry>>>
        auto get_msr_features(T &msrs) const -> unsigned
        {
            return m_fd.ioctl(KVM_GET_MSRS, msrs.data());
        }

        // Returns a list of host- and kvm-supported x86 cpuid features.
        //
        // See the documentation for KVM_GET_SUPPORTED_CPUID.
        template<std::size_t N=MAX_CPUID_ENTRIES>
        [[nodiscard]] auto supported_cpuids() const -> Cpuids<N>
        {
            auto cpuids = Cpuids<N>{};
            m_fd.ioctl(KVM_GET_SUPPORTED_CPUID, cpuids.data());
            return cpuids;
        }

        // Returns a list of kvm-emulated x86 cpuid features.
        //
        // See the documentation for KVM_GET_EMULATED_CPUID.
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
