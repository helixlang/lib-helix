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

#ifndef _$_HX_CORE_M6STRING
#define _$_HX_CORE_M6STRING

#include <include/meta/meta.hh>
#include <include/runtime/__memory/memory.hh>
#include <include/types/string/basic.hh>
#include <include/types/string/c-string.hh>
#include <include/types/string/char_traits.hh>
#include <include/types/string/slice.hh>

#include "include/config/config.h"

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

constexpr string to_string(libcxx::string str) { return {str.data(), str.size()}; }

/// \include belongs to the helix standard library.
/// \brief convert any type to a string
///
/// This function will try to convert the argument to a string using the following methods:
/// - if the argument has a to_string method, it will use that
/// - if the argument has a ostream operator, it will use that
/// - if the argument is an arithmetic type, it will use std::to_string
/// - if all else fails, it will convert the address of the argument to a string
template <typename Ty>
    requires(!std::Meta::same_as<Ty, char *>)
constexpr string to_string(Ty &&t) {
    if constexpr (std::Interface::SupportsOStream<Ty>) {
        LIBCXX_NAMESPACE::stringstream ss;
        ss << t;
        return to_string(ss.str());
    } else if constexpr (std::Interface::Castable<Ty, string>) {
        return t.operator$cast(static_cast<string *>(nullptr));
    } else if constexpr (std::Meta::same_as<Ty, bool>) {
        return t ? "true" : "false";
    } else if constexpr (LIBCXX_NAMESPACE::is_arithmetic_v<Ty>) {
        return LIBCXX_NAMESPACE::to_string(t);
    } else {
        LIBCXX_NAMESPACE::stringstream ss;

#ifdef _MSC_VER
        ss << "[" << typeid(t).name() << " at 0x" << LIBCXX_NAMESPACE::hex << &t << "]";
#else
        int   st;
        char *rn = __cxxabiv1::__cxa_demangle(typeid(t).name(), 0, 0, &st);
        ss << "[" << rn << " at 0x" << LIBCXX_NAMESPACE::hex << &t << "]";
        free(rn);
#endif

        return to_string(ss.str());
    }
}

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M6STRING
