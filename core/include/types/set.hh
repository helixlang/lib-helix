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

#ifndef __$LIBHELIX_SET__
#define __$LIBHELIX_SET__

#include "../config.h"
#include "../libcxx.h"
#include "../memory.h"
#include "forward.hh"

H_NAMESPACE_BEGIN

template <typename T>
class set : public LIBCXX_NAMESPACE::set<T> {
  public:
    using LIBCXX_NAMESPACE::set<T>::set;

    set(LIBCXX_NAMESPACE::initializer_list<T> init)
        : LIBCXX_NAMESPACE::set<T>(init) {}

    inline auto operator$cast(string * /* unused */) -> string {  // cast to string op to allow for printing
        string result = "{";

        for (auto it = this->begin(); it != this->end(); it++) {
            result += H_STD_NAMESPACE::to_string(*it);

            if (LIBCXX_NAMESPACE::next(it) != this->end()) {
                result += ", ";
            }
        }

        return result + "}";
    }

    explicit operator string() { return operator$cast(static_cast<string *>(nullptr)); }
};

H_NAMESPACE_END
#endif