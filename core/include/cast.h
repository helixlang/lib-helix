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

#ifndef __$LIBHELIX_CAST__
#define __$LIBHELIX_CAST__

#include "concepts.h"
#include "config.h"
#include "traits.h"

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

// ----- as_cast ----- //
template <typename _Ty, typename _Up>
constexpr _Ty as_cast(_Up &value) noexcept {
    if constexpr (LIBCXX_NAMESPACE::is_const_v<LIBCXX_NAMESPACE::remove_reference_t<_Up>> &&
                  LIBCXX_NAMESPACE::is_same_v<LIBCXX_NAMESPACE::remove_const_t<_Up>, _Ty>) {
        return const_cast<_Ty>(value);
    } else if constexpr (LIBCXX_NAMESPACE::is_pointer_v<_Ty>) {
        if constexpr (H_STD_NAMESPACE::concepts::SupportsPointerCast<_Up, _Ty>) {
            return dynamic_cast<_Ty>(value);
        } else {
            return static_cast<_Ty>(value);
        }
    } else if constexpr (LIBCXX_NAMESPACE::is_reference_v<_Ty>) {
        return static_cast<_Ty>(value);
    } else if constexpr (H_STD_NAMESPACE::concepts::SafelyCastable<_Up, _Ty>) {
        return value.$cast(static_cast<_Ty *>(nullptr));
    } else {
        return static_cast<_Ty>(value);
    }
}

template <typename _Ty, typename _Up>
constexpr _Ty as_cast(const _Up &value) noexcept {
    return as_cast<_Ty>(const_cast<_Up &>(value));
}

// ----- as_const ----- //
template <typename _Ty, typename _Up>
constexpr const _Ty &as_const(_Up &value) noexcept {
    return const_cast<const _Ty &>(value);
}

template <typename _Ty, typename _Up>
constexpr const _Ty &as_const(const _Up &value) noexcept {
    return static_cast<const _Ty &>(value);
}

// ----- as_unsafe ----- //
template <typename _Ty, typename _Up>
constexpr _Ty as_unsafe(_Up value) noexcept {
    return reinterpret_cast<_Ty>(value);
}

template <typename _Ty, typename _Up>
constexpr const _Ty as_unsafe(const _Up value) noexcept
    requires(H_STD_NAMESPACE::traits::_const::utils::is_const_v<_Up> ||
             H_STD_NAMESPACE::traits::_const::utils::is_const_v<_Ty>)
{
    return reinterpret_cast<const _Ty>(value);
}

H_STD_NAMESPACE_END
H_NAMESPACE_END
#endif