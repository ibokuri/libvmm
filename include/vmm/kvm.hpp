// Modular VMM components - KVM ioctls

#pragma once

#include <filesystem>

#include <fcntl.h>
#include <linux/kvm.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

namespace vmm::kvm {
	namespace fs = std::filesystem;

	/// Forward declaration for system::vm(), which has vm as its result type.
	class vm;

	class system final {
		private:
			int fd_;

			/// Creates a virtual machine and returns a file descriptor.
			///
			/// This should only be used indirectly through system::vm().
			auto create_vm() -> int {
				auto fd = ioctl(fd_, KVM_CREATE_VM, 0);
				if (fd < 0)
					throw fs::filesystem_error{"ioctl()",
											   "KVM_CREATE_VM",
											   std::error_code{errno, std::system_category()}};
				return fd;
			}
		public:
			/// Default constructor.
			system() : fd_{open()} {}

			/// Constructs a kvm object from a file descriptor assumed to be
			/// associated with /dev/kvm.
			///
			/// The passed file descriptor must have O_RDWR permissions for
			/// things to work. It is also encouraged to have O_CLOEXEC set,
			/// on the descriptor, though the flag may be omitted as needed.
			///
			/// Ownership of `fd` is transferred over to the created Kvm object.
			///
			/// # Example
			///
			/// ```
			/// #include <vmm/kvm.hpp>
			///
			/// auto fd{...}          // preferably, via kvm::system::open()
			/// kvm::system kvm{fd};
			/// ```
			explicit system(int fd) noexcept : fd_{fd} {};

			/// Opens /dev/kvm and returns a file descriptor.
			///
			/// Use cases for opening /dev/kvm without O_CLOEXEC typically
			/// involve using or passing the resulting file handle to another
			/// process. For example, a program may open /dev/kvm only to
			/// exec() into another program with seccomp filters that blacklist
			/// certain syscalls.
			///
			/// # Example
			///
			/// ```
			/// #include <vmm/kvm.hpp>
			///
			/// auto fd{kvm::system::open()};
			/// kvm::system kvm{fd};
			/// ```
			static auto open(bool cloexec=true) -> int {
				auto ret{::open("/dev/kvm", cloexec ? O_RDWR | O_CLOEXEC : O_RDWR)};
				if (ret < 0)
					throw fs::filesystem_error{"open()",
											   "/dev/kvm",
											   std::error_code{errno, std::system_category()}};
				return ret;
			}

			/// Returns the KVM API version.
			///
			/// Since one can construct a kvm object from any file descriptor,
			/// it is highly encouraged to always check that a kvm object
			/// contains a proper handle to /dev/kvm by calling this function
			/// before any other KVM-related functions.
			///
			/// There's really no situation where this function would be called
			/// outside a condition check. Therefore, it doesn't need to throw
			/// anything since the user will always check its value anyways.
			///
			/// # Example
			///
			/// ```
			/// #include <vmm/kvm.hpp>
			///
			/// kvm::system kvm;
			/// if (kvm.api_version() != 12) throw TODO;
			/// ```
			auto api_version() noexcept -> int {
				return ioctl(fd_, KVM_GET_API_VERSION, 0);
			}

			/// Returns the size of the shared memory region used by the KVM_RUN
			/// ioctl to communicate with userspace.
			auto vcpu_mmap_size() -> int {
				auto ret = ioctl(fd_, KVM_GET_VCPU_MMAP_SIZE, 0);
				if (ret < 0)
					throw fs::filesystem_error{"ioctl()",
											   "KVM_GET_VCPU_MMAP_SIZE",
											   std::error_code{errno, std::system_category()}};
				return ret;
			}

			auto vm() -> vm;

			~system() noexcept { close(fd_); }
	};

	class vm final {
		private:
			int fd_;
			int mmap_size_;

			/// Constructor.
			///
			/// To prevent users from constructing VMs using arbitrary file
			/// descriptors and to restrict construction to existing KVM system
			/// objects, system::vm() should be the only function that calls
			/// this method.
			vm(int fd, int mmap_size) : fd_{fd}, mmap_size_{mmap_size} {}
			friend vm system::vm();
		public:
			~vm() noexcept { close(fd_); }
	};

	/// Create a virtual machine.
	///
	/// # Example
	///
	/// ```
	/// #include <vmm/kvm.hpp>
	///
	/// kvm::system kvm;
	/// kvm::vm{kvm.vm()};
	/// ```
	auto system::vm() -> vmm::kvm::vm {
		auto mmap_size{vcpu_mmap_size()};
		auto fd{create_vm()};
		return vmm::kvm::vm{fd, mmap_size};
	}
}
