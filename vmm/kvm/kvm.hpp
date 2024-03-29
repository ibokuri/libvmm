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

using System = vmm::kvm::detail::System;
using Vm = vmm::kvm::detail::Vm;
using Vcpu = vmm::kvm::detail::Vcpu;
using Device = vmm::kvm::detail::Device;

using VcpuExit = vmm::kvm::detail::VcpuExit;

using IrqLevel = vmm::kvm::detail::IrqLevel;

template<std::size_t N> using IrqRouting = vmm::kvm::detail::IrqRouting<N>;
#if defined(__i386__) || defined(__x86_64__)
template<std::size_t N> using MsrList = vmm::kvm::detail::MsrList<N>;
template<std::size_t N> using Msrs = vmm::kvm::detail::Msrs<N>;
template<std::size_t N> using Cpuids = vmm::kvm::detail::Cpuids<N>;
#endif

}  // namespace vmm::kvm
