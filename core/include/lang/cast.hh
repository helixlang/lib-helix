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
/// \file cast.hh                                                                                ///
///                                                                                              ///
/// Provides `as` casting semantics in Helix, enabling safe and versatile casting operations.    ///
/// These include standard type casting (`as_cast`), const casting (`as_const`), and unsafe      ///
/// reinterpretation casting (`as_unsafe`). This framework is designed to integrate seamlessly   ///
/// with Helix's runtime and type system, supporting dynamic, static, and user-defined casting   ///
/// mechanisms where appropriate.                                                                ///
///                                                                                              ///
/// ### Purpose                                                                                  ///
/// - Facilitates the use of Helix-specific `as` casting constructs.                             ///
/// - Offers a type-safe interface for performing various cast operations, adhering to Helix's   ///
///   runtime semantics and ensuring compatibility with custom user-defined types.               ///
///                                                                                              ///
/// ### Features                                                                                 ///
/// #### `as_cast`                                                                               ///
/// - The general-purpose casting function, supporting:                                          ///
///   - Const casting.                                                                           ///
///   - Pointer casting (dynamic and static based on type constraints).                          ///
///   - Reference casting.                                                                       ///
///   - Helix-specific `operator$cast` for user-defined casting.                                 ///
///   - Default static casting as a fallback.                                                    ///
///                                                                                              ///
/// #### `as_const`                                                                              ///
/// - Ensures that a value is treated as const, regardless of its current qualifiers.            ///
/// - Can be used with both mutable and immutable inputs.                                        ///
///                                                                                              ///
/// #### `as_unsafe`                                                                             ///
/// - Enables unsafe reinterpretation of types using `reinterpret_cast`.                         ///
/// - Primarily intended for low-level operations where type safety is explicitly disregarded.   ///
///                                                                                              ///
/// ### Examples                                                                                 ///
/// ```helix                                                                                     ///
/// let x: i32 = 42;                                                                             ///
/// let y: i32* = x as i32*;             // Uses `as_cast` for pointer casting.                  ///
/// let z: const i32* = x as const i32*; // Uses `as_const` for const pointer casting.           ///
/// let u: usize = y as unsafe usize;    // Uses `as_unsafe` for reinterpretation.               ///
/// ```                                                                                          ///
///                                                                                              ///
/// ### Notes                                                                                    ///
/// - The `as_cast` function intelligently selects the appropriate casting method based on type  ///
///   traits and user-defined concepts, ensuring both safety and flexibility.                    ///
/// - Unsafe casting (`as_unsafe`) should be used sparingly and only when absolutely necessary.  ///
///                                                                                              ///
/// ### Limitations                                                                              ///
/// - While `as_cast` supports dynamic and static casting for pointers, it relies on type        ///
///   information and user-defined constraints to determine the safest approach.                 ///
/// - The `as_unsafe` function bypasses all safety checks, and misuse can lead to ub.            ///
///                                                                                              ///
/// ### Implementation Details                                                                   ///
/// - `as_cast` leverages concepts like `SupportsPointerCast` and `SafelyCastable` to ensure     ///
///   compatibility with Helix-specific features.                                                ///
/// - `as_const` and `as_unsafe` provide implementations for const and unsafe casting.           ///
///                                                                                              ///
///----------------------------------------------------------------------------------------------///

#ifndef __$LIBHELIX_CAST__
#define __$LIBHELIX_CAST__

#include "../concepts.h"
#include "../config.h"
#include "../traits.h"

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

// ------------------------------------------- as_cast ------------------------------------------ //
/// Performs a cast from type `Up` to type `Ty`, selecting the most appropriate casting mechanism.
/// \tparam Ty The target type.
/// \tparam Up The source type.
/// \param value The value to be cast.
/// \return The value cast to the target type.
template <typename Ty, typename Up>
constexpr Ty as_cast(Up &value);

/// Performs a cast from type `Up` to type `Ty`, selecting the most appropriate casting mechanism.
/// \tparam Ty The target type.
/// \tparam Up The source type.
/// \param value The value to be cast.
/// \return The value cast to the target type, removing const qualifiers.
template <typename Ty, typename Up>
constexpr Ty as_cast(const Up &value);

// ------------------------------------------ as_const ------------------------------------------ //
/// Performs a const-correct cast from type `Up` to type `Ty`.
/// \tparam Ty The target const-qualified type.
/// \tparam Up The source type.
/// \param value The value to be cast.
/// \return A const-qualified reference to the value.
template <typename Ty, typename Up>
constexpr const Ty &as_const(Up &value);

/// Performs a const-correct cast from type `Up` to type `Ty`.
/// \tparam Ty The target const-qualified type.
/// \tparam Up The source type.
/// \param value The value to be cast.
/// \return A const-qualified reference to the value.
template <typename Ty, typename Up>
constexpr const Ty &as_const(const Up &value);

// ------------------------------------------ as_unsafe ----------------------------------------- //
/// Performs an unsafe reinterpretation cast from type `Up` to type `Ty`.
/// \tparam Ty The target type.
/// \tparam Up The source type.
/// \param value The value to be cast.
/// \return The value reinterpret-cast to the target type.
template <typename Ty, typename Up>
constexpr Ty as_unsafe(Up value);

/// Performs an unsafe reinterpretation cast from type `Up` to type `Ty`.
/// \tparam Ty The target type.
/// \tparam Up The source type.
/// \param value The value to be cast.
/// \return The value reinterpret-cast to the target type.
template <typename Ty, typename Up>
constexpr const Ty as_unsafe(const Up value)  // NOLINT
    requires(H_STD_NAMESPACE::traits::_const::utils::is_const_v<Up> ||
             H_STD_NAMESPACE::traits::_const::utils::is_const_v<Ty>);

// ------------------------------------------- as_cast ------------------------------------------ //
template <typename Ty, typename Up>
constexpr Ty as_cast(Up &value) {
    if constexpr (LIBCXX_NAMESPACE::is_const_v<LIBCXX_NAMESPACE::remove_reference_t<Up>> &&
                  LIBCXX_NAMESPACE::is_same_v<LIBCXX_NAMESPACE::remove_const_t<Up>, Ty>) {
        return const_cast<Ty>(value);
    } else if constexpr (LIBCXX_NAMESPACE::is_pointer_v<Ty>) {
        if constexpr (H_STD_NAMESPACE::concepts::SupportsPointerCast<Up, Ty>) {
            return dynamic_cast<Ty>(value);
        } else {
            return static_cast<Ty>(value);
        }
    } else if constexpr (H_STD_NAMESPACE::concepts::SafelyCastable<Up, Ty>) {
        return value.operator$cast(static_cast<Ty *>(nullptr));
    } else {
        return static_cast<Ty>(value);
    }
}

template <typename Ty, typename Up>
constexpr Ty as_cast(const Up &value) {
    return as_cast<Ty>(const_cast<Up &>(value));  // NOLINT
}

// ------------------------------------------ as_const ------------------------------------------ //
template <typename Ty, typename Up>
constexpr const Ty &as_const(Up &value) {
    return const_cast<const Ty &>(value);
}

template <typename Ty, typename Up>
constexpr const Ty &as_const(const Up &value) {
    return static_cast<const Ty &>(value);
}

// ------------------------------------------ as_unsafe ----------------------------------------- //
template <typename Ty, typename Up>
constexpr Ty as_unsafe(Up value) {
    return reinterpret_cast<Ty>(value);  // NOLINT
}

template <typename Ty, typename Up>
constexpr const Ty as_unsafe(const Up value)  // NOLINT
    requires(H_STD_NAMESPACE::traits::_const::utils::is_const_v<Up> ||
             H_STD_NAMESPACE::traits::_const::utils::is_const_v<Ty>) {
    return reinterpret_cast<const Ty>(value);  // NOLINT
}

H_STD_NAMESPACE_END
H_NAMESPACE_END
#endif