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

#ifndef __$LIBHELIX_TYPES__
#define __$LIBHELIX_TYPES__

using byte = unsigned char;

using u8  = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using i8  = signed char;
using i16 = signed short;
using i32 = signed int;
using i64 = signed long long;

using f32 = float;
using f64 = double;
using f80 = long double;

#if defined(__LP64__)     || defined(_WIN64)     || defined(__x86_64__) || defined(__ppc64__) || \
    defined(__aarch64__)  || defined(__arm64__)  || defined(__mips64__) || defined(__mips64)  || \
    defined(__mips64el__) || defined(__mips64el) || defined(__s390x__)
using usize = u64;
using isize = i64;
#define __HELIX_64BIT__
#elif defined(__ILP32__) || defined(_WIN32) || defined(__i386__)   || defined(__arm__)  || \
    defined(__mips__)    || defined(__mips) || defined(__mipsel__) || defined(__mipsel) || \
    defined(__s390__)
using usize = u32;
using isize = i32;
#define __HELIX_32BIT__
#elif defined(__16BIT__) || defined(__MSP430__) || defined(__AVR__)
using usize = u16;
using isize = i16;
#define __HELIX_16BIT__
#elif defined(__8BIT__)
using usize = u8;
using isize = i8;
#define __HELIX_8BIT__
#else
#error \
    "Helix core: Unable to determine platform bitness. Supported macros: __LP64__, __ILP32__, __16BIT__, __8BIT__. Define one explicitly using '-D' during compilation."
#endif

// ensure that the sizes are correct and match the platform
static_assert(sizeof(usize) == sizeof(void *), "usize must match the size of a pointer.");
static_assert(sizeof(isize) == sizeof(void *), "isize must match the size of a pointer.");

#endif  // __$LIBHELIX_TYPES__