//
// guest.hpp -
//

#pragma once

#include <cassert> // assert
#include <cstdint> // uint*_t
#include <filesystem> // path
#include <fstream> // fstream
#include <ios> // ios_base
#include <iostream> // ostream
#include <limits> // numeric_limits
#include <memory> // shared_ptr
#include <string> // string

#include "vmm/memory/detail/address.hpp" // Address, AddressValue

namespace vmm::memory::detail {

// A guest physical address.
//
// On aarch64, a 32-bit hypervisor may be used to support a 64-bit guest. For
// simplicity, uint64_t is used to store the raw value regardless of whether
// the guest is 32-bit or 64-bit.
class GuestAddress : public Address<GuestAddress, uint64_t>
{
    public:
        using value_type = GuestAddress;
        using size_type = uint64_t;
        using reference = value_type&;
        using const_reference = const value_type&;

        explicit GuestAddress(size_type addr=0) noexcept : m_addr{addr} {}

        auto data() const noexcept -> size_type override;

        auto operator&(const size_type mask) const noexcept -> size_type override;
        auto operator|(const size_type mask) const noexcept -> size_type override;
        auto operator+(const size_type val) const noexcept -> value_type override;
        auto operator+(const_reference addr) const noexcept -> value_type override;
        auto operator-(const size_type val) const noexcept -> value_type override;
        auto operator-(const_reference addr) const noexcept -> value_type override;

        auto align(const size_type alignment) noexcept -> reference override;
    private:
        size_type m_addr{};
};

// An offset into a memory region.
class MemoryRegionAddress : public Address<MemoryRegionAddress, uint64_t>
{
    public:
        using value_type = MemoryRegionAddress;
        using size_type = uint64_t;
        using reference = value_type&;
        using const_reference = const value_type&;

        explicit MemoryRegionAddress(size_type addr=0) noexcept : m_addr{addr} {}

        auto data() const noexcept -> size_type override;

        auto operator&(const size_type mask) const noexcept -> size_type override;
        auto operator|(const size_type mask) const noexcept -> size_type override;
        auto operator+(const size_type val) const noexcept -> value_type override;
        auto operator+(const_reference addr) const noexcept -> value_type override;
        auto operator-(const size_type val) const noexcept -> value_type override;
        auto operator-(const_reference addr) const noexcept -> value_type override;

        auto align(const size_type alignment) noexcept -> reference override;
    private:
        size_type m_addr{};
};

// The starting point of a file which backs a GuestMemoryRegion.
class FileOffset
{
    private:
        std::shared_ptr<std::fstream> m_fstream;
        long m_start{};
    public:
        FileOffset(const char* filename, std::ios_base::openmode mode,
                   const long start={})
            : m_fstream{std::make_shared<std::fstream>(filename, mode)},
              m_start{start}
        {
            if (start) {
                // TODO (?): Only call these if the appropriate openmode is set.
                m_fstream->seekg(start);
                m_fstream->seekp(start);
            }
        }

        explicit FileOffset(const char* filename, const long start={})
            : FileOffset(filename, std::ios_base::in | std::ios_base::out, start) {}

        explicit FileOffset(const std::string& filename, const long start=0)
            : FileOffset(filename.c_str(), start) {}

        FileOffset(const std::string& filename,
                   const std::ios_base::openmode mode,
                   const long start={})
            : FileOffset(filename.c_str(), mode, start) {}

        explicit FileOffset(const std::filesystem::path& filename, const long start=0)
            : FileOffset(filename.c_str(), start) {}

        FileOffset(const std::filesystem::path& filename,
                   const std::ios_base::openmode mode,
                   const long start={})
            : FileOffset(filename.c_str(), mode, start) {}

        // TODO: thread-safety
        [[nodiscard]] auto data() const -> std::basic_filebuf<std::fstream::char_type,
                                                              std::fstream::traits_type>*
        {
            return m_fstream->rdbuf();
        }

        [[nodiscard]] constexpr auto start() const noexcept -> long
        {
            return m_start;
        }
};

}  // vmm::memory::detail
