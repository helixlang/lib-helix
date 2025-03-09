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

#ifndef _$_HX_CORE_M5PRINT
#define _$_HX_CORE_M5PRINT

#include <include/config/config.h>

#include <include/meta/meta.hh>
#include <include/runtime/__io/__print/endl.hh>
#include <include/runtime/__memory/memory.hh>
#include <include/types/string/string.hh>

H_NAMESPACE_BEGIN

template <typename... Args>
inline constexpr void print(Args &&...t) {
    if constexpr (sizeof...(t) == 0) {
        printf("\n");
        return;
    }

    ((printf("%s", std::to_string(std::Memory::forward<Args>(t)).c_str())), ...);

    if constexpr (sizeof...(t) > 0) {
        if constexpr (!std::Meta::same_as<
                          std::Meta::const_volatile_removed<std::Meta::reference_removed<
                              decltype(LIBCXX_NAMESPACE::get<sizeof...(t) - 1>(
                                  tuple<Args...>(t...)))>>,
                          std::endl>) {
            printf("\n");
        }
    }
}

H_NAMESPACE_END

#endif  // _$_HX_CORE_M5PRINT
