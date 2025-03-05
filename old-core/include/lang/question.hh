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
///                                                                                              ///
///  this is still a work in progress, the $question class is a work in progress and is not yet  ///
///  fully implemented, the $question class is a class that is similar to the
///  H_STD_NAMESPACE::optional but  /// also adds H_STD_NAMESPACE::expected like functionality, the
///  $question class is a class that can hold    /// one of 3 states: value, an error, or be null.
///  ///
///                                                                                              ///
///  the $question class is a class that is to be used inside the core since its required by the ///
///  compiler to allow for `?` type syntax.                                                      ///
///                                                                                              ///
///----------------------------------------------------------------------------------------------///

#ifndef __$LIBHELIX_QUESTION__
#define __$LIBHELIX_QUESTION__

#include "../config.h"
#include "../lang/panic.hh"
#include "../memory.h"
#include "include/meta.h"
#include "../primitives.h"
#include "include/types/errors.hh"

H_NAMESPACE_BEGIN



H_STD_NAMESPACE_BEGIN



H_STD_NAMESPACE_END
H_NAMESPACE_END
#endif