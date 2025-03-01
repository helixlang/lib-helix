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

#ifndef _$_HX_CORE_M20REFERENCE_TO_POINTER
#define _$_HX_CORE_M20REFERENCE_TO_POINTER

#include "../config/config.h"

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Meta {
    
}

H_STD_NAMESPACE_END
H_NAMESPACE_END

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Meta {
namespace _types {

    template <typename T>
    struct reference_to_pointer {
        using type = T;
    };

    template <typename T>
    struct reference_to_pointer<T &> {
        using type = T *;
    };

    template <typename T>
    struct reference_to_pointer<T &&> {
        using type = T *;
    };
}

template <typename T>
using ref_as_ptr = typename _types::reference_to_pointer<T>::type;
}

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M20REFERENCE_TO_POINTER
