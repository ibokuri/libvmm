#pragma once

#include "vmm/utility/utility.hpp"

namespace vmm::kvm::detail {

class KvmIoctl {
    protected:
        unsigned int fd_;
        bool closed_;

        KvmIoctl(const unsigned int fd) noexcept : fd_{fd}, closed_{false} {}

        ~KvmIoctl() noexcept {
            if (!closed_) {
                try {
                    utility::close(fd_);
                }
                catch (std::system_error& e) {
                    // TODO
                }
            }
        }
    public:
        /**
        * Closes the KVM ioctl class handle.
        *
        * Use this if you'd like to handle possible failures of `utility::close()`.
        *
        * Examples
        * ========
        * ```
        * #include <vmm/kvm.hpp>
        *
        * vmm::kvm::system kvm;
        * vmm::kvm::vm vm {kvm.vm()};
        * vmm::kvm::device device {vm.device()};
        *
        * try {
        *     device.close();
        * }
        * catch (std::system_error)
        *     throw;
        * ```
        */
        auto close() -> void {
            utility::close(fd_);
            closed_ = true;
        }
};

}  // namespace vmm::kvm::detail
