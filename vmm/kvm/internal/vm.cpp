/*
 * vm.cpp - VM ioctls
 */

namespace vmm::kvm_internal {

//auto vm::nr_vcpus() -> unsigned int {
    //auto ret {check_extension(KVM_CAP_NR_VCPUS)};
    //return ret > 0 ? ret : 4;
//}

//auto vm::max_vcpus() -> unsigned int {
    //auto ret {check_extension(KVM_CAP_MAX_VCPUS)};
    //return ret > 0 ? ret : nr_vcpus();
//}

//auto vm::nr_memslots() -> unsigned int {
    //auto ret {check_extension(KVM_CAP_NR_MEMSLOTS)};
    //return ret > 0 ? ret : 32;
//}

}  // namespace vmm::kvm_internal
