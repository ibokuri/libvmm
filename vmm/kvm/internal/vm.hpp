#pragma once

#include "vmm/kvm/internal/system.hpp"

namespace vmm::kvm_internal {
    class vm final {
        private:
            unsigned int fd_;
            unsigned int mmap_size_;

            /**
             * Constructor reserved for system::vm().
             *
             * To prevent users from constructing VMs using arbitrary file
             * descriptors and to restrict VM construction to occur through
             * existing `kvm objects, system::vm() should be the only function
             * that calls this method.
             */
            vm(const unsigned int fd, const unsigned int mmap_size) : fd_{fd}, mmap_size_{mmap_size} {}
            friend vm system::vm();
        public:
            ~vm() noexcept { close(fd_); }
    };
};
