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

#ifndef _$_HX_CORE_M8LITERALS
#define _$_HX_CORE_M8LITERALS

#include "../../config/config.h"
#include "num_data.hh"
#include "primitives.hh"

H_NAMESPACE_BEGIN

namespace {
constexpr i128 u128_to_i28(const u128 &x) noexcept {
    // For u128, x.head is of type u64 (i.e. __BitSet<unsigned long long>)
    // and x.tail is also of type u64.
    // We extract their underlying values and cast them to signed long long.
    return {static_cast<signed long long>(x.head.value),
            static_cast<signed long long>(x.tail.value)};
}
}  // namespace

constexpr u8 operator"" _u8(unsigned long long v) noexcept
    DIAGNOSE_IF(((__builtin_constant_p(v)) && ((v < 0) || (v > __NumData<unsigned char>::max)),
                 "Literal value too large for u8",
                 "warning")) {
    return {static_cast<unsigned char>(v)};
}
constexpr i8 operator"" _i8(unsigned long long v) noexcept
    DIAGNOSE_IF(((__builtin_constant_p(v)) &&
                     ((v > __NumData<signed char>::max) || (v < __NumData<signed char>::min)),
                 "Literal value too large for i8",
                 "warning")) {
    return {static_cast<signed char>(v)};
}

constexpr u16 operator"" _u16(const unsigned long long v) noexcept
    DIAGNOSE_IF(((__builtin_constant_p(v)) && ((v < 0) || (v > __NumData<unsigned short>::max)),
                 "Literal value too large for u16",
                 "warning")) {
    return {static_cast<unsigned short>(v)};
}
constexpr i16 operator"" _i16(unsigned long long v) noexcept
    DIAGNOSE_IF(((__builtin_constant_p(v)) &&
                     ((v > __NumData<signed short>::max) || (v < __NumData<signed short>::min)),
                 "Literal value too large for i16",
                 "warning")) {
    return {static_cast<signed short>(v)};
}

constexpr u32 operator"" _u32(unsigned long long v) noexcept
    DIAGNOSE_IF(((__builtin_constant_p(v)) && ((v < 0) || (v > __NumData<unsigned int>::max)),
                 "Literal value too large for u32",
                 "warning")) {
    return {static_cast<unsigned int>(v)};
}
constexpr i32 operator"" _i32(unsigned long long v) noexcept
    DIAGNOSE_IF(((__builtin_constant_p(v)) &&
                     ((v > __NumData<signed int>::max) || (v < __NumData<signed int>::min)),
                 "Literal value too large for i32",
                 "warning")) {
    return {static_cast<signed int>(v)};
}

constexpr u64 operator"" _u64(unsigned long long v) noexcept
    DIAGNOSE_IF(((__builtin_constant_p(v)) && ((v < 0) || (v > __NumData<unsigned long long>::max)),
                 "Literal value too large for u32",
                 "warning")) {
    return {v};
}

constexpr i64 operator"" _i64(unsigned long long v) noexcept
    DIAGNOSE_IF(((__builtin_constant_p(v)) && ((v > __NumData<signed long long>::max) ||
                                               (v < __NumData<signed long long>::min)),
                 "Literal value too large for i32",
                 "warning")) {
    return {static_cast<signed long long>(v)};
}

constexpr u128 operator"" _u128(const char *str) noexcept {
    const size_t len = std::Reflection::cstr_length(str);
    return u128::from_cstr(str, len);
}

constexpr i128 operator"" _i128(const char *str) noexcept {
    const size_t  len      = std::Reflection::cstr_length(str);
    bool          negative = false;
    unsigned long start    = 0;

    if (len > 0 && str[0] == '-') {
        negative = true;
        start    = 1;
    }

    u128 abs_val    = u128::from_cstr(str + start, len - start);
    i128 signed_val = u128_to_i28(abs_val);

    return negative ? (i128() - signed_val) : signed_val;
}

constexpr f32 operator"" _f32(long double v) noexcept { return {static_cast<float>(v)}; }

constexpr f64 operator"" _f64(long double v) noexcept { return {static_cast<double>(v)}; }

constexpr f80 operator"" _f80(long double v) noexcept { return {v}; }

// FIXME: add llvm level support for this
// constexpr f128 operator"" _f128(unsigned long long v) noexcept {
//     return {static_cast<signed long long>(v)};
// }

H_STD_NAMESPACE_END

#endif  // _$_HX_CORE_M8LITERALS
