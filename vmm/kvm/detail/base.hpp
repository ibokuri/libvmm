/*
 * base.hpp - Base class for KVM ioctls
 */

#pragma once

#include "vmm/types/file_descriptor.hpp"
#include "vmm/types/detail/exceptions.hpp"

namespace vmm::kvm::detail {

class KvmFd : public vmm::types::FileDescriptor {
    public:
        KvmFd(unsigned int fd) noexcept : vmm::types::FileDescriptor(fd) {}

        KvmFd(const KvmFd& other) = delete;
        KvmFd(KvmFd&& other) = default;
        auto operator=(const KvmFd& other) -> KvmFd& = delete;
        auto operator=(KvmFd& other) -> KvmFd& = default;

        auto read() const -> void;
        auto write() const -> void;
        auto stat() const -> void;
};

}  // namespace vmm::kvm::detail
