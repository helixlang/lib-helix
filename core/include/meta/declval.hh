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

#ifndef _$_HX_CORE_M7DECLVAL
#define _$_HX_CORE_M7DECLVAL

#include <include/config/config.h>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Meta {  /// portable bindings for declval
template <class T>
T &&__declval(int);  // NOLINT
template <class T>
T __declval(long);  // NOLINT

template <class T>
decltype(__declval<T>(0)) declval() noexcept {
    static_assert(
        !__is_same(T, T),
        "H_STD_NAMESPACE::declval can only be used in an unevaluated context. "
        "It's likely that your current usage is trying to extract a value from the function.");
}
}  // namespace Meta

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M7DECLVAL
