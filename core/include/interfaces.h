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

#ifndef __$LIBHELIX_INTERFACES__
#define __$LIBHELIX_INTERFACES__

#include "config.h"
#include "types.h"
#include "meta.h"

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN
namespace interfaces {
template <typename T>
concept SupportsOStream = requires(LIBCXX_NAMESPACE::ostream &os, T a) {
    { os << a } -> H_STD_NAMESPACE::meta::convertible_to<LIBCXX_NAMESPACE::ostream &>;
};

template <typename T, typename U>
concept SupportsPointerCast = requires(T from) {
    dynamic_cast<U>(from);  // Dynamic cast requirement
};

template <typename T, typename U>
concept Castable = requires(T t, U *u) {
    { t.operator$cast(u) } -> H_STD_NAMESPACE::meta::same_as<U>; // cast to the requested type
} || requires(T t, U *u) {
    { t.operator U() } -> H_STD_NAMESPACE::meta::same_as<U>; // call the implicit cast
};

template <typename T>
concept ConvertibleToString = SupportsOStream<T> || Castable<T, string>;
}  // namespace interfaces
H_STD_NAMESPACE_END
H_NAMESPACE_END
#endif