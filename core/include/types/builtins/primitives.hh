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

#ifndef _$_HX_CORE_M10PRIMITIVES
#define _$_HX_CORE_M10PRIMITIVES

#include "../../config/config.h"
#include "bit_set.hh"

H_NAMESPACE_BEGIN

using u8 = __BitSet<unsigned char>;
using i8 = __BitSet<signed char>;

using u16 = __BitSet<unsigned short>;
using i16 = __BitSet<signed short>;

using u32 = __BitSet<unsigned int>;
using i32 = __BitSet<signed int>;

using u64 = __BitSet<unsigned long long>;
using i64 = __BitSet<signed long long>;

using u128 = __BitSet<u64, u64>;
using i128 = __BitSet<i64, i64>;

using f32 = __BitSet<float>;
using f64 = __BitSet<double>;
using f80 = __BitSet<long double>;

H_STD_NAMESPACE_END

#endif  // _$_HX_CORE_M10PRIMITIVES
