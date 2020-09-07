/*
 * base.hpp - Base class for KVM ioctls
 */

#pragma once

#include "vmm/utility/utility.hpp"

namespace vmm::kvm::detail {

class KvmFd : public vmm::utility::FileDescriptor {
    public:
        KvmFd(unsigned int fd) noexcept : vmm::utility::FileDescriptor(fd) {}

        KvmFd(const KvmFd& other) = delete;
        KvmFd(KvmFd&& other) = default;
        auto operator=(const KvmFd& other) -> KvmFd& = delete;
        auto operator=(KvmFd& other) -> KvmFd& = default;

        auto read() const -> void;
        auto write() const -> void;
        auto stat() const -> void;
};

}  // namespace vmm::kvm::detail
