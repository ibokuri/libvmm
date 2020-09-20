#define CATCH_CONFIG_MAIN

#include <limits>
#include <catch2/catch.hpp>
#include "vmm/types/event.hpp"

namespace vt = vmm::types;

TEST_CASE("EventFd creation", "[api]") {
    REQUIRE_NOTHROW(vt::EventFd{});
    REQUIRE_NOTHROW(vt::EventFd{EFD_NONBLOCK});
}

TEST_CASE("EventFd read/write", "[api]") {
    auto fd = vt::EventFd{EFD_NONBLOCK};
    fd.write(55);
    REQUIRE(fd.read() == 55);
}

TEST_CASE("EventFd write overflow", "[api]") {
    auto fd = vt::EventFd{EFD_NONBLOCK};
    REQUIRE_NOTHROW(fd.write(std::numeric_limits<uint64_t>::max() - 1));
    REQUIRE_THROWS(fd.write(1));
}

TEST_CASE("EventFd empty read", "[api]") {
    auto fd = vt::EventFd{EFD_NONBLOCK};
    REQUIRE_THROWS(fd.read());
}

TEST_CASE("EventFd clone", "[api]") {
    auto fd = vt::EventFd{EFD_NONBLOCK};
    auto clone{fd};
    auto clone2 = fd;

    fd.write(123);
    REQUIRE(clone.read() == 123);

    fd.write(234);
    REQUIRE(clone2.read() == 234);
}
