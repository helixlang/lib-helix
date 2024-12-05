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

#ifndef __$LIBHELIX_DTYPES__
#define __$LIBHELIX_DTYPES__

#include <array>
#include <cstddef>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include "config.h"
#include "dtypes.h"
#include "refs.h"
#include "types.h"

H_NAMESPACE_BEGIN

using byte   = LIBCXX_NAMESPACE::byte;
using string = LIBCXX_NAMESPACE::string;

H_STD_NAMESPACE_BEGIN

template <typename _Ty>
constexpr string to_string(_Ty &&t);  // forward declaration

H_STD_NAMESPACE_END

template <typename _Ty, usize _Size>
using array = LIBCXX_NAMESPACE::array<_Ty, _Size>;

template <typename... _Tv>
class tuple : public LIBCXX_NAMESPACE::tuple<_Tv...> {
  public:
    using LIBCXX_NAMESPACE::tuple<_Tv...>::tuple;

    template <typename... Ts>
    tuple(Ts &&...args)
        : LIBCXX_NAMESPACE::tuple<_Tv...>(std::forward<Ts>(args)...) {}

    inline auto $cast(string * /* unused */) -> string {  // cast to string op to allow for printing
        string result = "(";

        for (usize i = 0; i < sizeof...(_Tv); i++) {
            result += std::to_string(LIBCXX_NAMESPACE::get<i>(*this));

            if (i < sizeof...(_Tv) - 1) {
                result += ", ";
            }
        }

        return result + ")";
    }

    explicit operator string() { return $cast(static_cast<string *>(nullptr)); }
};

template <typename _Ty>
class list : public LIBCXX_NAMESPACE::vector<_Ty> {
  public:
    using LIBCXX_NAMESPACE::vector<_Ty>::vector;

    list(LIBCXX_NAMESPACE::initializer_list<_Ty> init)
        : LIBCXX_NAMESPACE::vector<_Ty>(init) {}

    inline auto $cast(string * /* unused */) -> string {  // cast to string op to allow for printing
        string result = "[";

        for (usize i = 0; i < this->size(); i++) {
            result += std::to_string(this->at(i));

            if (i < this->size() - 1) {
                result += ", ";
            }
        }

        return result + "]";
    }

    explicit operator string() { return $cast(static_cast<string *>(nullptr)); }
};

template <typename _Ty>
class set : public LIBCXX_NAMESPACE::set<_Ty> {
  public:
    using LIBCXX_NAMESPACE::set<_Ty>::set;

    set(LIBCXX_NAMESPACE::initializer_list<_Ty> init)
        : LIBCXX_NAMESPACE::set<_Ty>(init) {}

    inline auto $cast(string * /* unused */) -> string {  // cast to string op to allow for printing
        string result = "{";

        for (auto it = this->begin(); it != this->end(); it++) {
            result += std::to_string(*it);

            if (LIBCXX_NAMESPACE::next(it) != this->end()) {
                result += ", ";
            }
        }

        return result + "}";
    }

    explicit operator string() { return $cast(static_cast<string *>(nullptr)); }
};

template <typename _Kt, typename _Vt>
class map : public LIBCXX_NAMESPACE::map<_Kt, _Vt> {
  public:
    using LIBCXX_NAMESPACE::map<_Kt, _Vt>::map;

    map(LIBCXX_NAMESPACE::initializer_list<LIBCXX_NAMESPACE::pair<const _Kt, _Vt>> init)
        : LIBCXX_NAMESPACE::map<_Kt, _Vt>(init) {}

    inline auto $cast(string * /* unused */) -> string {  // cast to string op to allow for printing
        string result = "{";

        for (auto it = this->begin(); it != this->end(); it++) {
            result += std::to_string(it->first) + ": " + std::to_string(it->second);

            if (LIBCXX_NAMESPACE::next(it) != this->end()) {
                result += ", ";
            }
        }

        return result + "}";
    }

    explicit operator string() { return $cast(static_cast<string *>(nullptr)); }
};

using $int = int;

H_NAMESPACE_END
#endif
