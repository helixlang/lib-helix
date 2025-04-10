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

#ifndef _$_HX_CORE_M10PRIMITIVES
#define _$_HX_CORE_M10PRIMITIVES

#include <include/config/config.h>

#include <include/c++/libc++.hh>
#include <include/meta/traits.hh>
#include <include/meta/enable_if.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

class null_t {};
H_STD_NAMESPACE_END
H_NAMESPACE_END

using u8 = unsigned char;
using i8 = signed char;

using u16 = unsigned short;
using i16 = signed short;

using u32 = unsigned int;
using i32 = signed int;

using u64 = unsigned long long;
using i64 = signed long long;

using f32 = float;
using f64 = double;
using f80 = long double;

#if defined(__LP64__) || defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__) ||      \
    defined(__aarch64__) || defined(__arm64__) || defined(__mips64__) || defined(__mips64) || \
    defined(__mips64el__) || defined(__mips64el) || defined(__s390x__) || defined(__HELIX_64BIT__)
#define __HELIX_64BIT__
using usize = u64;
using isize = i64;
#elif defined(__ILP32__) || defined(_WIN32) || defined(__i386__) || defined(__arm__) || \
    defined(__mips__) || defined(__mips) || defined(__mipsel__) || defined(__mipsel) || \
    defined(__s390__) || defined(__HELIX_32BIT__)
#define __HELIX_32BIT__
using usize = u32;
using isize = i32;
#elif defined(__16BIT__) || defined(__MSP430__) || defined(__AVR__) || defined(__HELIX_16BIT__)
#define __HELIX_16BIT__
using usize = u16;
using isize = i16;
#elif defined(__8BIT__) || defined(__HELIX_8BIT__)
#define __HELIX_8BIT__
using usize = u8;
using isize = i8;
#else
#error \
    "Helix core: Unable to determine platform bitness. Supported macros: __HELIX_64BIT__, __HELIX_32BIT__, __HELIX_16BIT__, __HELIX_8BIT__. Define one explicitly using '-D' during compilation."
#endif

using _H_RESERVED$char = wchar_t;

namespace helix::std::Legacy {
using _H_RESERVED$char = char;
}

inline constexpr helix::std::null_t null;

// FIXME: remove later
template <typename T>
using vec = helix::libcxx::vector<T>;

template <typename K, typename V>
using map = helix::libcxx::unordered_map<K, V>;

template <typename T, usize S>
using array = helix::libcxx::array<T, S>;

template <typename T>
using list = helix::libcxx::list<T>;

template <typename T>
using set = helix::libcxx::set<T>;

template <typename... T>
using tuple = helix::libcxx::tuple<T...>;

struct i128;

struct u128 {
    u64 high;
    u64 low;

    // Constructors
    u128();
    u128(u64 val);
    u128(u32 val);
    u128(u16 val);
    u128(u8 val);
    u128(i64 val);
    u128(i32 val);
    u128(i16 val);
    u128(i8 val);
    u128(u64 high, u64 low);
    u128(const i128 &x);

    // Arithmetic Operators
    u128 operator+(const u128 &other) const;
    u128 operator-(const u128 &other) const;
    u128 operator*(const u128 &other) const;
    u128 operator/(const u128 &divisor) const;
    u128 operator%(const u128 &divisor) const;

    // Bitwise Operators
    u128 operator&(const u128 &other) const;
    u128 operator|(const u128 &other) const;
    u128 operator^(const u128 &other) const;
    u128 operator~() const;
    u128 operator<<(int shift) const;
    u128 operator>>(int shift) const;

    // Comparison Operators
    bool operator==(const u128 &other) const;
    bool operator!=(const u128 &other) const;
    bool operator<(const u128 &other) const;
    bool operator>(const u128 &other) const;
    bool operator<=(const u128 &other) const;
    bool operator>=(const u128 &other) const;

    // Assignment Operators
    u128 &operator=(const u128 &other);
    u128 &operator+=(const u128 &other);
    u128 &operator-=(const u128 &other);
    u128 &operator*=(const u128 &other);
    u128 &operator/=(const u128 &other);
    u128 &operator%=(const u128 &other);
    u128 &operator&=(const u128 &other);
    u128 &operator|=(const u128 &other);
    u128 &operator^=(const u128 &other);
    u128 &operator<<=(int shift);
    u128 &operator>>=(int shift);

    // Increment/Decrement Operators
    u128 &operator++();
    u128  operator++(int);
    u128 &operator--();
    u128  operator--(int);

    // Unary Operators
    u128 operator+() const;
    u128 operator-() const;

  private:
    static u128 mul_u64_to_u128(u64 a, u64 b);
};

struct i128 {
    u64 high;
    u64 low;

    // Constructors
    i128();
    i128(u64 val);
    i128(u32 val);
    i128(u16 val);
    i128(u8 val);
    i128(i64 val);
    i128(i32 val);
    i128(i16 val);
    i128(i8 val);
    i128(const u128 &x);
    i128(u64 high, u64 low);

    // Helper Functions
    bool is_negative() const;

    // Arithmetic Operators
    i128 operator+(const i128 &other) const;
    i128 operator-(const i128 &other) const;
    i128 operator*(const i128 &other) const;
    i128 operator/(const i128 &other) const;
    i128 operator%(const i128 &other) const;

    // Bitwise Operators
    i128 operator&(const i128 &other) const;
    i128 operator|(const i128 &other) const;
    i128 operator^(const i128 &other) const;
    i128 operator~() const;
    i128 operator<<(int shift) const;
    i128 operator>>(int shift) const;

    // Comparison Operators
    bool operator==(const i128 &other) const;
    bool operator!=(const i128 &other) const;
    bool operator<(const i128 &other) const;
    bool operator>(const i128 &other) const;
    bool operator<=(const i128 &other) const;
    bool operator>=(const i128 &other) const;

    // Assignment Operators
    i128 &operator=(const i128 &other);
    i128 &operator+=(const i128 &other);
    i128 &operator-=(const i128 &other);
    i128 &operator*=(const i128 &other);
    i128 &operator/=(const i128 &other);
    i128 &operator%=(const i128 &other);
    i128 &operator&=(const i128 &other);
    i128 &operator|=(const i128 &other);
    i128 &operator^=(const i128 &other);
    i128 &operator<<=(int shift);
    i128 &operator>>=(int shift);

    // Increment/Decrement Operators
    i128 &operator++();
    i128  operator++(int);
    i128 &operator--();
    i128  operator--(int);

    // Unary Operators
    i128 operator+() const;
    i128 operator-() const;
};

#endif  // _$_HX_CORE_M10PRIMITIVES
