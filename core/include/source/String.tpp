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

#ifndef _$_HX_CORE_M6STRING
#define _$_HX_CORE_M6STRING

#include <include/config/config.h>
#include <include/c++/libc++.hh>
#include <include/meta/meta.hh>
#include <include/runtime/runtime.hh>
#include <include/types/types.hh>

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
inline basic<CharT, Traits> &basic<CharT, Traits>::operator=(basic &&other) noexcept {
    data = std::Memory::move(other.data);
    return *this;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline basic<CharT, Traits> &basic<CharT, Traits>::operator=(const CharT *str) noexcept {
    static constexpr CharT e[] = {0};
    data = str ? str : e;
    return *this;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline basic<CharT, Traits> &basic<CharT, Traits>::operator=(const slice_t &s) noexcept {
    data.assign(s.raw(), s.size());
    return *this;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline basic<CharT, Traits> &basic<CharT, Traits>::operator+=(const basic &other) noexcept {
    data += other.data;
    return *this;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline basic<CharT, Traits> &basic<CharT, Traits>::operator+=(const CharT *str) noexcept {
    data += str;
    return *this;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline basic<CharT, Traits> &basic<CharT, Traits>::operator+=(const CharT chr) noexcept {
    data += chr;
    return *this;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline basic<CharT, Traits> &basic<CharT, Traits>::operator+=(const slice_t &s) noexcept {
    data.append(s.raw(), s.size());
    return *this;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline basic<CharT, Traits> basic<CharT, Traits>::operator+(const basic &other) const {
    basic result = *this;
    result += other;
    return result;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline basic<CharT, Traits> basic<CharT, Traits>::operator+(const CharT *str) const {
    basic result = *this;
    result += str;
    return result;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline basic<CharT, Traits> basic<CharT, Traits>::operator+(const CharT chr) const {
    basic result = *this;
    result += chr;
    return result;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline basic<CharT, Traits> basic<CharT, Traits>::operator+(const slice_t &s) const {
    basic result = *this;
    result += s;
    return result;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
template <typename U>
basic<CharT, Traits>::basic(
    const char *str,
    typename libcxx::enable_if_t<!libcxx::is_same_v<U, char>> * = nullptr) noexcept {
    usize    size = LIBCXX_NAMESPACE::char_traits<char>::length(str);
    auto *buff = static_cast<wchar_t *>(alloca((size + 1) << String::__internal::log2_sizeof_wchar_t));

    try {
        for (usize i = 0; i < size; ++i) {
            buff[i] = static_cast<wchar_t>(static_cast<unsigned char>(str[i]));
        }
        data = buff;
    } catch (...) { data = L""; }
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
template <typename U>
basic<CharT, Traits>::basic(
    const char *str,
    usize       size,
    typename libcxx::enable_if_t<!libcxx::is_same_v<U, char>> *) noexcept {
    auto *buff = static_cast<wchar_t *>(alloca((size) << String::__internal::log2_sizeof_wchar_t));

    try {
        for (usize i = 0; i < size; ++i) {
            buff[i] = static_cast<wchar_t>(static_cast<unsigned char>(str[i]));
        }
        data = buff;
    } catch (...) { data = L""; }
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline basic<CharT, Traits> basic<CharT, Traits>::subslice(size_t pos, size_t len) const noexcept {
    return basic(
        data.substr(static_cast<size_t>(pos), static_cast<size_t>(len)).c_str());
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline basic<CharT, Traits> basic<CharT, Traits>::l_strip(const char_vec &delim) const {
    size_t start = 0;
    while (start < size() &&
           libcxx::find(delim.begin(), delim.end(), operator[](start)) != delim.end()) {
        ++start;
    }
    return subslice(start, size() - start);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline basic<CharT, Traits> basic<CharT, Traits>::r_strip(const char_vec &delim) const {
    size_t end = size();
    while (end > 0 &&
           libcxx::find(delim.begin(), delim.end(), operator[](end - 1)) != delim.end()) {
        --end;
    }
    return subslice(0, end);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline basic<CharT, Traits> basic<CharT, Traits>::strip(const char_vec &delim) const {
    size_t start = 0;
    size_t end   = size();
    while (start < end &&
           libcxx::find(delim.begin(), delim.end(), operator[](start)) != delim.end()) {
        ++start;
    }
    while (end > start &&
           libcxx::find(delim.begin(), delim.end(), operator[](end - 1)) != delim.end()) {
        --end;
    }
    return subslice(start, end - start);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline vec<basic<CharT, Traits>> basic<CharT, Traits>::split(const basic       &delim,
                                                             slice_t::Operation op) const {
    vec<basic> result;
    size_t     start      = 0;
    size_t     end        = 0;
    size_t     delim_size = delim.size();
    while (end <= size()) {
        if (end + delim_size <= size() && subslice(end, delim_size) == delim) {
            if (op == slice_t::Operation::Keep) {
                result.push_back(subslice(start, end - start));
                result.push_back(delim);
            } else {
                result.push_back(subslice(start, end - start));
            }
            start = end + delim_size;
            end   = start;
        } else {
            ++end;
        }
    }
    if (start < size()) {
        result.push_back(subslice(start, size() - start));
    }
    return result;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline vec<basic<CharT, Traits>> basic<CharT, Traits>::split_lines() const {
    vec<basic> result;
    size_t     start = 0;
    size_t     end   = 0;
    while (end < size()) {
        if (operator[](end) == '\n') {
            result.push_back(subslice(start, end - start));
            start = end + 1;
        } else if (operator[](end) == '\r') {
            if (end + 1 < size() && operator[](end + 1) == '\n') {
                result.push_back(subslice(start, end - start));
                start = end + 2;
                end += 1;
            } else {
                result.push_back(subslice(start, end - start));
                start = end + 1;
            }
        }
        ++end;
    }
    if (start < size()) {
        result.push_back(subslice(start, size() - start));
    }
    return result;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline std::Questionable<usize> basic<CharT, Traits>::lfind(slice needle) const {
    return slice_t(data.data(), data.size()).lfind(needle);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline std::Questionable<usize> basic<CharT, Traits>::rfind(slice needle) const {
    return slice_t(data.data(), data.size()).rfind(needle);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline std::Questionable<usize> basic<CharT, Traits>::find_first_of(slice needle) const {
    return slice_t(data.data(), data.size()).find_first_of(needle);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline std::Questionable<usize> basic<CharT, Traits>::find_last_of(slice needle) const {
    return slice_t(data.data(), data.size()).find_last_of(needle);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline std::Questionable<usize> basic<CharT, Traits>::find_first_not_of(slice needle) const {
    return slice_t(data.data(), data.size()).find_first_not_of(needle);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline std::Questionable<usize> basic<CharT, Traits>::find_last_not_of(slice needle) const {
    return slice_t(data.data(), data.size()).find_last_not_of(needle);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline std::Questionable<usize> basic<CharT, Traits>::lfind(slice needle, usize pos) const {
    auto s = data.find(needle, pos);
    return s == string_t::npos ? std::Questionable<usize>() : std::Questionable<usize>(s);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline std::Questionable<usize> basic<CharT, Traits>::rfind(slice needle, usize pos) const {
    auto s = data.rfind(needle, pos);
    return s == string_t::npos ? std::Questionable<usize>() : std::Questionable<usize>(s);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline std::Questionable<usize> basic<CharT, Traits>::find_first_of(slice needle, usize pos) const {
    auto s = data.find_first_of(needle, pos);
    return s == string_t::npos ? std::Questionable<usize>() : std::Questionable<usize>(s);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline std::Questionable<usize> basic<CharT, Traits>::find_last_of(slice needle, usize pos) const {
    auto s = data.find_last_of(needle, pos);
    return s == string_t::npos ? std::Questionable<usize>() : std::Questionable<usize>(s);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline std::Questionable<usize> basic<CharT, Traits>::find_first_not_of(slice needle, usize pos) const {
    auto s = data.find_first_not_of(needle, pos);
    return s == string_t::npos ? std::Questionable<usize>() : std::Questionable<usize>(s);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline std::Questionable<usize> basic<CharT, Traits>::find_last_not_of(slice needle, usize pos) const {
    auto s = data.find_last_not_of(needle, pos);
    return s == string_t::npos ? std::Questionable<usize>() : std::Questionable<usize>(s);
}

template class basic<char>;
template class basic<wchar_t>;
}  // namespace String

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M6STRING