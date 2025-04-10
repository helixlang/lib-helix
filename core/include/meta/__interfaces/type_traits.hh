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

#ifndef _$_HX_CORE_M11TYPE_TRAITS
#define _$_HX_CORE_M11TYPE_TRAITS

#include <include/config/config.h>

#include <include/meta/traits.hh>
#include <include/meta/type_properties.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Interface {
template <typename T>
concept ClassType = Meta::is_class<T>;

template <class T>
concept ConstType = std::Meta::is_const<T>;

template <typename T>
concept ReferenceableType = std::Meta::is_referenceable<T>;

template <class T>
concept RValueReference = std::Meta::is_rval_reference<T>;

template <class T>
concept LValueReference = std::Meta::is_rval_reference<T>;

template <class T>
concept ReferenceType = std::Meta::is_reference<T>;

template <typename T>
concept MoveConstructible = std::Meta::can_move_noexcept<T>;

template <typename T, typename Arg>
concept NothrowAssignable = std::Meta::can_assign_noexcept<T, Arg>;

template <class T>
concept CopyConstructible = std::Meta::can_copy_construct<T>;
}  // namespace Interface

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M11TYPE_TRAITS
