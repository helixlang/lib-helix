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

#ifndef _$_HX_CORE_M15TYPE_PROPERTIES
#define _$_HX_CORE_M15TYPE_PROPERTIES

#include "../config/config.h"
#include "integral_constant.hh"
#include "const_traits.hh"
#include "reference_traits.hh"

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Meta {
    namespace _types {
        template <typename T>
        struct is_nothrow_move_constructible : Meta::_types::integral_constant<bool, __is_nothrow_constructible(T, typename _types::add_rvalue_reference<T>::type)> {};

        template <typename T, typename Arg>
        struct is_nothrow_assignable : Meta::_types::integral_constant<bool, __is_nothrow_assignable(T, Arg)> {};

        template <class T>
        struct is_copy_constructible : public Meta::_types::integral_constant<bool,  __is_constructible(T, typename add_lvalue_reference<typename add_const<T>::type>::type)> {};
    }  // namespace _types

    template <typename T> // FIXME( add typename if its not working on msvc )
    using can_move_noexcept = _types::is_nothrow_move_constructible<T>::value;

    template <typename T, typename Arg>
    using can_assign_noexcept = _types::is_nothrow_assignable<T, Arg>::value;

    template <typename T>
    using can_copy_construct = _types::is_copy_constructible<T>::value;
}  // namespace Meta

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M15TYPE_PROPERTIES