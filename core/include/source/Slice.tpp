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

#ifndef _$_HX_CORE_M5SLICE_TPP
#define _$_HX_CORE_M5SLICE_TPP

#include <include/config/config.h>

#include <include/c++/libc++.hh>
#include <include/meta/meta.hh>
#include <include/types/types.hh>
#include <include/runtime/runtime.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace String::__internal {
namespace {
    constexpr static usize log2_sizeof_wchar_t = static_cast<usize>(sizeof(wchar_t) > 1) +
                                                 static_cast<usize>(sizeof(wchar_t) > 2) +
                                                 static_cast<usize>(sizeof(wchar_t) > 4);
}
}  // namespace String::__internal

namespace String {
template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits>::slice(const CharT *str) noexcept
    : length(str ? Traits::length(str) : 0)
    , data(str, length) {}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits>::slice(const CharT *str, size_t size) noexcept
    : length(size)
    , data(str, length) {}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits>::slice(view_t view) noexcept
    : length(view.size())
    , data(view) {}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits>::slice(char_vec &vec) noexcept
    : length(vec.size())
    , data(vec.data(), length) {}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits>::slice(char_vec &&vec) noexcept
    : length(vec.size())
    , data(std::Memory::move(vec).data(), length) {}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
template <typename U>
slice<CharT, Traits>::slice(const char *str,
                            typename libcxx::enable_if_t<!libcxx::is_same_v<U, char>> *) noexcept {
    usize    size = LIBCXX_NAMESPACE::char_traits<char>::length(str);
    wchar_t *buff =
        static_cast<wchar_t *>(alloca((size + 1) << String::__internal::log2_sizeof_wchar_t));

    try {
        for (usize i = 0; i < size; ++i) {
            buff[i] = static_cast<wchar_t>(static_cast<unsigned char>(str[i]));
        }
        this->replace(buff, size);
    } catch (...) { this->replace((wchar_t *)nullptr, 1_usize); }
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
template <typename U>
slice<CharT, Traits>::slice(const char *str,
                            usize       size,
                            typename libcxx::enable_if_t<!libcxx::is_same_v<U, char>> *) noexcept {
    wchar_t *buff =
        static_cast<wchar_t *>(alloca((size) << String::__internal::log2_sizeof_wchar_t));

    try {
        for (usize i = 0; i < size; ++i) {
            buff[i] = static_cast<wchar_t>(static_cast<unsigned char>(str[i]));
        }
        this->replace(buff, size);
    } catch (...) { this->replace((wchar_t *)nullptr, 1_usize); }
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
void slice<CharT, Traits>::exchange(slice &other) noexcept {
    view_t tmp     = this->data;
    this->data     = other.data;
    other.data     = tmp;
    size_t tmp_len = this->length;
    this->length   = other.length;
    other.length   = tmp_len;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
void slice<CharT, Traits>::replace(slice &other) noexcept {
    this->data   = other.data;
    this->length = other.length;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
void slice<CharT, Traits>::replace(CharT *str, usize size) noexcept {
    this->data   = view_t(str, size);
    this->length = size;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits> slice<CharT, Traits>::subslice(usize pos, usize len) const noexcept {
    return slice_t(data.substr(pos, len));
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits> slice<CharT, Traits>::l_strip(char_vec &delim) const {
    usize start = 0;
    usize end   = length;
    while (start < end &&
           libcxx::find(delim.begin(), delim.end(), this->operator[](start)) != delim.end()) {
        ++start;
    }
    return slice_t(subslice(start, end - start));
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits> slice<CharT, Traits>::r_strip(char_vec &delim) const {
    usize start = 0;
    usize end   = length;
    while (end > start &&
           libcxx::find(delim.begin(), delim.end(), this->operator[](end - 1)) != delim.end()) {
        --end;
    }
    return slice_t(subslice(start, end - start));
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits> slice<CharT, Traits>::strip(char_vec &delim) const {
    return l_strip(delim).r_strip(delim);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
bool slice<CharT, Traits>::starts_with(slice &needle) const {
    return length >= needle.size() && subslice(0, needle.size()) == needle;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
bool slice<CharT, Traits>::ends_with(slice &needle) const {
    return length >= needle.size() && subslice(length - needle.size(), needle.size()) == needle;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
bool slice<CharT, Traits>::contains(slice &needle) const {
    return data.find(needle) != view_t::npos;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
bool slice<CharT, Traits>::contains(wchar_t &chr) const {
    return data.find(chr) != view_t::npos;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
isize slice<CharT, Traits>::compare(slice &other) const noexcept {
    return Traits::compare(data.data(), other.data.data(), length);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits>::slice_vec slice<CharT, Traits>::split_lines() const {
    slice_vec result;
    bool      encoding_CR = false;
    usize     start       = 0;
    usize     end         = 0;
    while (end < length) {
        if (this->operator[](end) == '\r') {
            encoding_CR = true;
        } else if (this->operator[](end) == '\n') {
            if (encoding_CR) {
                result.push_back(subslice(start, end - start - 1));
                encoding_CR = false;
            } else {
                result.push_back(subslice(start, end - start));
            }
            start = end + 1;
        }
        ++end;
    }
    if (start < end) {
        result.push_back(subslice(start, end - start));
    }
    return result;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits>::slice_vec slice<CharT, Traits>::split(slice &delim, Operation op) const {
    slice_vec result;
    usize     start = 0;
    usize     end   = 0;
    while (end < length) {
        if (subslice(end, delim.size()) == delim) {
            if (op == Operation::Keep) {
                result.push_back(subslice(start, end - start));
                result.push_back(delim);
            } else {
                result.push_back(subslice(start, end - start));
            }
            end += delim.size();
            start = end;
        } else {
            ++end;
        }
    }
    if (start < end) {
        result.push_back(subslice(start, end - start));
    }
    return result;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
std::Questionable<usize> slice<CharT, Traits>::lfind(slice &needle) const {
    usize pos = data.find(needle);
    return pos == view_t::npos ? std::Questionable<usize>() : std::Questionable<usize>(pos);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
std::Questionable<usize> slice<CharT, Traits>::rfind(slice &needle) const {
    usize pos = data.rfind(needle);
    return pos == view_t::npos ? std::Questionable<usize>() : std::Questionable<usize>(pos);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
std::Questionable<usize> slice<CharT, Traits>::find_first_of(slice &needle) const {
    usize pos = data.find_first_of(needle);
    return pos == view_t::npos ? std::Questionable<usize>() : std::Questionable<usize>(pos);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
std::Questionable<usize> slice<CharT, Traits>::find_last_of(slice &needle) const {
    usize pos = data.find_last_of(needle);
    return pos == view_t::npos ? std::Questionable<usize>() : std::Questionable<usize>(pos);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
std::Questionable<usize> slice<CharT, Traits>::find_first_not_of(slice &needle) const {
    usize pos = data.find_first_not_of(needle);
    return pos == view_t::npos ? std::Questionable<usize>() : std::Questionable<usize>(pos);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
std::Questionable<usize> slice<CharT, Traits>::find_last_not_of(slice &needle) const {
    usize pos = data.find_last_not_of(needle);
    return (pos == view_t::npos) ? std::Questionable<usize>() : std::Questionable<usize>(pos);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
CharT slice<CharT, Traits>::operator[](usize index) const noexcept {
    return data[static_cast<usize>(index)];
}

template class slice<char>;
template class slice<wchar_t>;
}  // namespace String

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M5SLICE