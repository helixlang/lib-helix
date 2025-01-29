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

#ifndef __$LIBHELIX_META__
#define __$LIBHELIX_META__

#include "config.h"

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Meta {
namespace _types {
    template <class T, T v>
    struct integral_constant {
        using value_type = T;
        using type       = integral_constant;

        static constexpr const T value = v;

        constexpr            operator value_type() const { return value; }  // NOLINT
        constexpr value_type operator()() const { return value; }
    };
}  // namespace _types

using true_t  = Meta::_types::integral_constant<bool, true>;
using false_t = Meta::_types::integral_constant<bool, false>;

namespace _internal {
    template <class>
    constexpr bool is_const = false;

    template <class T>
    constexpr bool is_const<const T> = true;

    template <class B, class D>
    inline constexpr bool is_derived_of = __is_base_of(B, D);

    template <class T>
    constexpr bool is_class = __is_class(T);

    template <class Up, class T>
    constexpr bool is_convertible = __is_convertible_to(Up, T);

#ifdef __clang__  // clang has an intrinsic for this

    template <class T, class Up>
    constexpr bool same_as = __is_same(T, Up) && __is_same(Up, T);

#else  // use a custom implementation

    template <class, class>
    constexpr bool same_as = false;

    template <class T>
    constexpr bool same_as<T, T> = true;

#endif

struct is_referenceable_helper {
    template <typename T>
    static T &test(int);  // Prefers this overload if T is referenceable
    template <typename T>
    static false_t test(...);  // Fallback for non-referenceable types
};
}  // namespace _internal

namespace _types {
template <typename T>
struct remove_reference {
    using type = T;
};

template <typename T>
struct remove_reference<T &> {
    using type = T;
};

template <typename T>
struct remove_reference<T &&> {
    using type = T;
};

template <typename T>
struct remove_const {
    using type = T;
};

template <typename T>
struct remove_const<const T> {
    using type = T;
};

template <typename T>
struct remove_const_volatile {
    using type = T;
};

template <typename T>
struct remove_const_volatile<const T> {
    using type = T;
};

template <typename T>
struct remove_const_volatile<volatile T> {
    using type = T;
};

template <typename T>
struct remove_const_volatile<const volatile T> {
    using type = T;
};

template <typename T>
struct remove_cvref {
    using type = typename remove_const_volatile<typename remove_reference<T>::type>::type;
};

template <typename T>
struct reference_to_pointer {
    using type = T;
};

template <typename T>
struct reference_to_pointer<T &> {
    using type = T *;
};

template <typename T>
struct reference_to_pointer<T &&> {
    using type = T *;
};

template <class T>
struct is_lvalue_reference : public false_t {};

template <class T>
struct is_lvalue_reference<T &> : public true_t {};

template <class T>
struct is_rvalue_reference : public false_t {};

template <class T>
struct is_rvalue_reference<T &&> : public true_t {};

template <class T>
struct is_reference : public false_t {};

template <class T>
struct is_reference<T &> : public true_t {};

template <class T>
struct is_reference<T &&> : public true_t {};

template <typename T, bool = (!Meta::_internal::same_as<decltype(_internal::is_referenceable_helper::test<T>(0)), false_t>)>
struct add_rvalue_reference {
    using type = T;
};

template <typename T>
struct add_rvalue_reference<T, true> {
    using type = T &&;
};

template <typename T, bool = (!Meta::_internal::same_as<decltype(_internal::is_referenceable_helper::test<T>(0)), false_t>)>
struct add_lvalue_reference {
    using type = T;
};

template <typename T>
struct add_lvalue_reference<T &&, true> {
    using type = T &;
};

template <typename T>
struct add_lvalue_reference<T, true> {
    using type = T &;
};

template <typename T>
struct is_nothrow_move_constructible
    : Meta::_types::integral_constant<bool,
                                      __is_nothrow_constructible(
                                          T, typename _types::add_rvalue_reference<T>::type)> {};

template <typename T, typename Arg>
struct is_nothrow_assignable
    : Meta::_types::integral_constant<bool, __is_nothrow_assignable(T, Arg)> {};

template <class T>
struct add_const {
    using type = const T;
};

template <class T>
struct is_copy_constructible
    : public Meta::_types::integral_constant<
          bool,  __is_constructible(T, typename add_lvalue_reference<typename add_const<T>::type>::type)> {};
}  // namespace _types

template <bool, class T = void>
struct enable_if {};

template <class T>
struct enable_if<true, T> {
    using type = T;
};

/// portable bindings for declval
template <class T>
T &&__declval(int);  // NOLINT
template <class T>
T __declval(long);  // NOLINT

template <class T>
decltype(__declval<T>(0)) declval() noexcept {
    static_assert(
        !__is_same(T, T),
        "H_STD_NAMESPACE::declval can only be used in an unevaluated context. "
        "It's likely that your current usage is trying to extract a value from the function.");
}

template <class Up, class T>
concept convertible_to = _internal::is_convertible<Up, T> && requires { static_cast<T>(declval<Up>()); };

template <class T, class Up>
concept same_as = _internal::same_as<T, Up>;

template <class T>
concept is_const = _internal::is_const<T>;

template <class T>
concept is_class = _internal::is_class<T>;

template <class B, class D>
concept is_derived_of = _internal::is_derived_of<B, D>;

template <typename T>
concept is_referenceable = !Meta::same_as<decltype(_internal::is_referenceable_helper::test<T>(0)), false_t>;

template <class T>
concept is_rvalue_reference = _types::is_rvalue_reference<T>::value;

template <class T>
concept is_lvalue_reference = _types::is_lvalue_reference<T>::value;

template <class T>
concept is_reference = _types::is_reference<T>::value;

template <typename T>
concept is_nothrow_move_constructible = _types::is_nothrow_move_constructible<T>::value;

template <typename T, typename Arg>
concept is_nothrow_assignable = _types::is_nothrow_assignable<T, Arg>::value;

template <class T>
concept is_copy_constructible = _types::is_copy_constructible<T>::value;

template <class T>
using remove_const_t = typename _types::remove_const<T>::type;

template <typename T>
using remove_reference_t = typename _types::remove_reference<T>::type;

template <typename T>
using remove_const_volatile_t = typename _types::remove_const_volatile<T>::type;

template <typename T>
using remove_cvref_t = typename _types::remove_cvref<T>::type;

template <typename T>
using reference_to_pointer_t = typename _types::reference_to_pointer<T>::type;

template <class T>
using add_const_t = typename _types::add_const<T>::type;

template <typename T>
using add_rvalue_reference_t = typename _types::add_rvalue_reference<T>::type;

template <typename T>
using add_lvalue_reference_t = typename _types::add_lvalue_reference<T>::type;
}  // namespace Meta

H_STD_NAMESPACE_END
H_NAMESPACE_END
#endif