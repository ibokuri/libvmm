// Modular VMM components - KVM ioctls

#pragma once

#include <filesystem>
#include <vmm/utils.hpp>

#include <fcntl.h>
#include <linux/kvm.h>
#include <sys/stat.h>
#include <unistd.h>

namespace vmm::kvm {
	namespace fs = std::filesystem;

	/// Forward declaration for system::vm(), which has vm as its result type.
	class vm;

	class system final {
		private:
			unsigned int fd_;

			/// Creates a virtual machine and returns a file descriptor.
			///
			/// This should only be used indirectly through system::vm().
			auto create_vm() -> unsigned int {
				return utils::ioctl(fd_, KVM_CREATE_VM);
			}
		public:
			/// Default constructor.
			system() : fd_{open()} {}

			/// Constructs a kvm object from a file descriptor.
			///
			/// The passed file descriptor must have O_RDWR permissions for
			/// things to work. It is also encouraged to have O_CLOEXEC set,
			/// on the descriptor, though the flag may be omitted as needed.
			///
			/// Note that the passed file descriptor is of type unsigned int.
			/// As such, users will have to use kvm::system::open() instead of
			/// the C-style open() if they want to create a kvm object. This
			/// ensures that `fd` is both a valid handle and one that contains
			/// a proper amount of permissions for subsequent KVM operations.
			///
			/// # Safety
			///
			/// Ownership of `fd` is transferred over to the created Kvm object.
			///
			/// # Examples
			///
			/// See kvm::system::open().
			explicit system(unsigned int fd) noexcept : fd_{fd} {};

			/// Opens /dev/kvm and returns a file descriptor.
			///
			/// Use cases for opening /dev/kvm without O_CLOEXEC typically
			/// involve using or passing the resulting file handle to another
			/// process. For example, a program may open /dev/kvm only to
			/// exec() into another program with seccomp filters that blacklist
			/// certain syscalls.
			///
			/// # Examples
			///
			/// ```
			/// #include <vmm/kvm.hpp>
			///
			/// auto fd {kvm::system::open()};
			/// kvm::system kvm {fd};
			/// ```
			static auto open(const bool cloexec=true) -> unsigned int {
				const auto fd {::open("/dev/kvm", cloexec ? O_RDWR | O_CLOEXEC : O_RDWR)};
				if (fd < 0)
					throw fs::filesystem_error{"open()",
											   "/dev/kvm",
											   std::error_code{errno, std::system_category()}};
				return fd;
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
			/// # Examples
			///
			/// ```
			/// #include <vmm/kvm.hpp>
			///
			/// kvm::system kvm;
			/// if (kvm.api_version() != 12) throw TODO;
			/// ```
			auto api_version() -> unsigned int {
				return utils::ioctl(fd_, KVM_GET_API_VERSION);
			}

			/// Returns the size of the shared memory region used by the KVM_RUN
			/// ioctl to communicate with userspace.
			///
			/// # Examples
			///
			/// ```
			/// #include <vmm/kvm.hpp>
			///
			/// kvm::system kvm;
			/// TODO
			/// ```
			auto vcpu_mmap_size() -> unsigned int {
				return utils::ioctl(fd_, KVM_GET_VCPU_MMAP_SIZE);
			}

			auto vm() -> vm;

			~system() noexcept { close(fd_); }
	};

	class vm final {
		private:
			unsigned int fd_;
			unsigned int mmap_size_;

			/// Constructor.
			///
			/// To prevent users from constructing VMs using arbitrary file
			/// descriptors and to restrict construction to existing KVM system
			/// objects, system::vm() should be the only function that calls
			/// this method.
			vm(const unsigned int fd, const unsigned int mmap_size) : fd_{fd}, mmap_size_{mmap_size} {}
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
