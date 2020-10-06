#include <unistd.h> // close, dup

#include "vmm/types/file_descriptor.hpp"

namespace vmm::types {

FileDescriptor::~FileDescriptor() noexcept {
    if (!m_closed)
        m_closed = ::close(m_fd) == 0;
};

FileDescriptor::FileDescriptor(const FileDescriptor& other) {
    m_fd = ::dup(other.m_fd);

    if (m_fd < 0)
        VMM_THROW(std::system_error(errno, std::system_category()));
}

auto FileDescriptor::operator=(const FileDescriptor& other) -> FileDescriptor& {
    if (&other == this)
        return *this;

    m_fd = ::dup(other.m_fd);

    if (m_fd < 0)
        VMM_THROW(std::system_error(errno, std::system_category()));

    return *this;
}


auto FileDescriptor::close() -> void {
    if (::close(m_fd) < 0)
        VMM_THROW(std::system_error(errno, std::system_category()));

    m_closed = true;
}

}  // namespace vmm::types
