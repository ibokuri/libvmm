#include <unistd.h> // close, dup

#include "vmm/types/file_descriptor.hpp"

namespace vmm::types {

FileDescriptor::FileDescriptor() noexcept : closed_{false} {};
FileDescriptor::FileDescriptor(unsigned int fd) noexcept : fd_{fd}, closed_{false} {};

FileDescriptor::~FileDescriptor() noexcept {
    if (!closed_) {
        closed_ = ::close(fd_) == 0;
    }
};

FileDescriptor::FileDescriptor(const FileDescriptor& other) {
    fd_ = ::dup(other.fd_);

    if (fd_ < 0) {
        VMM_THROW(std::system_error(errno, std::system_category()));
    }
}

auto FileDescriptor::operator=(const FileDescriptor& other) -> FileDescriptor& {
    if (&other == this)
        return *this;

    fd_ = ::dup(other.fd_);

    if (fd_ < 0) {
        VMM_THROW(std::system_error(errno, std::system_category()));
    }

    return *this;
}


auto FileDescriptor::close() -> void {
    if (::close(fd_) < 0) {
        VMM_THROW(std::system_error(errno, std::system_category()));
    }
    closed_ = true;
}

}  // namespace vmm::types
