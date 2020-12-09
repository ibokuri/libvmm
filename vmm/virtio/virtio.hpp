//
// virtio.hpp - Public Virtio header
//

#pragma once

#include "vmm/virtio/detail/device.hpp"
#include "vmm/virtio/detail/queue.hpp"

namespace vmm::virtio {

using VirtDevice = vmm::virtio::detail::VirtDevice;
using VirtQueue = vmm::virtio::detail::VirtQueue;

}  // namespace vmm::virtio
