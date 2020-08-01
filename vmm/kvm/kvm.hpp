#pragma once

#include "vmm/kvm/detail/kvm.hpp"

namespace vmm::kvm {

/* ioctl classes */
using system = vmm::kvm::detail::system;
using vm = vmm::kvm::detail::vm;
//using vcpu = vmm::kvm::detail::vcpu;

/* types */
using MsrIndexList = vmm::kvm::detail::MsrIndexList;
using MsrFeatureList = vmm::kvm::detail::MsrFeatureList;
using Msrs = vmm::kvm::detail::Msrs;
using Cpuid = vmm::kvm::detail::Cpuid;

}  // namespace vmm
