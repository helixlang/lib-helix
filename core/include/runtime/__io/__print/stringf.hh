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

#ifndef _$_HX_CORE_M7STRINGF
#define _$_HX_CORE_M7STRINGF

#include <include/config/config.h>

#include <include/types/string/string.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

/// \include belongs to the helix standard library.
/// \brief format a string with arguments
///
/// TODO: = is not yet suppoted
///
/// the following calls can happen in helix and becomes the following c++:
///
/// f"hi: {var}"   -> stringf("hi: \{\}", var)
/// f"hi: {var1=}" -> stringf("hi: var1=\{\}", var1)
///
/// f"hi: {(some_expr() + 12)=}" -> stringf("hi: (some_expr() + 12)=\{\}", some_expr())
/// f"hi: {some_expr() + 12}"    -> stringf("hi: \{\}", some_expr() + 12)
///
template <typename... Ty>
constexpr string stringf(string s, Ty &&...t) {
    const array<string, sizeof...(t)> EAS = {to_string(H_STD_NAMESPACE::Memory::forward<Ty>(t))...};

    usize pos = 0;

#ifdef __GNUG__
#pragma unroll
#endif

    for (auto &&arg : EAS) {
        pos = s.lfind(L"\\{\\}", pos);

        if (pos == string::npos) [[unlikely]] {
            throw LIBCXX_NAMESPACE::runtime_error(
                "error: [f-stirng engine]: format argument count mismatch, this should not "
                "happen, please open a issue on github");
        }

        s.replace(pos, 4, arg);
        pos += arg.size();
    }

    return s;
}

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M7STRINGF
