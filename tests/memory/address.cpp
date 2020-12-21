#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include "vmm/memory.hpp"

class TestAddress : public vmm::memory::Address<TestAddress, uint64_t>
{
    using value_type = TestAddress;
    using size_type = uint64_t;
    using const_reference = const value_type&;

    private:
        size_type m_addr{};
    public:
        explicit TestAddress(size_type addr=0) noexcept : m_addr{addr} {}

        auto data() const noexcept -> size_type override { return m_addr; }

        auto operator&(const size_type mask) const noexcept -> size_type override { return m_addr & mask; }
        auto operator|(const size_type mask) const noexcept -> size_type override { return m_addr | mask; }
        auto operator+(const size_type val) const noexcept -> value_type override { return TestAddress{m_addr + val}; }
        auto operator+(const_reference addr) const noexcept -> value_type override { return *this + addr.data(); }
        auto operator-(const size_type val) const noexcept -> value_type override { return TestAddress{m_addr - val}; }
        auto operator-(const_reference addr) const noexcept -> value_type override { return *this - addr.data(); }

        auto operator==(const_reference addr) const noexcept -> bool { return m_addr == addr.data(); }
        auto operator!=(const_reference addr) const noexcept -> bool { return m_addr != addr.data(); }

        auto align(const size_type alignment) noexcept -> void override
		{
            const auto mask = alignment - 1;
            m_addr = (*this + mask) & ~mask;
		}
};

TEST_CASE("Operators") {
    const auto a = 0x128;
    const auto b = 0x130;

    SECTION("Addition") {
        REQUIRE(TestAddress{a} + b == TestAddress{a + b});
        REQUIRE(TestAddress{a} + TestAddress{b} == TestAddress{a + b});
    }

    SECTION("Subtraction") {
        REQUIRE(TestAddress{b} - a == TestAddress{b - a});
        REQUIRE(TestAddress{b} - TestAddress{a} == TestAddress{b - a});
    }

    SECTION("Equality") {
        const auto addr_a1 = TestAddress{a};
        const auto addr_a2 = TestAddress{a};
        const auto addr_b = TestAddress{b};

        REQUIRE(addr_a1 == TestAddress{addr_a1.data()});
        REQUIRE(addr_a1 == addr_a2);
        REQUIRE(addr_a2 == addr_a1);

        REQUIRE(addr_a1 != addr_b);
        REQUIRE(addr_b != addr_a1);
    }

    SECTION("Bit-wise") {
        const auto addr = TestAddress{0x0ff0};

        REQUIRE((addr & 0xf00f) == TestAddress{0}.data());
        REQUIRE((addr | 0xf00f) == TestAddress{0xffff}.data());
    }
}

TEST_CASE("Aligned address") {
    auto addr = TestAddress{0x128};

    addr.align(8);
    REQUIRE(addr == TestAddress{0x128});

    addr.align(16);
    REQUIRE(addr == TestAddress{0x130});
}

TEST_CASE("Default construction") {
    REQUIRE(TestAddress{} == TestAddress{0});
}
