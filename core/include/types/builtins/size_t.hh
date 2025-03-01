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

#ifndef _$_HX_CORE_M6SIZE_T
#define _$_HX_CORE_M6SIZE_T

#include "../../config/config.h"

H_NAMESPACE_BEGIN

template <typename... Ts>
struct __BitSet;

using u64 = __BitSet<unsigned long long>;
using i64 = __BitSet<signed long long>;

#if defined(__LP64__) || defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__) ||      \
    defined(__aarch64__) || defined(__arm64__) || defined(__mips64__) || defined(__mips64) || \
    defined(__mips64el__) || defined(__mips64el) || defined(__s390x__) || defined(__HELIX_64BIT__)
#define __HELIX_64BIT__
using usize = u64;
using isize = i64;
#elif defined(__ILP32__) || defined(_WIN32) || defined(__i386__) || defined(__arm__) || \
    defined(__mips__) || defined(__mips) || defined(__mipsel__) || defined(__mipsel) || \
    defined(__s390__) || defined(__HELIX_32BIT__)
#define __HELIX_32BIT__
using usize = u32;
using isize = i32;
#elif defined(__16BIT__) || defined(__MSP430__) || defined(__AVR__) || defined(__HELIX_16BIT__)
#define __HELIX_16BIT__
using usize = u16;
using isize = i16;
#elif defined(__8BIT__) || defined(__HELIX_8BIT__)
#define __HELIX_8BIT__
using usize = u8;
using isize = i8;
#else
#error \
    "Helix core: Unable to determine platform bitness. Supported macros: __HELIX_64BIT__, __HELIX_32BIT__, __HELIX_16BIT__, __HELIX_8BIT__. Define one explicitly using '-D' during compilation."
#endif

namespace helix::std {
class null_t {};
}  // namespace helix::std

inline constexpr helix::std::null_t null;

H_STD_NAMESPACE_END

#endif  // _$_HX_CORE_M6SIZE_T
