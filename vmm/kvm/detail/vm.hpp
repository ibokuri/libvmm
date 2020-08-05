/*
 * vm.hpp - VM ioctls
 */

#pragma once

#include "vmm/kvm/detail/system.hpp"

namespace vmm::kvm::detail {

class vm {
    private:
        unsigned int fd_;
        unsigned int mmap_size_;
        bool closed_;

        /**
         * Constructor reserved for system::vm().
         *
         * To prevent users from constructing VMs via arbitrary descriptors and
         * to restrict VM construction to occur through existing kvm objects,
         * system::vm() should be the only function that calls this method.
         */
        vm(const unsigned int fd, const unsigned int mmap_size)
            : fd_{fd}, mmap_size_{mmap_size}, closed_{false} {}
        friend vm system::vm(unsigned int);
    public:
        ~vm() noexcept;
        auto close() -> void;

        void user_memory_region(kvm_userspace_memory_region region);
        auto check_extension(unsigned int cap) -> unsigned int;
        auto nr_vcpus() -> unsigned int;
        auto max_vcpus() -> unsigned int;
        auto nr_memslots() -> unsigned int;
};

}  // namespace vmm:vm
