#include "vmm/kvm/detail/base.hpp"
#include "vmm/utility/utility.hpp"

#include <linux/kvm.h>

namespace vmm::kvm::detail {

KvmIoctl::~KvmIoctl() noexcept {
    if (!closed_) {
        try {
            utility::close(fd_);
        }
        catch (std::system_error& e) {
            // TODO
        }
    }
}

/**
 * Closes the ioctl class handle.
 *
 * Use this if you'd like to handle possible failures of `utility::close()`.
 *
 * Examples
 * ========
 * ```
 * #include <vmm/kvm.hpp>
 *
 * auto kvm = vmm::kvm::system{};
 * auto vm = kvm.vm();
 * auto device = vm.device();
 *
 * try {
 *     device.close();
 * }
 * catch (std::system_error&)
 *     throw;
 * ```
 */
auto KvmIoctl::close() -> void {
    utility::close(fd_);
    closed_ = true;
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
auto KvmIoctl::check_extension(const unsigned int cap) -> unsigned int {
    return utility::ioctl(fd_, KVM_CHECK_EXTENSION, cap);
}

}  // namespace vmm::kvm::detail
