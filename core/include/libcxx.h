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

#include <any>
#include <array>
#include <cassert>
#include <algorithm>
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

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Memory {
template <typename T>
T &&forward(Meta::remove_reference_t<T> &t) noexcept;

template <typename T>
T &&forward(Meta::remove_reference_t<T> &&t) noexcept;
}  // namespace Memory

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