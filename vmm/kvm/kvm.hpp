#pragma once

#include "vmm/kvm/detail/vcpu.hpp"
#include "vmm/kvm/detail/device.hpp"

namespace vmm::kvm {

/* ioctl classes */
using system = vmm::kvm::detail::system;
using vm = vmm::kvm::detail::vm;
using vcpu = vmm::kvm::detail::vcpu;
using device = vmm::kvm::detail::device;

/* types */
using MsrIndexList = vmm::kvm::detail::MsrIndexList;
using MsrFeatureList = vmm::kvm::detail::MsrFeatureList;
using MsrList = vmm::kvm::detail::MsrList;
using CpuidList = vmm::kvm::detail::CpuidList;
using IrqRoutingList = vmm::kvm::detail::IrqRoutingList;

}  // namespace vmm::kvm
