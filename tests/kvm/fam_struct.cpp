#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <cassert>
#include <initializer_list>

#include "vmm/kvm/detail/types.hpp"

struct MockFamStruct {
    uint32_t len;
    uint32_t entries[0];
};

template<std::size_t N>
using FamStruct = vmm::kvm::detail::FamStruct<MockFamStruct, uint32_t, N>;

template<std::size_t N>
class MockWrapper : public FamStruct<N> {
    using Base = FamStruct<N>;
    using Base::Base;
};

//  fn test_new() {
//      let num_entries = 10;

//      let adapter = MockFamStructWrapper::new(num_entries);
//      assert_eq!(num_entries, adapter.capacity());

//      let u32_slice = unsafe {
//          std::slice::from_raw_parts(
//              adapter.as_fam_struct_ptr() as *const u32,
//              num_entries + ENTRIES_OFFSET,
//          )
//      };
//      assert_eq!(num_entries, u32_slice[0] as usize);
//      for entry in u32_slice[1..].iter() {
//          assert_eq!(*entry, 0);
//      }
//  }

TEST_CASE("FAM struct creation", "[all]") {
    SECTION("Entries (N)") {
        auto fam = MockWrapper<5>{1, 2, 3, 4, 5};

        REQUIRE(fam.size() == 5);

        REQUIRE(fam[0] == 1);
        REQUIRE(fam[1] == 2);
        REQUIRE(fam[2] == 3);
        REQUIRE(fam[3] == 4);
        REQUIRE(fam[4] == 5);
    }
}
