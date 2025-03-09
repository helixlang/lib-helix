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
///-------------------------------------------------------------------------------- Lib-Helix ---///

#ifndef _$_HX_CORE_M5BASIC
#define _$_HX_CORE_M5BASIC

#include <include/config/config.h>

#include <include/meta/__interfaces/casting.hh>
#include <include/runtime/__memory/forwarding.hh>
#include <include/types/string/char_traits.hh>
#include <include/types/string/slice.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace String {
template <typename CharT, typename Traits /* defaulted in casting.hh */>
    requires CharTraits<Traits, CharT>
class basic {
  public:
    struct slice : public String::slice<CharT, Traits> {
        using String::slice<CharT, Traits>::slice;
    };

    using char_traits = Traits;
    using char_t      = CharT;
    using size_t      = usize;
    using string_t    = libcxx::basic_string<CharT, Traits>;
    using slice_t     = slice;
    using slice_vec   = vec<slice_t>;
    using char_vec    = vec<CharT>;

  private:
    string_t data;

  public:
    static constexpr size_t npos = string_t::npos;

    // Constructors
    constexpr basic() noexcept
        : data() {}
    constexpr basic(const basic &other) noexcept
        : data(other.data) {}
    constexpr basic(basic &&other) noexcept
        : data(std::Memory::move(other.data)) {}
    constexpr basic(const libcxx::basic_string<CharT, Traits> &str) noexcept
        : data(str) {}
    constexpr basic(const CharT *str) noexcept {
        static constexpr CharT e[] = {0};
        data                       = str ? str : e;
    }
    constexpr basic(const CharT chr, size_t count) noexcept
        : data(count, chr) {}
    constexpr basic(const CharT *str, size_t len) noexcept
        : data(str, len) {}
    constexpr basic(const slice_t &s) noexcept
        : data(s.raw(), s.size()) {}

    template <typename U = CharT>
    constexpr basic(const char *str,
                    typename libcxx::enable_if_t<!libcxx::is_same_v<U, char>> * = nullptr) noexcept;

    template <typename U = CharT>
    constexpr basic(const char *str,
                    usize       size,
                    typename libcxx::enable_if_t<!libcxx::is_same_v<U, char>> * = nullptr) noexcept;

    // Assignment
    basic &operator=(const basic &other) noexcept = default;
    basic &operator=(basic &&other) noexcept;
    basic &operator=(const CharT *str) noexcept;
    basic &operator=(const slice_t &s) noexcept;

    // Access Operators
    CharT &operator[](size_t index) noexcept { return data[static_cast<size_t::c_type>(index)]; }
    const CharT &operator[](size_t index) const noexcept {
        return data[static_cast<size_t::c_type>(index)];
    }

    // Mutable Methods
    void push_back(CharT c) noexcept { data.push_back(c); }
    void append(const basic &other) noexcept { data.append(other.data); }
    void append(const CharT *str, size_t len) noexcept { data.append(str, len); }
    void append(const slice_t &s) noexcept { data.append(s.raw(), s.size()); }
    void clear() noexcept { data.clear(); }
    void replace(size_t pos, size_t len, const slice_t &other) noexcept {
        data.replace(static_cast<size_t::c_type>(pos),
                     static_cast<size_t::c_type>(len),
                     libcxx::basic_string_view<CharT>(other));
    }
    void resize(size_t new_size, CharT c = CharT()) noexcept {
        data.resize(static_cast<size_t::c_type>(new_size), c);
    }

    // Concatenation Operators
    basic &operator+=(const basic &other) noexcept;
    basic &operator+=(const CharT *str) noexcept;
    basic &operator+=(const slice_t &s) noexcept;
    basic  operator+(const basic &other) const;
    basic  operator+(const CharT *str) const;
    basic  operator+(const slice_t &s) const;

    // Comparison Operators
    bool operator==(const basic &other) const noexcept { return data == other.data; }
    bool operator!=(const basic &other) const noexcept { return data != other.data; }
    bool operator<(const basic &other) const noexcept { return data < other.data; }
    bool operator>(const basic &other) const noexcept { return data > other.data; }
    bool operator<=(const basic &other) const noexcept { return data <= other.data; }
    bool operator>=(const basic &other) const noexcept { return data >= other.data; }

    // Basic Access
    const CharT *raw() const noexcept { return data.c_str(); }
    size_t       size() const noexcept { return data.size(); }
    size_t       length() const noexcept { return data.length(); }
    bool         is_empty() const noexcept { return data.empty(); }

    // Slice Conversion
    operator slice_t() const noexcept { return slice_t(data.data(), data.size()); }
    slice_t operator$cast(const slice_t * /* p */) const noexcept {
        return slice_t(data.data(), data.size());
    }
    const char_t *operator$cast(const char_t * /* p */) const noexcept { return data.data(); }

    // Copy-Returning Methods
    basic      subslice(size_t pos, size_t len) const noexcept;
    basic      l_strip(const char_vec &delim = {' ', '\t', '\n', '\r'}) const;
    basic      r_strip(const char_vec &delim = {' ', '\t', '\n', '\r'}) const;
    basic      strip(const char_vec &delim = {' ', '\t', '\n', '\r'}) const;
    vec<basic> split(const basic &delim, slice_t::Operation op = slice_t::Operation::Remove) const;
    vec<basic> split_lines() const;

    // Search
    bool contains(const basic &needle) const noexcept { return data.find(needle.data) != npos; }
    bool contains(CharT c) const noexcept { return data.find(c) != npos; }

    constexpr bool operator$contains(slice needle) const { return contains(needle); }
    constexpr bool operator$contains(wchar_t chr) const { return contains(chr); }

    std::Questionable<usize> lfind(slice needle) const;
    std::Questionable<usize> rfind(slice needle) const;
    std::Questionable<usize> find_first_of(slice needle) const;
    std::Questionable<usize> find_last_of(slice needle) const;
    std::Questionable<usize> find_first_not_of(slice needle) const;
    std::Questionable<usize> find_last_not_of(slice needle) const;

    std::Questionable<usize> lfind(slice needle, usize pos) const;
    std::Questionable<usize> rfind(slice needle, usize pos) const;
    std::Questionable<usize> find_first_of(slice needle, usize pos) const;
    std::Questionable<usize> find_last_of(slice needle, usize pos) const;
    std::Questionable<usize> find_first_not_of(slice needle, usize pos) const;
    std::Questionable<usize> find_last_not_of(slice needle, usize pos) const;
};
}  // namespace String

H_STD_NAMESPACE_END

using short_string = std::String::basic<char>;
using string       = std::String::basic<wchar_t>;

H_NAMESPACE_END

#endif  // _$_HX_CORE_M5BASIC
