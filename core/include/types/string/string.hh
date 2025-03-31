///--- The Helix Project ------------------------------------------------------------------------///
///                                                                                              ///
///   part of the helix project, under the attribution 4.0 international license (cc by 4.0).    ///
///   you are allowed to use, modify, redistribute, and create derivative works, even for        ///
///   commercial purposes, provided that you give appropriate credit, and indicate if changes    ///
///   were made.                                                                                 ///
///                                                                                              ///
///   for more information on the license terms and requirements, please visit:                  ///
///     https://creativecommons.org/licenses/by/4.0/                                             ///
///                                                                                              ///
///   spdx-license-identifier: cc-by-4.0                                                         ///
///   copyright (c) 2024 the helix project (cc by 4.0)                                           ///
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

inline string to_string(libcxx::string str) { return {str.data(), str.size()}; }

/// \include belongs to the helix standard library.
/// \brief convert any type to a string
///
/// this function will try to convert the argument to a string using the following methods:
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

// compile-time check for wchar_t size
static_assert(sizeof(wchar_t) >= sizeof(char), "wchar_t must be at least as large as char");

inline char char_to_cchar(wchar_t wc) {
    char temp[MB_CUR_MAX]; // stack-allocated, typically 4-6 bytes
    
    int len = wctomb(temp, wc);
    
    if (len <= 0) {
        throw libcxx::range_error("Invalid wchar_t conversion");
    }
    if (len > 1) {
        throw libcxx::range_error("wchar_t converts to multibyte, not single char");
    }
    
    return temp[0];
}

inline string sstring_to_string(const sstring& cstr) {
    // Get the raw char* data and its size
    const char* raw_data = cstr.raw(); // c_str equivalent
    size_t cstr_size = cstr.size();

    if (cstr_size == 0) {
        return {}; // Empty string
    }

    // Worst-case size estimation: each byte could be part of a multibyte sequence,
    // but we assume at most 1 wchar_t per MB_CUR_MAX bytes as a rough upper bound
    size_t max_size = cstr_size; // Conservative: 1 wchar_t per byte in worst case
    string result;
    result.resize(max_size); // Pre-allocate space in the internal string_t

    auto* buffer = const_cast<wchar_t*>(result.raw()); // Access raw buffer (mutable)
    size_t buf_pos = 0;
    size_t char_pos = 0;

    // Convert multibyte to wide characters
    while (char_pos < cstr_size) {
        wchar_t wc;
        int len = mbtowc(&wc, raw_data + char_pos, cstr_size - char_pos);
        if (len <= 0) {
            throw libcxx::range_error("Invalid multibyte sequence at position " + libcxx::to_string(char_pos));
        }
        if (buf_pos >= max_size) {
            throw libcxx::runtime_error("Internal buffer overflow (unexpected)");
        }

        buffer[buf_pos++] = wc;
        char_pos += len;
    }

    // Resize to actual length
    result.resize(buf_pos);
    return result;
}

inline string cstrptr_to_string(const char* cstr, size_t size) {
    if (!cstr && size > 0) {
        throw libcxx::invalid_argument("Null pointer with non-zero size");
    }

    if (size == 0) {
        return {}; // Empty string
    }

    // Worst-case size estimation: assume 1 wchar_t per byte as an upper bound
    size_t max_size = size; // Conservative estimate
    string result;
    result.resize(max_size); // Pre-allocate space in the internal string_t

    auto* buffer = const_cast<wchar_t*>(result.raw()); // Access raw buffer (mutable)
    size_t buf_pos = 0;
    size_t char_pos = 0;

    // Convert multibyte to wide characters
    while (char_pos < size) {
        wchar_t wc;
        int len = mbtowc(&wc, cstr + char_pos, size - char_pos);
        if (len <= 0) {
            throw libcxx::range_error("Invalid multibyte sequence at position " + libcxx::to_string(char_pos));
        }
        if (buf_pos >= max_size) {
            throw libcxx::runtime_error("Internal buffer overflow (unexpected)");
        }
        buffer[buf_pos++] = wc;
        char_pos += len;
    }

    // Resize to actual length
    result.resize(buf_pos);
    return result;
}

inline sstring string_to_sstring(const string& wstr) {
    // get the raw wchar_t* data and its size
    const wchar_t* raw_data = wstr.raw(); // c_str equivalent
    size_t wstr_size = wstr.size();

    if (wstr_size == 0) {
        return {}; // empty string
    }

    // worst-case size estimation: each wchar_t could become mb_cur_max bytes
    size_t max_size = wstr_size * MB_CUR_MAX;
    sstring result;
    result.resize(max_size); // pre-allocate space in the internal string_t

    char* buffer = const_cast<char*>(result.raw()); // access raw buffer (mutable)
    size_t buf_pos = 0;
    char temp[MB_CUR_MAX]; // stack-allocated temp buffer for wctomb

    // convert each wchar_t to multibyte
    for (size_t i = 0; i < wstr_size; ++i) {
        int len = wctomb(temp, raw_data[i]);
        if (len <= 0) {
            throw libcxx::range_error("Invalid wchar_t conversion at index " + libcxx::to_string(i));
        }
        if (buf_pos + len > max_size) {
            throw libcxx::runtime_error("Internal buffer overflow (unexpected)");
        }
        for (int j = 0; j < len; ++j) {
            buffer[buf_pos++] = temp[j];
        }
    }

    // resize to actual length and null-terminate
    result.resize(buf_pos);
    return result;
}

inline void strptr_to_cstrptr(const wchar_t* wstr, char* buffer, size_t buffer_size) {
    if (!wstr || !buffer) {
        throw libcxx::invalid_argument("Null pointer provided");
    }
    if (buffer_size == 0) {
        throw libcxx::invalid_argument("Buffer size must be at least 1 for null terminator");
    }

    size_t buf_pos = 0;
    char temp[MB_CUR_MAX]; // stack-allocated temp buffer for each conversion

    for (size_t i = 0; wstr[i] != L'\0'; ++i) {
        int len = wctomb(temp, wstr[i]);
        if (len <= 0) {
            throw libcxx::range_error("Invalid wchar_t conversion at index " + libcxx::to_string(i));
        }
        if (buf_pos + len >= buffer_size) {
            buffer[buf_pos] = '\0'; // null-terminate what we have
            throw libcxx::range_error("Buffer too small at index " + libcxx::to_string(i));
        }
        for (int j = 0; j < len; ++j) {
            buffer[buf_pos++] = temp[j];
        }
    }
    buffer[buf_pos] = '\0'; // null-terminate
}

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_hx_core_m6string
