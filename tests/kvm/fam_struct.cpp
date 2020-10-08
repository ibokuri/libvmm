#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <cassert>
#include <initializer_list>

#include "vmm/kvm/detail/types/fam_struct.hpp"

struct MockFamStruct {
    uint32_t len;
    uint32_t entries[0];
};

template<std::size_t N>
using FamStruct = vmm::kvm::detail::FamStruct<MockFamStruct,
                                              uint32_t,
                                              &MockFamStruct::len,
                                              &MockFamStruct::entries,
                                              N>;

template<std::size_t N>
class MockWrapper : public FamStruct<N> {
    using Base = FamStruct<N>;
    using Base::Base;
};

TEST_CASE("Create FAM struct", "[all]") {
    SECTION("Entries: 0") {
        auto fam = MockWrapper<0>{};

        REQUIRE(fam.size() == 0);
        REQUIRE(fam.empty() == true);
        REQUIRE(fam.size() == fam.capacity());
    }

    SECTION("Entries: N") {
        auto fam = MockWrapper<2>{1, 2};

        REQUIRE(fam.size() == 2);
        REQUIRE(fam.empty() == false);
        REQUIRE(fam.size() == fam.capacity());
        REQUIRE(fam[0] == 1);
        REQUIRE(fam[1] == 2);
    }

    SECTION("Entries: N (empty)") {
        auto fam = MockWrapper<2>{};

        REQUIRE(fam.size() == 2);
        REQUIRE(fam.empty() == false);
        REQUIRE(fam.size() == fam.capacity());
        REQUIRE(fam[0] == 0);
        REQUIRE(fam[1] == 0);
    }
}

TEST_CASE("Copy/move FAM struct", "[all]") {
    auto fam = MockWrapper<2>{1, 2};

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
