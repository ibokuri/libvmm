//
// memory.hpp - Public memory header
//

#pragma once

#include "vmm/memory/detail/address.hpp"
#include "vmm/memory/detail/guest.hpp"

namespace vmm::memory {

template<typename T>
using AddressValue = vmm::memory::detail::AddressValue<T>;

template<typename T, typename S>
using Address = vmm::memory::detail::Address<T, S>;

using GuestAddressValue = vmm::memory::detail::GuestAddressValue;
using GuestAddress = vmm::memory::detail::GuestAddress;

}  // vmm::memory
