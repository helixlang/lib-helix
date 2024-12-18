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

#ifndef __$LIBHELIX_CONCEPTS__
#define __$LIBHELIX_CONCEPTS__

#include "concepts.h"
#include "config.h"
#include "dtypes.h"
#include "traits.h"

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN
namespace concepts {

template <typename T>
concept HasToString = requires(T a) {
    { a.to_string() } -> LIBCXX_NAMESPACE::convertible_to<string>;
};

template <typename T>
concept SupportsOStream = requires(LIBCXX_NAMESPACE::ostream &os, T a) {
    { os << a } -> H_STD_NAMESPACE::traits::convertible_to<LIBCXX_NAMESPACE::ostream &>;
};

template <typename _Ty, typename _Up>
concept SupportsPointerCast = requires(_Ty from) {
    dynamic_cast<_Up>(from);  // Dynamic cast requirement
};

template <typename T, typename U>
concept SafelyCastable = requires(T t, U *u) {
    { t.$cast(u) } -> H_STD_NAMESPACE::traits::same_as<string>;
};

template <typename T>
concept ConvertibleToString = HasToString<T> || SupportsOStream<T> || SafelyCastable<T, string>;

}  // namespace concepts
H_STD_NAMESPACE_END
H_NAMESPACE_END
#endif