/*
 * base.hpp - Base class for KVM ioctls
 */

#pragma once

#include "vmm/types/detail/exceptions.hpp"
#include "vmm/types/file_descriptor.hpp"

namespace vmm::kvm::detail {

class KvmFd : public vmm::types::FileDescriptor {
    public:
        KvmFd(int fd) noexcept : vmm::types::FileDescriptor(fd) {}

        KvmFd(const KvmFd& other) = delete;
        KvmFd(KvmFd&& other) = default;
        auto operator=(const KvmFd& other) -> KvmFd& = delete;
        auto operator=(KvmFd& other) -> KvmFd& = default;
};

}  // namespace vmm::kvm::detail
