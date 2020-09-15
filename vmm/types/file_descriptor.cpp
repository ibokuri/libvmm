#include <unistd.h> // close

#include "vmm/types/file_descriptor.hpp"

namespace vmm::types {

FileDescriptor::FileDescriptor() noexcept : closed_{false} {};
FileDescriptor::FileDescriptor(unsigned int fd) noexcept : fd_{fd}, closed_{false} {};

FileDescriptor::~FileDescriptor() noexcept {
    if (!closed_) {
        closed_ = ::close(fd_) == 0;
    }
};

auto FileDescriptor::close() -> void {
    if (::close(fd_) < 0) {
        VMM_THROW(std::system_error(errno, std::system_category()));
    }
    closed_ = true;
}

}  // namespace vmm::types
