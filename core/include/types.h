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

#include "config.h"
#include "libcxx.h"
#include "memory.h"
#include "primitives.h"

H_NAMESPACE_BEGIN

using string = LIBCXX_NAMESPACE::string;

H_STD_NAMESPACE_BEGIN

template <typename T>
constexpr string to_string(T &&t);  // forward declaration

H_STD_NAMESPACE_END

template <typename T, usize N>
using array = LIBCXX_NAMESPACE::array<T, N>;

template <typename... T>
class tuple : public LIBCXX_NAMESPACE::tuple<T...> {
  public:
    using LIBCXX_NAMESPACE::tuple<T...>::tuple;

    template <typename... Ts>
    tuple(Ts &&...args) // NOLINT
        : LIBCXX_NAMESPACE::tuple<T...>(H_STD_NAMESPACE::memory::forward<Ts>(args)...) {}

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

template <typename T>
class list : public LIBCXX_NAMESPACE::vector<T> {
  public:
    using LIBCXX_NAMESPACE::vector<T>::vector;

    list(LIBCXX_NAMESPACE::initializer_list<T> init)
        : LIBCXX_NAMESPACE::vector<T>(init) {}

    inline auto operator$cast(string * /* unused */) -> string {  // cast to string op to allow for printing
        string result = "[";

        for (usize i = 0; i < this->size(); i++) {
            result += H_STD_NAMESPACE::to_string(this->at(i));

            if (i < this->size() - 1) {
                result += ", ";
            }
        }

        return result + "]";
    }

    explicit operator string() { return operator$cast(static_cast<string *>(nullptr)); }
};

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

template <typename K, typename V>
class map : public LIBCXX_NAMESPACE::map<K, V> {
  public:
    using LIBCXX_NAMESPACE::map<K, V>::map;

    map(LIBCXX_NAMESPACE::initializer_list<LIBCXX_NAMESPACE::pair<const K, V>> init)
        : LIBCXX_NAMESPACE::map<K, V>(init) {}

    inline auto operator$cast(string * /* unused */) -> string {  // cast to string op to allow for printing
        string result = "{";

        for (auto it = this->begin(); it != this->end(); it++) {
            result += H_STD_NAMESPACE::to_string(it->first) + ": " + H_STD_NAMESPACE::to_string(it->second);

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
