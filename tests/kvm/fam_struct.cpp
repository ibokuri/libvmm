#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <linux/kvm.h>

#include "vmm/kvm/detail/types/fam_struct.hpp"

template<std::size_t N>
using FamStruct = vmm::kvm::detail::FamStruct<&kvm_signal_mask::len,
                                              &kvm_signal_mask::sigset, N>;

template<std::size_t N>
class SignalMask : public FamStruct<N> {
    using Base = FamStruct<N>;
    using Base::Base;
};

TEST_CASE("Create FAM struct") {
    SECTION("Entries: 0") {
        auto fam = SignalMask<0>{};

        REQUIRE(fam.size() == 0);
        REQUIRE(fam.empty() == true);
        REQUIRE(fam.size() == fam.capacity());
    }

    SECTION("Entries: N") {
        auto fam = SignalMask<2>{1, 2};

        REQUIRE(fam.size() == 2);
        REQUIRE(fam.empty() == false);
        REQUIRE(fam.size() == fam.capacity());
        REQUIRE(fam[0] == 1);
        REQUIRE(fam[1] == 2);
    }

    SECTION("Entries: N (empty)") {
        auto fam = SignalMask<2>{};

        REQUIRE(fam.size() == 2);
        REQUIRE(fam.empty() == false);
        REQUIRE(fam.size() == fam.capacity());
        REQUIRE(fam[0] == 0);
        REQUIRE(fam[1] == 0);
    }
}

TEST_CASE("Copy/move FAM struct") {
    auto fam = SignalMask<2>{1, 2};

    SECTION("Copy constructor") {
        auto copy{fam};

        REQUIRE(copy.size() == fam.size());
        REQUIRE(copy.empty() == fam.empty());
        REQUIRE(copy.size() == fam.capacity());
        REQUIRE(copy[0] == fam[0]);
        REQUIRE(copy[1] == fam[1]);
    }

    SECTION("Copy assignment") {
        auto copy = fam;

        REQUIRE(copy.size() == fam.size());
        REQUIRE(copy.empty() == fam.empty());
        REQUIRE(copy.size() == fam.capacity());
        REQUIRE(copy[0] == fam[0]);
        REQUIRE(copy[1] == fam[1]);
    }

    //SECTION("Move constructor") {
        //auto copy{std::move(fam)};

        //REQUIRE(copy.size() == fam.size());
        //REQUIRE(copy.empty() == fam.empty());
        //REQUIRE(copy.size() == fam.capacity());
        //REQUIRE(copy[0] == fam[0]);
        //REQUIRE(copy[1] == fam[1]);
    //}

    //SECTION("Move assignment") {
        //auto copy = std::move(fam);

        //REQUIRE(copy.size() == fam.size());
        //REQUIRE(copy.empty() == fam.empty());
        //REQUIRE(copy.size() == fam.capacity());
        //REQUIRE(copy[0] == fam[0]);
        //REQUIRE(copy[1] == fam[1]);
    //}
}
