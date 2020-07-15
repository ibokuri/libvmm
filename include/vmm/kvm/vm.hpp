#pragma once

#include "system.hpp"

namespace vmm::kvm {
    class vm final {
        private:
            unsigned int fd_;
            unsigned int mmap_size_;

            /* To prevent users from constructing VMs using arbitrary file
             * descriptors and to restrict construction to existing KVM system
             * objects, system::vm() should be the only function that calls
             * this method.  */
            vm(const unsigned int fd, const unsigned int mmap_size) : fd_{fd}, mmap_size_{mmap_size} {}
            friend vm system::vm();
        public:
            ~vm() noexcept { close(fd_); }
    };
};
