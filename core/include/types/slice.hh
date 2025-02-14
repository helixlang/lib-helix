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
/// \brief A string slice is a view into a string, it does not own the data, this can also be set
///        to read-only char pointers
/// \tparam _ElemT The element type of the string
/// \tparam _Size The size of the string slice (this is not the size of the string but the size of
///         the slice itself like a substring)
template <typename T>
class slice {
  private:
    using view_t = LIBCXX_NAMESPACE::basic_string_view<T>;

  public:
    // --- Type Definitions ---
    using char_t  = T;
    using slice_v = LIBCXX_NAMESPACE::vector<slice>;

    // --- Constructors ---
    constexpr slice() noexcept                   = default;
    constexpr slice(const slice &other) noexcept = default;
    constexpr slice(slice &&other) noexcept      = default;

    constexpr explicit slice(const char_t *str) noexcept
        : data(str, LIBCXX_NAMESPACE::char_traits<char_t>::length(str)) {}

    constexpr explicit slice(const char_t *str, size_t size) noexcept
        : data(str, size) {}

    constexpr explicit slice(view_t view) noexcept
        : data(view) {}

    constexpr slice &operator=(const slice &) noexcept = default;
    constexpr slice &operator=(slice &&) noexcept      = default;
    constexpr ~slice()                                 = default;

    // --- Data Access & Utility ---
    [[nodiscard]] constexpr size_t        length() const noexcept { return data.size(); }
    [[nodiscard]] constexpr bool          empty() const noexcept { return data.empty(); }
    [[nodiscard]] constexpr const char_t *raw() const noexcept { return data.data(); }
    [[nodiscard]] constexpr LIBCXX_NAMESPACE::wstring str() const {
        return LIBCXX_NAMESPACE::wstring(data);
    }

    // --- Python-like Methods ---
    [[nodiscard]] constexpr slice to_upper() const noexcept {
        LIBCXX_NAMESPACE::wstring result(data.begin(), data.end());
        for (auto &c : result)
            c = LIBCXX_NAMESPACE::towupper(c);
        return slice(result);
    }

    [[nodiscard]] constexpr slice to_lower() const noexcept {
        LIBCXX_NAMESPACE::wstring result(data.begin(), data.end());
        for (auto &c : result)
            c = LIBCXX_NAMESPACE::towlower(c);
        return slice(result);
    }

    [[nodiscard]] constexpr slice to_strip() const noexcept {
        size_t start = 0;
        size_t end   = data.size();
        while (start < end && LIBCXX_NAMESPACE::iswspace(data[start])) {
            ++start;
        }
        while (end > start && LIBCXX_NAMESPACE::iswspace(data[end - 1])) {
            --end;
        }
        return slice(data.substr(start, end - start));
    }

    [[nodiscard]] constexpr slice l_strip() const noexcept {
        size_t start = 0;
        while (start < data.size() && LIBCXX_NAMESPACE::iswspace(data[start])) {
            ++start;
        }
        return slice(data.substr(start));
    }

    [[nodiscard]] constexpr slice r_strip() const noexcept {
        size_t end = data.size();
        while (end > 0 && LIBCXX_NAMESPACE::iswspace(data[end - 1])) {
            --end;
        }
        return slice(data.substr(0, end));
    }

    [[nodiscard]] constexpr slice replace(char_t old_c, char_t new_c) const noexcept {
        LIBCXX_NAMESPACE::wstring result(data.begin(), data.end());
        for (auto &c : result) {
            if (c == old_c) {
                c = new_c;
            }
        }
        return slice(result);
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

    [[nodiscard]] constexpr bool starts_with(slice prefix) const noexcept {
        return data.starts_with(prefix.data);
    }

    [[nodiscard]] constexpr bool ends_with(slice suffix) const noexcept {
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

    [[nodiscard]] constexpr int compare(slice other) const noexcept {
        return data.compare(other.data);
    }

    friend constexpr bool operator==(const slice &lhs, const slice &rhs) noexcept {
        return lhs.data == rhs.data;
    }

    friend constexpr bool operator!=(const slice &lhs, const slice &rhs) noexcept {
        return lhs.data != rhs.data;
    }

    friend constexpr bool operator<(const slice &lhs, const slice &rhs) noexcept {
        return lhs.data < rhs.data;
    }

    friend constexpr bool operator>(const slice &lhs, const slice &rhs) noexcept {
        return lhs.data > rhs.data;
    }

    friend constexpr bool operator<=(const slice &lhs, const slice &rhs) noexcept {
        return lhs.data <= rhs.data;
    }

    friend constexpr bool operator>=(const slice &lhs, const slice &rhs) noexcept {
        return lhs.data >= rhs.data;
    }

  private:
    view_t data{};
};
}  // end namespace String

H_STD_NAMESPACE_END

// using string = String::basic<wchar_t>;
using string = LIBCXX_NAMESPACE::string;

H_NAMESPACE_END
#endif

/*
slice_v split(isize p, slice::Operation op = slice::Operation::Remove) const;
    slice_v split(char_t c, slice::Operation op = slice::Operation::Remove) const;
    slice_v split(slice s, slice::Operation op = slice::Operation::Remove) const;
    slice_v split(char_t *c, slice::Operation op = slice::Operation::Remove) const;

    // char at p and n after
    slice sub_slice(isize p, usize n) const;
    // char at p till size - p
    slice sub_slice(isize p) const;
    // r.start to r.end
    slice sub_slice(std::Range<isize> r) const;

    // char at i
    std::Questionable<char_t> get(isize i) const;

    // pos of char after n occurrences, if neg n, find in reverse, null returned if not found at n
occurrence
    // example: slice("foo bar is bad").index_of('b') == 4
    // example: slice("foo bar is bad").index_of('b', 1) == 4
    // example: slice("foo bar is bad").index_of('b', -1) == 11
    // example: slice("foo bar is bad").index_of('b', -2) == 4
    // example: slice("foo bar is bad").index_of('b', 2) == 11
    // example: slice("foo bar is bad").index_of('b', -3) == null
    // example: slice("foo bar is bad").index_of('b', 3) == null
    std::Questionable<usize> index_of(char_t c, isize n = 1) const;

    void remove_suffix();
    void remove_prefix();

    // --- String-like Operations ---
    bool  starts_with(slice &) const;
    bool  ends_with(slice &) const;
    slice strip() const;
    slice r_strip() const;
    slice l_strip() const;
*/