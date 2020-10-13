//
// file_descriptor.hpp - KVM file descriptor
//

#pragma once

#include "vmm/types/file_descriptor.hpp"

namespace vmm::kvm::detail {

class KvmFd : public vmm::types::FileDescriptor
{
    public:
        explicit KvmFd(int fd) noexcept : vmm::types::FileDescriptor(fd) {}

        KvmFd(const KvmFd&) = delete;
        KvmFd(KvmFd&&) = default;
        auto operator=(const KvmFd&) -> KvmFd& = delete;
        auto operator=(KvmFd&&) -> KvmFd& = default;
};

}  // namespace vmm::kvm::detail
