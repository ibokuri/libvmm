//
// queue.cpp - Virtio queue
//

#include "vmm/virtio/detail/queue.hpp"

namespace vmm::virtio::detail {

auto Descriptor::addr() const noexcept -> GuestAddress
{
    return m_addr;
}

}  // namespace vmm::virtio::detail
