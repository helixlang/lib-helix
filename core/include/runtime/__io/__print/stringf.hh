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
#include <include/types/question/question_impl.hh>
#include <include/runtime/__panic/panic_config.hh>
#include <include/runtime/__error/runtime_error.hh>

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
inline string stringf(string s, Ty &&...t) {
    const string args[sizeof...(t)] = {to_string(H_STD_NAMESPACE::Memory::forward<Ty>(t))...};

    string result = Memory::move(s);
    usize  pos    = 0;

    usize arg_index = 0;
    while (arg_index < sizeof...(t)) {
        auto placeholder = string::slice(L"\\{\\}", 4);  // Wide string placeholder
        std::Questionable<usize> found_pos = result.lfind(placeholder, pos);

        if (found_pos == null) {
            _HX_MC_Q7_INTERNAL_CRASH_PANIC_M(
                Error::RuntimeError(L"error: [f-string rt]: too many arguments for format string"));
        }

        usize replace_pos = *found_pos;
        result.replace(replace_pos, 4, args[arg_index]);

        pos = replace_pos + args[arg_index].size();
        ++arg_index;
    }

    auto                     remaining_placeholder = string::slice(L"\\{\\}", 4);
    std::Questionable<usize> remaining_pos         = result.lfind(remaining_placeholder, pos);
    if (remaining_pos != null) {
        _HX_MC_Q7_INTERNAL_CRASH_PANIC_M(
            Error::RuntimeError(L"error: [f-string rt]: too few arguments for format string"));
    }

    return result;
}

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M7STRINGF
