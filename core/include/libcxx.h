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

#ifndef __$LIBHELIX_LIBCXX__
#define __$LIBHELIX_LIBCXX__

#include <array>
#include <cassert>
#include <cstddef>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include <coroutine>
#include <iterator>
#include <optional>
#include <sstream>
#include <type_traits>
#include <typeinfo>
#include <any>

#include "meta.h"
#include "config.h"

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace memory {
template <typename T>
T &&forward(meta::remove_reference_t<T> &t) noexcept;

template <typename T>
T &&forward(meta::remove_reference_t<T> &&t) noexcept;
}

template <typename _Tp, typename... _Ty>
constexpr _Tp *_H_RESERVED$new(_Ty &&...t) { // NOLINT
    return new _Tp(H_STD_NAMESPACE::memory::forward<_Ty>(t)...); // NOLINT
}
H_NAMESPACE_END
H_STD_NAMESPACE_END

#endif