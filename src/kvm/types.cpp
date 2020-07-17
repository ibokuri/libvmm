/*
 * types.cpp - KVM types
 */

#include "../../include/vmm/kvm/types.hpp"

namespace vmm::kvm {
    MsrList::MsrList(MsrList&& other) : list_{std::move(other.list_)} {}

    MsrList& MsrList::operator=(MsrList&& other) {
        list_ = std::move(other.list_);
        return *this;
    }

    MsrFeatureList& MsrFeatureList::operator=(MsrFeatureList&& other) {
        MsrList::operator=(std::move(other));
        return *this;
    }
};
