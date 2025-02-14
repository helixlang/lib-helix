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

#ifndef __$LIBHELIX_LIBCXX__
#define __$LIBHELIX_LIBCXX__

#include <algorithm>
#include <any>
#include <array>
#include <cassert>
#include <concepts>
#include <coroutine>
#include <cstddef>
#include <iterator>
#include <map>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <vector>

#include "config.h"
#include "meta.h"
#include "primitives.h"
#include "types/forward.hh"

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Memory {
inline void *copy(void *dest, const void *src, size_t n) noexcept {
    return LIBCXX_NAMESPACE::memcpy(dest, src, n);
}

inline void *move(void *dest, const void *src, size_t n) noexcept {
    return LIBCXX_NAMESPACE::memmove(dest, src, n);
}

inline void *set(void *dest, int value, size_t n) noexcept {
    return LIBCXX_NAMESPACE::memset(dest, value, n);
}

template <typename T>
inline std::Questionable<T *> find(const T *src, int value, size_t n) noexcept {
    void *result = LIBCXX_NAMESPACE::memchr(src, value, n);
    return result ? static_cast<T *>(result) : null;
}

inline int compare(const void *a, const void *b, size_t n) noexcept {
    return LIBCXX_NAMESPACE::memcmp(a, b, n);
}

template <typename T>
T &&forward(Meta::remove_reference_t<T> &t) noexcept;

template <typename T>
T &&forward(Meta::remove_reference_t<T> &&t) noexcept;
}  // namespace Memory

namespace String {
template <typename T>
constexpr inline T *concat(T *dest, const T *src) noexcept {
    return LIBCXX_NAMESPACE::wcscat(dest, src);
}

template <typename T>
constexpr inline T *concat_n(T *dest, const T *src, size_t n) noexcept {
    return LIBCXX_NAMESPACE::wcsncat(dest, src, n);
}

template <typename T>
constexpr inline T *copy(T *dest, const T *src) noexcept {
    return LIBCXX_NAMESPACE::wcscpy(dest, src);
}

template <typename T>
constexpr inline T *copy_n(T *dest, const T *src, size_t n) noexcept {
    return LIBCXX_NAMESPACE::wcsncpy(dest, src, n);
}

template <typename T>
constexpr inline std::Questionable<T *> find(const T *str, int c) noexcept {
    T *result = LIBCXX_NAMESPACE::wcschr(str, c);
    return result ? result : null;
}

template <typename T>
constexpr inline std::Questionable<T *> find_last(const T *str, int c) noexcept {
    T *result = LIBCXX_NAMESPACE::wcsrchr(str, c);
    return result ? result : null;
}

template <typename T>
constexpr inline std::Questionable<T *> find_any(const T *str, const T *accept) noexcept {
    T *result = LIBCXX_NAMESPACE::wcspbrk(str, accept);
    return result ? result : null;
}

template <typename T>
constexpr inline std::Questionable<T *> find_sub(const T *haystack, const T *needle) noexcept {
    T *result = LIBCXX_NAMESPACE::wcsstr(haystack, needle);
    return result ? result : null;
}

template <typename T>
constexpr inline vec<T *> split(T *str, const T *delim) noexcept {
    vec<T *> tokens;
    T       *token = LIBCXX_NAMESPACE::wcstok(str, delim, nullptr);
    while (token) {
        tokens.push_back(token);
        token = LIBCXX_NAMESPACE::wcstok(nullptr, delim, nullptr);
    }
    return tokens;
}

template <typename T>
constexpr inline size_t length(const T *str) noexcept {
    return LIBCXX_NAMESPACE::wcslen(str);
}

template <typename T>
constexpr inline size_t prefix_length(const T *str, const T *chars, bool exclude = false) noexcept {
    return exclude ? LIBCXX_NAMESPACE::wcscspn(str, chars) : LIBCXX_NAMESPACE::wcsspn(str, chars);
}

template <typename T>
constexpr inline int compare(const T *a, const T *b) noexcept {
    return LIBCXX_NAMESPACE::wcscmp(a, b);
}

template <typename T>
constexpr inline int compare_n(const T *a, const T *b, size_t n) noexcept {
    return LIBCXX_NAMESPACE::wcsncmp(a, b, n);
}

template <typename T>
constexpr inline int compare_locale(const T *a, const T *b) noexcept {
    return LIBCXX_NAMESPACE::wcscoll(a, b);
}

template <typename T>
constexpr inline size_t transform(T *dest, const T *src, size_t n) noexcept {
    return LIBCXX_NAMESPACE::wcsxfrm(dest, src, n);
}

constexpr inline const char *error(int errnum) noexcept {
    return LIBCXX_NAMESPACE::strerror(errnum);  // No modern alternative for this
}
}  // namespace String

template <typename _Tp, typename... _Ty>
constexpr _Tp *_H_RESERVED$new(_Ty &&...t) {                      // NOLINT
    return new _Tp(H_STD_NAMESPACE::Memory::forward<_Ty>(t)...);  // NOLINT
}

///
/// \brief Dynamically allocates memory for a type T, respecting T's alignment,
///        and then constructs (placement-new) the object.
///
/// \tparam T   The object type to allocate and construct.
/// \tparam Args Constructor argument types.
/// \param  args Constructor arguments forwarded to T's constructor.
/// \return      Pointer to the newly constructed T.
/// \throws      std::bad_alloc if memory allocation fails.
///
template <typename T, typename... Args>
T *make_aligned(Args &&...args) {
    static_assert(alignof(T) <= __STDCPP_DEFAULT_NEW_ALIGNMENT__ || __cpp_aligned_new,
                  "Your compiler does not support over-aligned allocations.");

    void *raw_mem = nullptr;
#ifdef _MSC_VER
    raw_mem = _aligned_malloc(sizeof(T), alignof(T));

    if (!raw_mem) {
        throw LIBCXX_NAMESPACE::bad_alloc();
    }
#else
    raw_mem = ::operator new(sizeof(T), LIBCXX_NAMESPACE::align_val_t(alignof(T)));
#endif

    return ::new (raw_mem) T(LIBCXX_NAMESPACE::forward<Args>(args)...);
}

///
/// \brief Destroys and deallocates an object previously created by make_aligned.
///
/// \tparam T   The object type to destroy.
/// \param ptr  Pointer to the object to destroy and deallocate.
///
/// Calling destroy_aligned on a pointer not allocated by make_aligned<T> is undefined.
///
template <typename T>
void destroy_aligned(T *ptr) {
    if (!ptr) {
        return;
    }

    ptr->~T();
#ifdef _MSC_VER
    _aligned_free(ptr);
#else
    ::operator delete(ptr, LIBCXX_NAMESPACE::align_val_t(alignof(T)));
#endif
}

H_NAMESPACE_END
H_STD_NAMESPACE_END

#endif