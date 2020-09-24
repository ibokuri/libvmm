#pragma once

#include <cstddef> // size_t

#include "vmm/kvm/detail/system.hpp"
#include "vmm/kvm/detail/vm.hpp"
#include "vmm/kvm/detail/vcpu.hpp"
#include "vmm/kvm/detail/device.hpp"
#include "vmm/kvm/detail/types.hpp"

namespace vmm::kvm {

/* ioctl classes */
using system = vmm::kvm::detail::system;
using vm = vmm::kvm::detail::vm;
using vcpu = vmm::kvm::detail::vcpu;
using device = vmm::kvm::detail::device;

/* types */
template<std::size_t N> using MsrList = vmm::kvm::detail::MsrList<N>;
template<std::size_t N> using Msrs = vmm::kvm::detail::Msrs<N>;
template<std::size_t N> using Cpuids = vmm::kvm::detail::Cpuids<N>;
template<std::size_t N> using IrqRouting = vmm::kvm::detail::IrqRouting<N>;

}  // namespace vmm::kvm
