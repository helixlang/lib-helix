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

#include <include/config/config.h>
#include <include/types/builtins/bitset.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

class null_t {};
H_STD_NAMESPACE_END

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

inline constexpr helix::std::null_t null;

// FIXME: remove later
template <typename T>
using vec = helix::libcxx::vector<T>;

template <typename K, typename V>
using map = helix::libcxx::unordered_map<K, V>;

template <typename T, helix::usize S>
using array = helix::libcxx::array<T, static_cast<helix::usize::c_type>(S)>;

template <typename T>
using list = helix::libcxx::list<T>;

template <typename T>
using set = helix::libcxx::set<T>;
H_NAMESPACE_END

#endif  // _$_HX_CORE_M10PRIMITIVES
