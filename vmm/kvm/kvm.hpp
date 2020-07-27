#pragma once

#include "vmm/kvm/internal/kvm.hpp"

namespace vmm::kvm {

/* ioctl classes */
using system = vmm::kvm_internal::system;
using vm = vmm::kvm_internal::vm;
//using vcpu = vmm::kvm_internal::vcpu;

/* types */
using MsrIndexList = vmm::kvm_internal::MsrIndexList;
using MsrFeatureList = vmm::kvm_internal::MsrFeatureList;
using Msrs = vmm::kvm_internal::Msrs;
using Cpuid = vmm::kvm_internal::Cpuid;

}  // namespace vmm
