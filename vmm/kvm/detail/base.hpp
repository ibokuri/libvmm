/*
 * base.hpp - Base class for KVM ioctls
 */

#pragma once

namespace vmm::kvm::detail {

class KvmIoctl {
    protected:
        unsigned int fd_;
        bool closed_;

        KvmIoctl(unsigned int fd) noexcept : fd_{fd}, closed_{false} {}
        ~KvmIoctl() noexcept;
    public:
        auto close() -> void;

        // Copy & move constructors
        KvmIoctl(const KvmIoctl& other) = delete;
        KvmIoctl(KvmIoctl&& other) = default;
        auto operator=(const KvmIoctl& other) -> KvmIoctl& = delete;
        auto operator=(KvmIoctl& other) -> KvmIoctl& = default;
};

}  // namespace vmm::kvm::detail
