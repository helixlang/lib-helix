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

#ifndef __$LIBHELIX_MMEMORY__
#define __$LIBHELIX_MMEMORY__

#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "libcxx.h"
#include "primitives.h"

#if defined(__linux__) || defined(__APPLE__)
#include <pthread.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <unistd.h>
#elif defined(_WIN32)
#include <processthreadsapi.h>
#include <tlhelp32.h>
#include <windows.h>
#endif

#include "meta.h"

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Memory {
using LIBCXX_NAMESPACE::move;

template <typename T>
T &&forward(Meta::remove_reference_t<T> &t) noexcept {
    return static_cast<T &&>(t);
}

template <typename T>
T &&forward(Meta::remove_reference_t<T> &&t) noexcept {
    return static_cast<T &&>(t);
}

template <typename T>
    requires(!Meta::is_lvalue_reference<T>)
constexpr Meta::reference_to_pointer_t<T> as_pointer(T &&ref) noexcept {
    return &ref;
}

template <typename T>
constexpr T &&as_reference(T *ptr) noexcept {
    return *ptr;
}

template <class T, class U = T>
constexpr T exchange(T &obj, U &&new_value) noexcept
    requires(Meta::is_nothrow_move_constructible<T> && Meta::is_nothrow_assignable<T &, U>)
{
    T old_value = move(obj);
    obj         = forward<U>(new_value);
    return old_value;
}

enum class AddressType : u8 {
    ROTData,  // (.rodata)
    Stack,    // (stack)
    Heap,     // (heap)
    Unknown   // (in cases like an other program or some other shared memory configuration)
};

inline usize stack_size(usize *size = nullptr) {
    static size_t stack_size = 0;
    static bool   called     = false;

    if (called) {
        return (size != nullptr) ? (*size = usize(stack_size)) : usize(stack_size);
    }

#if defined(__linux__) || defined(__APPLE__)
#if defined(__APPLE__)
    stack_size = pthread_get_stacksize_np(pthread_self());
#elif defined(__linux__)
    pthread_attr_t attr;

    pthread_getattr_np(pthread_self(), &attr);
    pthread_attr_getstacksize(&attr, &stack_size);
    pthread_attr_destroy(&attr);
#endif
#elif defined(_WIN32)
    NT_TIB *tib = reinterpret_cast<NT_TIB *>(NtCurrentTeb());
    stack_size  = usize(reinterpret_cast<uintptr_t>(tib->StackBase) -
                       reinterpret_cast<uintptr_t>(tib->StackLimit));
#endif
    called = true;
    return (size != nullptr) ? (*size = usize(stack_size)) : usize(stack_size);
}

inline void *stack_start(void **start = nullptr) {
    static void *st_addr = nullptr;
    static bool  called  = false;

    if (called) {
        return (start != nullptr) ? (*start = st_addr) : (st_addr);
    }

#if defined(__linux__) || defined(__APPLE__)
    static size_t  st_size = 0;
    pthread_attr_t attr;

#if defined(__linux__)
    pthread_getattr_np(pthread_self(), &attr);
#endif

    pthread_attr_getstack(&attr, &st_addr, &st_size);
    pthread_attr_destroy(&attr);
#elif defined(_WIN32)
    NT_TIB *tib = reinterpret_cast<NT_TIB *>(NtCurrentTeb());
    st_addr     = tib->StackLimit;
#endif
    called = true;
    return (start != nullptr) ? (*start = st_addr) : (st_addr);
}

inline void *stack_end(void **end = nullptr) {
    static void *st_end = nullptr;
    static bool  called = true;

    if (called) {
        return (end != nullptr) ? (*end = st_end) : (st_end);
    }

#if defined(_WIN32)
    NT_TIB *tib = reinterpret_cast<NT_TIB *>(NtCurrentTeb());
    st_end      = tib->StackBase;
#elif defined(__linux__) || defined(__APPLE__)
    st_end = static_cast<void *>(static_cast<char *>(stack_start()) + stack_size());  // NOLINT
#endif
    called = true;
    return (end != nullptr) ? (*end = st_end) : (st_end);
}

inline LIBCXX_NAMESPACE::pair<void *, void *> stack_bounds(void **start = nullptr,
                                                           void **end   = nullptr) {
    LIBCXX_NAMESPACE::pair<void *, void *> pair;

    static void *s_st = stack_start();
    static void *s_ed = stack_end();

    pair.first  = (start != nullptr) ? (*start = s_st) : (s_st);
    pair.second = (end != nullptr) ? (*end = s_ed) : s_ed;

    return pair;
}

inline bool in_stack(const void *ptr) {
    static auto [stack_start, stack_end] = stack_bounds();
    return (ptr >= stack_start && ptr < stack_end);
}

inline void *heap_start(void **start = nullptr) {
    static void *hp_start = nullptr;
    static bool  called   = false;
    if (called) {
        return (start != nullptr) ? (*start = hp_start) : (hp_start);
    }
#if defined(__linux__) || defined(__APPLE__)
#if defined(__clang__) || defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4996)
#endif
    hp_start = libc::sbrk(0);
#if defined(__clang__) || defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif
#elif defined(_WIN32)
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS *)&pmc, sizeof(pmc));
    hp_start = pmc.PrivateUsage;
#endif
    called = true;
    return (start != nullptr) ? (*start = hp_start) : (hp_start);
}

inline bool in_rotdata(const void *ptr) {
#if defined(__linux__) || defined(__APPLE__)
    char mincore_vec;  // NOLINT
    if (mincore(reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(ptr) &
                                         ~static_cast<uintptr_t>(4095)),
                4096,
                &mincore_vec) == 0) {
        return (mincore_vec & 0x4) == 0;
    }
    return false;
#elif defined(_WIN32)
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(ptr, &mbi, sizeof(mbi))) {
        return !(mbi.Protect & PAGE_READWRITE);
    }
    return false;
#endif
}

inline AddressType address_type(const void *ptr) {
    static auto [stack_st, stack_ed] = stack_bounds();
    static auto heap_base            = reinterpret_cast<uintptr_t>(heap_start());
    static auto stack_low            = reinterpret_cast<uintptr_t>(stack_st);
    static auto stack_high           = reinterpret_cast<uintptr_t>(stack_ed);
    auto        addr                 = reinterpret_cast<uintptr_t>(ptr);

    if (addr >= heap_base) {
        return AddressType::Heap;
    }

    if ((addr - stack_low) < (stack_high - stack_low)) {
        return AddressType::Stack;
    }

    if (in_rotdata(ptr)) {
        return AddressType::ROTData;
    }

    return AddressType::Unknown;
}
}  // end namespace Memory

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // __$LIBHELIX_MMEMORY__