//
// kvm.hpp - Public KVM header
//

#pragma once

#include <cstddef> // size_t

#include "vmm/kvm/detail/ioctls/system.hpp"
#include "vmm/kvm/detail/ioctls/vm.hpp"
#include "vmm/kvm/detail/ioctls/vcpu.hpp"
#include "vmm/kvm/detail/ioctls/device.hpp"
#include "vmm/kvm/detail/types/fam_struct.hpp"

namespace vmm::kvm {

using system = vmm::kvm::detail::system;
using vm = vmm::kvm::detail::vm;
using vcpu = vmm::kvm::detail::vcpu;
using device = vmm::kvm::detail::device;

template<std::size_t N> using MsrList = vmm::kvm::detail::MsrList<N>;
template<std::size_t N> using Msrs = vmm::kvm::detail::Msrs<N>;
template<std::size_t N> using Cpuids = vmm::kvm::detail::Cpuids<N>;
template<std::size_t N> using IrqRouting = vmm::kvm::detail::IrqRouting<N>;

}  // namespace vmm::kvm
