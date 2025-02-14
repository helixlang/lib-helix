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

#ifndef __$LIBHELIX_FORWARD__
#define __$LIBHELIX_FORWARD__

#include <string>

#include "../config.h"
#include "../meta.h"
#include "../primitives.h"

H_NAMESPACE_BEGIN
template <typename T>
class vec;

template <typename T>
class $question;

H_STD_NAMESPACE_BEGIN

template <typename T>
using Questionable = $question<T>;

namespace Interface {
template <typename self>
concept CharaterType = requires(self inst, self other, char c, int i) {
    { ++inst } -> std::Meta::convertible_to<self>;
    { inst++ } -> std::Meta::convertible_to<self>;
    { --inst } -> std::Meta::convertible_to<self>;
    { inst-- } -> std::Meta::convertible_to<self>;

    { inst == other } -> std::Meta::convertible_to<bool>;
    { inst != other } -> std::Meta::convertible_to<bool>;
    { inst < other } -> std::Meta::convertible_to<bool>;
    { inst > other } -> std::Meta::convertible_to<bool>;
    { inst <= other } -> std::Meta::convertible_to<bool>;
    { inst >= other } -> std::Meta::convertible_to<bool>;

    { inst = other } -> std::Meta::same_as<self &>;

    { static_cast<char>(inst) } -> std::Meta::same_as<char>;
    { static_cast<int>(inst) } -> std::Meta::same_as<int>;

    { inst + i } -> std::Meta::convertible_to<self>;
    { inst - i } -> std::Meta::convertible_to<self>;
    { i + inst } -> std::Meta::convertible_to<self>;

    // eval if needed for char
    { inst - other } -> std::Meta::convertible_to<int>;

    { inst += i } -> std::Meta::same_as<self &>;
    { inst -= i } -> std::Meta::same_as<self &>;
};
}  // namespace Interface

namespace String {
template <typename T>
class basic;
}

H_STD_NAMESPACE_END

using string = LIBCXX_NAMESPACE::string;

H_STD_NAMESPACE_BEGIN

template <typename T>
constexpr string to_string(T &&t);  // forward declaration

H_STD_NAMESPACE_END
H_NAMESPACE_END
#endif