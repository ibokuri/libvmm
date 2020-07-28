#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include "vmm/kvm/kvm.hpp"

namespace kvm = vmm::kvm;

TEST_CASE("API version (default)", "[api]") {
	kvm::system kvm;

	REQUIRE(kvm.api_version() == KVM_API_VERSION);
}

TEST_CASE("API version (manual)", "[api]") {
	auto fd {kvm::system::open()};
	kvm::system kvm{fd};

	REQUIRE(kvm.api_version() == KVM_API_VERSION);
}

TEST_CASE("Create VM", "[api]") {
	kvm::system kvm;
	kvm::vm vm {kvm.vm()};
}

TEST_CASE("Get MSR index list", "[api]") {
	kvm::system kvm;
	auto msr_list = kvm.msr_index_list();
}
