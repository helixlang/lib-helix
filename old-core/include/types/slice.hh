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

#ifndef __$LIBHELIX_SLICE__
#define __$LIBHELIX_SLICE__

#include "../config.h"
#include "../interfaces.h"
#include "../lang/generator.hh"
#include "../lang/question.hh"
#include "../libc.h"
#include "../memory.h"
#include "../meta.h"
#include "../primitives.h"
#include "forward.hh"

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

template <typename T>
    requires std::Interface::RangeCompliant<T>
class Range;

namespace String {
/// \class string_slice
/// \brief A string basic_slice is a view into a string, it does not own the data, this can also be set
///        to read-only char pointers
/// \tparam _ElemT The element type of the string
/// \tparam _Size The size of the string basic_slice (this is not the size of the string but the size of
///         the basic_slice itself like a substring)
template <typename T>
class basic_slice {
  private:
    using view_t = LIBCXX_NAMESPACE::basic_string_view<T>;

  public:
    // --- Type Definitions ---
    using char_t  = T;
    using slice_v = LIBCXX_NAMESPACE::vector<basic_slice>;

    // --- Constructors ---
    constexpr basic_slice() noexcept                   = default;
    constexpr basic_slice(const basic_slice &other) noexcept = default;
    constexpr basic_slice(basic_slice &&other) noexcept      = default;

    constexpr explicit basic_slice(const char_t *str) noexcept
        : data(str, LIBCXX_NAMESPACE::char_traits<char_t>::length(str)) {}

    constexpr explicit basic_slice(const char_t *str, size_t size) noexcept
        : data(str, size) {}

    constexpr explicit basic_slice(view_t view) noexcept
        : data(view) {}

    constexpr basic_slice &operator=(const basic_slice &) noexcept = default;
    constexpr basic_slice &operator=(basic_slice &&) noexcept      = default;
    constexpr ~basic_slice()                                 = default;

    // --- Data Access & Utility ---
    [[nodiscard]] constexpr size_t        length() const noexcept { return data.size(); }
    [[nodiscard]] constexpr bool          empty() const noexcept { return data.empty(); }
    [[nodiscard]] constexpr const char_t *raw() const noexcept { return data.data(); }

    // --- Python-like Methods ---
    [[nodiscard]] constexpr basic_slice to_strip() const noexcept {
        size_t start = 0;
        size_t end   = data.size();
        while (start < end && LIBCXX_NAMESPACE::iswspace(data[start])) {
            ++start;
        }
        while (end > start && LIBCXX_NAMESPACE::iswspace(data[end - 1])) {
            --end;
        }
        return basic_slice(data.substr(start, end - start));
    }

    [[nodiscard]] constexpr basic_slice l_strip() const noexcept {
        size_t start = 0;
        while (start < data.size() && LIBCXX_NAMESPACE::iswspace(data[start])) {
            ++start;
        }
        return basic_slice(data.substr(start));
    }

    [[nodiscard]] constexpr basic_slice r_strip() const noexcept {
        size_t end = data.size();
        while (end > 0 && LIBCXX_NAMESPACE::iswspace(data[end - 1])) {
            --end;
        }
        return basic_slice(data.substr(0, end));
    }

    [[nodiscard]] constexpr slice_v split(char_t delim) const noexcept {
        slice_v parts;
        size_t  start = 0;
        while (true) {
            size_t pos = data.find(delim, start);
            if (pos == view_t::npos) {
                break;
            }
            parts.emplace_back(data.substr(start, pos - start));
            start = pos + 1;
        }
        parts.emplace_back(data.substr(start));
        return parts;
    }

    [[nodiscard]] constexpr std::Questionable<size_t> find(char_t c) const noexcept {
        size_t pos = data.find(c);
        return pos == view_t::npos ? null : pos;
    }

    [[nodiscard]] constexpr bool starts_with(basic_slice prefix) const noexcept {
        return data.starts_with(prefix.data);
    }

    [[nodiscard]] constexpr bool ends_with(basic_slice suffix) const noexcept {
        return data.ends_with(suffix.data);
    }

    // --- Operators ---
    [[nodiscard]] constexpr std::Questionable<char_t> operator[](size_t i) const noexcept {
        return get(i);
    }

    [[nodiscard]] constexpr std::Questionable<char_t> get(size_t i) const noexcept {
        if (i >= data.size()) {
            return null;
        }

        return data[i];
    }

    [[nodiscard]] constexpr int compare(basic_slice other) const noexcept {
        return data.compare(other.data);
    }

    friend constexpr bool operator==(const basic_slice &lhs, const basic_slice &rhs) noexcept {
        return lhs.data == rhs.data;
    }

    friend constexpr bool operator!=(const basic_slice &lhs, const basic_slice &rhs) noexcept {
        return lhs.data != rhs.data;
    }

    friend constexpr bool operator<(const basic_slice &lhs, const basic_slice &rhs) noexcept {
        return lhs.data < rhs.data;
    }

    friend constexpr bool operator>(const basic_slice &lhs, const basic_slice &rhs) noexcept {
        return lhs.data > rhs.data;
    }

    friend constexpr bool operator<=(const basic_slice &lhs, const basic_slice &rhs) noexcept {
        return lhs.data <= rhs.data;
    }

    friend constexpr bool operator>=(const basic_slice &lhs, const basic_slice &rhs) noexcept {
        return lhs.data >= rhs.data;
    }

  private:
    view_t data{};
};

using wide_slice = basic_slice<wchar_t>;
}  // end namespace String

H_STD_NAMESPACE_END
H_NAMESPACE_END
#endif

/*
slice_v split(isize p, basic_slice::Operation op = basic_slice::Operation::Remove) const;
    slice_v split(char_t c, basic_slice::Operation op = basic_slice::Operation::Remove) const;
    slice_v split(basic_slice s, basic_slice::Operation op = basic_slice::Operation::Remove) const;
    slice_v split(char_t *c, basic_slice::Operation op = basic_slice::Operation::Remove) const;

    // char at p and n after
    basic_slice sub_slice(isize p, usize n) const;
    // char at p till size - p
    basic_slice sub_slice(isize p) const;
    // r.start to r.end
    basic_slice sub_slice(std::Range<isize> r) const;

    // char at i
    std::Questionable<char_t> get(isize i) const;

    // pos of char after n occurrences, if neg n, find in reverse, null returned if not found at n
occurrence
    // example: basic_slice("foo bar is bad").index_of('b') == 4
    // example: basic_slice("foo bar is bad").index_of('b', 1) == 4
    // example: basic_slice("foo bar is bad").index_of('b', -1) == 11
    // example: basic_slice("foo bar is bad").index_of('b', -2) == 4
    // example: basic_slice("foo bar is bad").index_of('b', 2) == 11
    // example: basic_slice("foo bar is bad").index_of('b', -3) == null
    // example: basic_slice("foo bar is bad").index_of('b', 3) == null
    std::Questionable<usize> index_of(char_t c, isize n = 1) const;

    void remove_suffix();
    void remove_prefix();

    // --- String-like Operations ---
    bool  starts_with(basic_slice &) const;
    bool  ends_with(basic_slice &) const;
    basic_slice strip() const;
    basic_slice r_strip() const;
    basic_slice l_strip() const;
*/