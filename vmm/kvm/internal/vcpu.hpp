/*
 * vcpu.hpp - vcpu ioctls
 */

#pragma once

namespace vmm {
    //class vcpu {
        //private:
        //public:
            /**
             * Reads the values of the MSRs provided.
             *
             * # Examples
             *
             * ```
             * #include <vmm/kvm.hpp>
             *
             * kvm::system kvm;
             * kvm::vm vm {kvm.vm()};
             * kvm::vcpu vcpu {vm.vcpu()};
             *
             * kvm_msr_entry entry{0x175, 0 , 1};
             * kvm::Msrs msrs{entry};
             * vcpu.set_msrs(msrs);
             * auto nmsrs {vcpu.msrs(msrs)};
             * ```
             *
             * ```
             * #include <vector>
             * #include <vmm/kvm.hpp>
             *
             * kvm::system kvm;
             * kvm::vm vm {kvm.vm()};
             * kvm::vcpu vcpu {vm.vcpu()};
             * std::vector<kvm_msr_entry> entries;
             * kvm::MsrIndexList msr_list {kvm.msr_index_list()};
             *
             * for (auto msr : msr_list) {
             *     kvm_msr_entry entry{msr};
             *     vec.push_back(entry);
             * }
             *
             * kvm::Msrs msrs{vec};
             * vcpu.set_msrs(msrs);
             * auto nmsrs {vcpu.msrs(msrs)};
             * ```
             */
            //auto msrs() -> unsigned int;
    //};
} // namespace vmm
