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

#ifndef __$LIBHELIX_MMEMORY__
#define __$LIBHELIX_MMEMORY__

#include "config.h"
#include "libcxx.h"
#include "meta.h"

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace memory {
using LIBCXX_NAMESPACE::move;

template <typename T>
T &&forward(meta::remove_reference_t<T> &t) noexcept {
    return static_cast<T &&>(t);
}

template <typename T>
T &&forward(meta::remove_reference_t<T> &&t) noexcept {
    return static_cast<T &&>(t);
}

template <typename T>
    requires(!meta::is_add_lvalue_reference<T>)
constexpr meta::reference_to_pointer_t<T> as_pointer(T &&ref) noexcept {
    return &ref;
}

template <typename T>
constexpr T &&as_reference(T *ptr) noexcept {
    return *ptr;
}

template <class T, class U = T>
inline constexpr T exchange(T &obj, U &&new_value) noexcept
  requires(meta::is_nothrow_move_constructible<T> && meta::is_nothrow_assignable<T &, U>) {
    T old_value = move(obj);
    obj         = forward<U>(new_value);
    return old_value;
}
}

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // __$LIBHELIX_MMEMORY__