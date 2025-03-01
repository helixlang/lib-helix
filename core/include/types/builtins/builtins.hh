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

#ifndef _$_HX_CORE_M8BUILTINS
#define _$_HX_CORE_M8BUILTINS

#include "256_bit.hh"
#include "f128_bit.hh"
#include "literals.hh"
#include "primitives.hh"
#include "size_t.hh"
#include "variant.hh"

// ensure that the sizes are correct and match the platform
static_assert(sizeof(helix::usize) == sizeof(void *), "usize must match the size of a pointer.");
static_assert(sizeof(helix::isize) == sizeof(void *), "isize must match the size of a pointer.");

#endif  // _$_HX_CORE_M8BUILTINS
