/*
 * base.cpp - Base class for KVM ioctls
 */

#include "vmm/kvm/detail/base.hpp"
#include "vmm/utility/utility.hpp"

namespace vmm::kvm::detail {

//KvmIoctl::~KvmIoctl() noexcept {
    //if (!closed_) {
        //try {
            //utility::close(fd_);
        //}
        //catch (std::system_error& e) {
            //// TODO
        //}
    //}
//}

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
//auto KvmIoctl::close() -> void {
    //utility::close(fd_);
    //closed_ = true;
//}

}  // namespace vmm::kvm::detail
