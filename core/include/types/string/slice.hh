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

#ifndef _$_HX_CORE_M5SLICE
#define _$_HX_CORE_M5SLICE

#include <include/config/config.h>

#include <include/types/builtins/primitives.hh>
#include <include/types/builtins/size_t.hh>
#include <include/types/question/question_fwd.hh>
#include <include/types/string/char_traits.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace String {

template <typename CharT, typename Traits = LIBCXX_NAMESPACE::char_traits<CharT>>
    requires CharTraits<Traits, CharT>
class slice {
    using view_t = LIBCXX_NAMESPACE::basic_string_view<CharT>;

    usize  length;
    view_t data{};

  public:
    enum class Operation { Keep, Remove };

    using char_traits = Traits;
    using char_t      = CharT;
    using slice_t     = slice<CharT>;
    using size_t      = usize;
    using slice_vec   = vec<slice_t>;
    using char_vec    = vec<CharT>;

    [[nodiscard("size() returns the length of the slice's data, which is essential for bounds "
                "checking and iteration; discarding it may lead to unsafe or undefined behavior")]]
    constexpr size_t size() const noexcept;

    constexpr slice() noexcept                   = default;
    constexpr slice(const slice &other) noexcept = default;
    constexpr slice(slice &&other) noexcept      = default;
    constexpr slice(const CharT *str) noexcept;
    constexpr slice(const CharT *str, size_t size) noexcept;
    constexpr slice(view_t view) noexcept;
    constexpr slice(char_vec &vec) noexcept;
    constexpr slice(char_vec &&vec) noexcept;
    
    template <typename U = CharT>
    constexpr slice(const char* str, typename libcxx::enable_if_t<!libcxx::is_same_v<U, char>>* = nullptr) noexcept;

    template <typename U = CharT>
    constexpr slice(const char* str, usize size, typename libcxx::enable_if_t<!libcxx::is_same_v<U, char>>* = nullptr) noexcept;

    constexpr operator view_t() const noexcept { return data; }

    constexpr void exchange(slice &other) noexcept;
    constexpr void replace(slice &other) noexcept;
    constexpr void replace(CharT *str, usize size) noexcept;

    [[nodiscard("raw() returns a pointer to the slice's underlying data, essential for direct "
                "access; ignoring it may discard critical information")]]
    constexpr const CharT *raw() const noexcept {
        return data.data();
    }
    [[nodiscard("is_empty() indicates whether the slice has no data, crucial for control flow; "
                "discarding it may lead to incorrect assumptions")]]
    constexpr bool is_empty() const noexcept {
        return length == 0;
    }
    [[nodiscard("subslice() creates a view into a portion of the slice, vital for safe substring "
                "operations; ignoring it wastes the result")]]
    constexpr slice subslice(usize pos, usize len) const noexcept;

    constexpr slice l_strip(char_vec &delim = {' ', '\t', '\n', '\r'}) const;
    constexpr slice r_strip(char_vec &delim = {' ', '\t', '\n', '\r'}) const;
    constexpr slice strip(char_vec &delim = {' ', '\t', '\n', '\r'}) const;

    constexpr bool starts_with(slice &needle) const;
    constexpr bool ends_with(slice &needle) const;

    constexpr bool contains(slice &needle) const;
    constexpr bool contains(wchar_t &chr) const;

    bool operator==(const slice &other) const noexcept { return data == other.data; }
    bool operator!=(const slice &other) const noexcept { return data != other.data; }
    bool operator<(const slice &other) const noexcept { return data < other.data; }
    bool operator>(const slice &other) const noexcept { return data > other.data; }
    bool operator<=(const slice &other) const noexcept { return data <= other.data; }
    bool operator>=(const slice &other) const noexcept { return data >= other.data; }

    constexpr bool operator$contains(slice &needle) const { return contains(needle); }
    constexpr bool operator$contains(wchar_t &chr) const { return contains(chr); }

    constexpr isize compare(slice &other) const noexcept;

    [[nodiscard("split_lines() returns a vector of line views, necessary for line-based "
                "processing; discarding it neglects the parsed structure")]]
    slice_vec split_lines() const;

    [[nodiscard("split() returns a vector of views, necessary for delimeter-based processing; "
                "discarding it neglects the parsed structure")]]
    slice_vec split(slice &delim, Operation op = Operation::Remove) const;

    std::Questionable<usize> lfind(slice &needle) const;
    std::Questionable<usize> rfind(slice &needle) const;
    std::Questionable<usize> find_first_of(slice &needle) const;
    std::Questionable<usize> find_last_of(slice &needle) const;
    std::Questionable<usize> find_first_not_of(slice &needle) const;
    std::Questionable<usize> find_last_not_of(slice &needle) const;

    CharT operator[](usize index) const noexcept;
};
}  // namespace String

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M5SLICE
