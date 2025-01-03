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

#include "config.h"
#include "types.h"
#include "traits.h"

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN
namespace concepts {

template <typename _Ty>
concept HasToString = requires(_Ty a) {
    { a.to_string() } -> LIBCXX_NAMESPACE::convertible_to<string>;
};

template <typename _Ty>
concept SupportsOStream = requires(LIBCXX_NAMESPACE::ostream &os, _Ty a) {
    { os << a } -> H_STD_NAMESPACE::traits::convertible_to<LIBCXX_NAMESPACE::ostream &>;
};

template <typename _Ty, typename _Up>
concept SupportsPointerCast = requires(_Ty from) {
    dynamic_cast<_Up>(from);  // Dynamic cast requirement
};

template <typename _Ty, typename _Up>
concept SafelyCastable = requires(_Ty t, _Up *u) {
    { t.operator$cast(u) } -> H_STD_NAMESPACE::traits::same_as<_Up>; // castable to the requested type
};

template <typename _Ty>
concept ConvertibleToString = HasToString<_Ty> || SupportsOStream<_Ty> || SafelyCastable<_Ty, string>;

}  // namespace concepts
H_STD_NAMESPACE_END
H_NAMESPACE_END
#endif