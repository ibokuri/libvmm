#include "vmm/kvm/detail/vcpu.hpp"
#include "vmm/utility/utility.hpp"

namespace vmm::kvm::detail {

vcpu::~vcpu() noexcept {
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
 * Closes the vcpu handle.
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
 * vmm::kvm::vcpu vcpu {vm.vcpu()};
 *
 * try {
 *     vcpu.close();
 * }
 * catch (std::system_error)
 *     throw;
 * ```
 */
auto vcpu::close() -> void {
    utility::close(fd_);
    closed_ = true;
}

}  // namespace vmm::kvm::detail
