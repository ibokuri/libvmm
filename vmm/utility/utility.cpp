#include <fcntl.h>
#include <unistd.h>

#include "vmm/utility/utility.hpp"

namespace vmm::utility {

FileDescriptor::FileDescriptor(unsigned int fd) noexcept : fd_{fd}, closed_{false} {};

FileDescriptor::~FileDescriptor() noexcept {
    if (!closed_) {
        try {
            close();
        }
        catch (std::system_error& e) {
            // TODO
        }
    }
};

auto FileDescriptor::close() -> void {
    if (::close(fd_) < 0)
        throw std::system_error{errno, std::system_category()};
    closed_ = true;
}

}  // namespace vmm::utility
