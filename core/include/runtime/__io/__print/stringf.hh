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
inline string stringf(string fmt, Ty &&...args) {
    const wchar_t placeholder_chars[] = {L'\\', L'{', L'\\', L'}'};
    const string::slice placeholder = string::slice(placeholder_chars, 4);

    const string arg_strs[sizeof...(Ty)] = {to_string(H_STD_NAMESPACE::Memory::forward<Ty>(args))...};

    string result = Memory::move(fmt);
    usize pos = 0;

    usize arg_index = 0;
    while (arg_index < sizeof...(Ty)) {
        auto found = result.lfind(placeholder, pos);
        if (found == null) {
            _HX_MC_Q7_INTERNAL_CRASH_PANIC_M(Error::RuntimeError(L"[f-string rt]: too many arguments for format string"));
        }


        usize replace_pos = *found;
        const string& arg_str = arg_strs[arg_index];

        result.replace(replace_pos, placeholder.size(), arg_str);

        pos = replace_pos + arg_str.size();
        ++arg_index;
    }

    // Ensure no placeholders are left
    auto leftover = result.lfind(placeholder, pos);
    if (leftover != null) {
        _HX_MC_Q7_INTERNAL_CRASH_PANIC_M(Error::RuntimeError(L"[f-string rt]: too few arguments for format string"));
    }

    return result;
}

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M7STRINGF
