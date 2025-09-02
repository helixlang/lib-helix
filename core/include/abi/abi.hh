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

#ifndef _$_HX_CORE_M3ABI
#define _$_HX_CORE_M3ABI

#include <include/c++/libc++.hh>
#include <include/config/config.hh>
#include <include/runtime/__io/__print/print.hh>
#include <include/runtime/__io/__print/stringf.hh>
#include <include/types/types.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace ABI {

enum class ObjectType : u8 { Module, Class, Struct, Function, Operator, Reserved, Internal, None };

inline string mangle(const string &input, ObjectType type) {
    if (input.empty()) {
        throw libcxx::invalid_argument("Input string cannot be empty");
    }

    string prefix;
    switch (type) {
        case ObjectType::Module:
            prefix = L"_$M_";
            break;
        case ObjectType::Class:
            prefix = L"_$C_";
            break;
        case ObjectType::Struct:
            prefix = L"_$S_";
            break;
        case ObjectType::Function:
            prefix = L"_$F_";
            break;
        case ObjectType::Operator:
            prefix = L"_$O_";
            break;
        case ObjectType::Reserved:
            prefix = L"_$R_";
            break;
        case ObjectType::Internal:
            prefix = L"_$I_";
            break;
        default:
            break;
    }

    string output = prefix;

    for (wchar_t ch : input) {
        if (libcxx::iswalnum(ch) || ch == L'_') {
            output += ch;
        } else {
            // Encode as $XXXX (4 hex digits)
            wchar_t hex[5];
            swprintf(hex, 5, L"%04X", static_cast<unsigned int>(ch));
            output += L'$';
            output += hex;
        }
    }

    // Append length marker
    output += L"$L";
    output += input + L"_E$";

    return output;
}

inline ObjectType is_mangled(const string &input) {
    if (input.length() < 4 || input[0] != L'_' || input[1] != L'$') {
        return ObjectType::None;
    }

    string prefix = input.subslice(0, 3);
    if (prefix == L"_$M_")
        return ObjectType::Module;
    if (prefix == L"_$C_")
        return ObjectType::Class;
    if (prefix == L"_$S_")
        return ObjectType::Struct;
    if (prefix == L"_$F_")
        return ObjectType::Function;
    if (prefix == L"_$O_")
        return ObjectType::Operator;
    if (prefix == L"_$R_")
        return ObjectType::Reserved;
    if (prefix == L"_$I_")
        return ObjectType::Internal;

    return ObjectType::None;
}

inline bool is_hex_digit(wchar_t c) {
    return (c >= L'0' && c <= L'9') || (c >= L'a' && c <= L'f') || (c >= L'A' && c <= L'F');
}

inline string basename_no_ext(const string &path) {
    std::Questionable<usize> slash  = path.rfind(L"/");
    std::Questionable<usize> bslash = path.rfind(L"\\");
    std::Questionable<usize> sep    = slash;

    if (bslash != null && (sep == null || (*bslash) > (*sep))) {
        sep = *bslash;
    }

    usize                    start = (sep == null) ? 0 : (*sep) + 1;  // PROBLEM
    std::Questionable<usize> dot   = path.rfind(L".");

    usize end = (dot != null && (*dot) > start) ? (*dot) : static_cast<usize>(path.length());

    return path.subslice(start, end - start);
}

// --- FIXED: supports $XX and $XXXX
inline string demangle(const string &input, ObjectType type) {
    string expected_prefix;
    switch (type) {
        case ObjectType::Module:
            expected_prefix = L"_$M_";
            break;
        case ObjectType::Class:
            expected_prefix = L"_$C_";
            break;
        case ObjectType::Struct:
            expected_prefix = L"_$S_";
            break;
        case ObjectType::Function:
            expected_prefix = L"_$F_";
            break;
        case ObjectType::Operator:
            expected_prefix = L"_$O_";
            break;
        case ObjectType::Reserved:
            expected_prefix = L"_$R_";
            break;
        case ObjectType::Internal:
            expected_prefix = L"_$I_";
            break;
        default:
            break;
    }

    if (!input.starts_with(expected_prefix)) {
        throw libcxx::invalid_argument("Invalid mangled name or type mismatch");
    }

    usize len_pos = input.raw_string().rfind(L"$L");
    if (len_pos == string::npos) {
        throw libcxx::invalid_argument("Invalid mangled name: missing length");
    }
    usize end_pos = input.raw_string().rfind(L"_E$");
    if (end_pos == string::npos || end_pos <= len_pos) {
        throw libcxx::invalid_argument("Invalid mangled name: missing end marker");
    }

    string len_str = input.subslice(len_pos + 2, end_pos - len_pos - 2);
    if (len_str.empty()) {
        throw libcxx::invalid_argument("Invalid length in mangled name");
    }

    usize expected_len = 0;
    try {
        expected_len = libcxx::stoul(len_str);
    } catch (...) { throw libcxx::invalid_argument("Invalid length in mangled name"); }

    string encoded = input.subslice(expected_prefix.length(), len_pos - expected_prefix.length());
    string output;

    for (usize i = 0; i < encoded.length();) {
        if (encoded[i] == L'$') {
            usize j     = i + 1;
            usize count = 0;
            while (j < encoded.length() && count < 4 && is_hex_digit(encoded[j])) {
                ++j;
                ++count;
            }

            if (count == 4 || count == 2) {
                string hx = encoded.subslice(i + 1, count);
                try {
                    unsigned long code = libcxx::stoul(hx, nullptr, 16);
                    output += static_cast<wchar_t>(code);
                    i = j;

                    continue;
                } catch (...) { ; }
            }
        }

        output += encoded[i];
        ++i;
    }

    if (output.length() != expected_len) {
        throw libcxx::invalid_argument("Demangled length mismatch");
    }

    return output;
}

// --- FIXED: replaces mangled segments; for Modules returns basename w/o extension
inline string demangle_partial(const string &input) {
    string output;
    for (usize i = 0; i < input.length(); ++i) {
        if (i + 3 < input.length() && input[i] == L'_' && input[i + 1] == L'$') {
            ObjectType ty = ObjectType::None;
            switch (input[i + 2]) {
                case L'M':
                    ty = ObjectType::Module;
                    break;
                case L'C':
                    ty = ObjectType::Class;
                    break;
                case L'S':
                    ty = ObjectType::Struct;
                    break;
                case L'F':
                    ty = ObjectType::Function;
                    break;
                case L'O':
                    ty = ObjectType::Operator;
                    break;
                case L'R':
                    ty = ObjectType::Reserved;
                    break;
                case L'I':
                    ty = ObjectType::Internal;
                    break;
                default:
                    output += input[i];
                    continue;
            }

            usize end_pos = input.lfind(L"_E$", i);  // your string type uses lfind
            if (end_pos == string::npos) {
                output += input[i];
                continue;
            }

            string mangled = input.subslice(i, end_pos - i + 3);  // include "_E$"
            string dem     = demangle(mangled, ty);

            // For Modules, keep only basename (strip dirs + .hlx)
            if (ty == ObjectType::Module) {
                dem = basename_no_ext(dem);
            }

            output += dem;
            i = end_pos + 2;  // will ++ at loop end → land after '$'
            continue;
        }

        output += input[i];
    }
    return output;
}

// --- Utility: remove leading "helix::" (and the "::helix::" variant)
inline string strip_helix_prefix(const string &input) {
    const libcxx::wstring raw = input.raw_string();
    const libcxx::wstring p1  = L"helix::";
    const libcxx::wstring p2  = L"::helix::";

    auto pos2 = raw.find(p2);
    auto pos1 = raw.find(p1);

    if (pos2 == libcxx::wstring::npos && pos1 == libcxx::wstring::npos) {
        return input;
    }

    usize chosen_pos;
    usize chosen_len;
    if (pos2 != libcxx::wstring::npos && (pos1 == libcxx::wstring::npos || pos2 < pos1)) {
        chosen_pos = pos2;
        chosen_len = p2.length();
    } else {
        chosen_pos = pos1;
        chosen_len = p1.length();
    }

    auto start = static_cast<usize>(chosen_pos);
    auto len   = static_cast<usize>(chosen_len);

    usize total = input.length();

    string left  = input.subslice(0, start);
    string right = input.subslice(start + len, total - (start + len));
    return left + right;
}

}  // namespace ABI

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif
