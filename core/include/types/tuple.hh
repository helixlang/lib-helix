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

#ifndef __$LIBHELIX_TUPLE__
#define __$LIBHELIX_TUPLE__

#include "../config.h"
#include "../libcxx.h"
#include "../primitives.h"
#include "forward.hh"

H_NAMESPACE_BEGIN

template <typename... T>
class tuple : public LIBCXX_NAMESPACE::tuple<T...> {
  public:
    using LIBCXX_NAMESPACE::tuple<T...>::tuple;

    template <typename... Ts>
    tuple(Ts &&...args) // NOLINT
        : LIBCXX_NAMESPACE::tuple<T...>(H_STD_NAMESPACE::Memory::forward<Ts>(args)...) {}

    inline auto operator$cast(string * /* unused */) -> string {  // cast to string op to allow for printing
        string result = "(";

        for (usize i = 0; i < sizeof...(T); i++) {
            result += H_STD_NAMESPACE::to_string(LIBCXX_NAMESPACE::get<i>(*this));

            if (i < sizeof...(T) - 1) {
                result += ", ";
            }
        }

        return result + ")";
    }

    explicit operator string() { return operator$cast(static_cast<string *>(nullptr)); }
};

H_NAMESPACE_END
#endif