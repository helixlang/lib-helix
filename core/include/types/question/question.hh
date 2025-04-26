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

#ifndef _$_HX_CORE_M8QUESTION
#define _$_HX_CORE_M8QUESTION

#include <include/config/config.hh>

#include <include/types/question/question_fwd.hh>
#include <include/types/question/question_impl.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

/// \typedef Questionable
///
/// A type alias for the `$question` class, providing a more concise and readable syntax for
/// working with quantum types (`?`). This alias also allows developers to use the `Questionable`
/// type in either C++ or Helix code. exposing the `Questionable` type for interoperability.
template <typename T>
using Questionable = $question<T>;

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M8QUESTION
