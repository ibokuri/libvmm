#pragma once

namespace vmm::kvm::detail {

class KvmIoctl {
    protected:
        unsigned int fd_;
        bool closed_;

        KvmIoctl(const unsigned int fd) noexcept : fd_{fd}, closed_{false} {}
        ~KvmIoctl() noexcept;
    public:
        auto close() -> void;

        // Copy/Move constructors
        KvmIoctl(const KvmIoctl& other) = delete;
        KvmIoctl(KvmIoctl&& other) = default;
        KvmIoctl& operator=(const KvmIoctl& other) = delete;
        KvmIoctl& operator=(KvmIoctl& other) = default;

        // Control routines
        auto check_extension(const unsigned int cap) -> unsigned int;
};

}  // namespace vmm::kvm::detail
