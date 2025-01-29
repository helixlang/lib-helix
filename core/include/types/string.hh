///--- The Helix Project ------------------------------------------------------------------------///
///                                                                                              ///
///   Part of the Helix Project, under the Attribution 4.0 International license (CC BY 4.0).    ///
///   You are allowed to use, modify, redistribute, and create derivative works, even for        ///
///   commercial purposes, provided that you give appropriate credit, and indicate if changes    ///
///   were made.                                                                                 ///
///                                                                                              ///
///   For more information on the license terms and requirements, please visit:                  ///
///     https://creativecommons.org/licenses/by/4.0/                                             ///
///                                                                                              ///
///   SPDX-License-Identifier: CC-BY-4.0                                                         ///
///   Copyright (c) 2024 The Helix Project (CC BY 4.0)                                           ///
///                                                                                              ///
///------------------------------------------------------------------------------------ Helix ---///
/// Ideology: make sure this is highly optimized, a basic string must have the following grantees:
/// 1. const char*'s that point to the ROM of the file must not be moved to the heap unless modifed
///    even then to optimize the performance of strings, the allocation model should follow that of
///    a slab allocator, unless the string is truncated. In the case of trunaction the whole ROM
///    string must be moved to the heap and resized accordingly.
///
/// 2. char*'s that point to heap memory should be copied to a new location on the heap owned by
///    'string' instead of the original owner, this is to avoid any issue of dangling pointers or
///    use after frees
///
/// 3. string class maintains a record of the size of the string, in the case of an operation that
///    requires the O(n) operarion of incrementing though the entire string, there is an implicit
///    null char check to validate the string size matches the size of the internal pointer.
///
/// 4. string concat operations between 2 strings, just point the pointer of the last string to the
///    data of the new string (this is to avoid reallocation, while allowing full functionality)

#ifndef __$LIBHELIX_STRING__
#define __$LIBHELIX_STRING__

#include "../config.h"
#include "../lang/generator.hh"
#include "../libc.h"
#include "../libcxx.h"
#include "../memory.h"
#include "../meta.h"
#include "../primitives.h"

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN
namespace Interface {
template <typename self>
concept CharaterCompliance = requires(self inst, self other, char c, int i) {
    { ++inst } -> std::Meta::convertible_to<self>;
    { inst++ } -> std::Meta::convertible_to<self>;
    { --inst } -> std::Meta::convertible_to<self>;
    { inst-- } -> std::Meta::convertible_to<self>;

    { inst == other } -> std::Meta::convertible_to<bool>;
    { inst != other } -> std::Meta::convertible_to<bool>;
    { inst < other } -> std::Meta::convertible_to<bool>;
    { inst > other } -> std::Meta::convertible_to<bool>;
    { inst <= other } -> std::Meta::convertible_to<bool>;
    { inst >= other } -> std::Meta::convertible_to<bool>;

    { inst = other } -> std::Meta::same_as<self &>;

    { static_cast<char>(inst) } -> std::Meta::same_as<char>;
    { static_cast<int>(inst) } -> std::Meta::same_as<int>;

    { inst + i } -> std::Meta::convertible_to<self>;
    { inst - i } -> std::Meta::convertible_to<self>;
    { i + inst } -> std::Meta::convertible_to<self>;

    // eval if needed for char
    { inst - other } -> std::Meta::convertible_to<int>;

    { inst += i } -> std::Meta::same_as<self &>;
    { inst -= i } -> std::Meta::same_as<self &>;
};
}  // end namespace Interface
H_STD_NAMESPACE_END

namespace String {
/// \class string_slice
/// \brief A string slice is a view into a string, it does not own the data, this can also be set
///        to read-only char pointers
/// \tparam _ElemT The element type of the string
/// \tparam _Size The size of the string slice (this is not the size of the string but the size of
///         the slice itself like a substring)
template <typename CharT>
class slice {
  private:
    mutable helix::$generator<CharT> $gen_state = operator$generator();

  public:
    using value_type      = CharT;
    using size_type       = usize;
    using difference_type = isize;

    using reference       = const CharT &;
    using const_reference = const CharT &;

    using pointer       = const CharT *;
    using const_pointer = const CharT *;

    constexpr slice() noexcept;
    constexpr slice(const CharT *data, size_type size) noexcept;
    constexpr slice(const slice &) noexcept = default;
    constexpr slice(slice &&) noexcept      = default;

    constexpr slice &operator=(const slice &) noexcept = default;
    constexpr slice &operator=(slice &&) noexcept      = default;

    constexpr reference operator[](size_type pos) const noexcept;
    constexpr reference at(size_type pos) const;
    constexpr reference front() const noexcept;
    constexpr reference back() const noexcept;
    constexpr pointer   data() const noexcept;

    [[nodiscard]] constexpr bool      empty() const noexcept;
    [[nodiscard]] constexpr size_type size() const noexcept;
    [[nodiscard]] constexpr size_type length() const noexcept;

    // constexpr const_iterator begin() const noexcept;
    // constexpr const_iterator cbegin() const noexcept;
    // constexpr const_iterator end() const noexcept;
    // constexpr const_iterator cend() const noexcept;
    // constexpr const_reverse_iterator rbegin() const noexcept;
    // constexpr const_reverse_iterator crbegin() const noexcept;
    // constexpr const_reverse_iterator rend() const noexcept;
    // constexpr const_reverse_iterator crend() const noexcept;

    inline auto operator$generator() -> helix::$generator<CharT> {

    }
    
    auto iter() -> helix::$generator<CharT> { return operator$generator(); }
    auto begin() { return $gen_state.begin(); }
    auto cbegin() { return $gen_state.cbegin(); }
    auto end() { return $gen_state.end(); }
    auto cend() { return $gen_state.cend(); }

    constexpr slice split(size_type pos, size_type count) const;
    constexpr int   compare(const slice &other) const noexcept;

    constexpr void left_strip(size_type n) noexcept;
    constexpr void right_strip(size_type n) noexcept;

    friend constexpr bool operator==(const slice &lhs, const slice &rhs) noexcept {
        return lhs.size() == rhs.size() && LIBCXX_NAMESPACE::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    friend constexpr bool operator!=(const slice &lhs, const slice &rhs) noexcept {
        return !(lhs == rhs);
    }

    friend constexpr bool operator<(const slice &lhs, const slice &rhs) noexcept {
        return LIBCXX_NAMESPACE::lexicographical_compare(
            lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    friend constexpr bool operator<=(const slice &lhs, const slice &rhs) noexcept {
        return !(rhs < lhs);
    }

    friend constexpr bool operator>(const slice &lhs, const slice &rhs) noexcept {
        return rhs < lhs;
    }

    friend constexpr bool operator>=(const slice &lhs, const slice &rhs) noexcept {
        return !(lhs < rhs);
    }

  private:
    const_pointer m_data;
    size_type     m_size;
};

template <typename CharT, const usize _SlabSize = 16>  // NOLINT
    requires(std::Interface::CharaterCompliance<CharT>)
class basic {
  private:
    enum StorageLocation { Stack, ROM, Heap };

    basic(const CharT *rom_ptr);
    basic(CharT *rom_ptr);
    basic(basic<CharT> rom_ptr);
};

}  // namespace String

using string = String::basic<wchar_t>;

H_NAMESPACE_END
#endif