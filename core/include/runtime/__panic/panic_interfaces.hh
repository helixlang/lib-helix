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

#ifndef _$_HX_CORE_M16PANIC_INTERFACES
#define _$_HX_CORE_M16PANIC_INTERFACES

#include <include/config/config.h>
#include <include/meta/meta.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Panic {
/// \class Frame
///
/// A forward declaration of the `Panic::Frame` class, which encapsulates the context of a panic
/// event.
///
/// ### Purpose
/// The `Frame` class represents the core structure for managing panic states in Helix.
/// It tracks error details such as the file, line number, and error reason, and interacts with
/// Helix's panic-handling system.
///
/// ### Usage
/// This forward declaration allows other components, such as `Panic` utilities and handlers,
/// to reference the `Frame` class without requiring its full definition.
///
/// ### Related
/// - See the full definition of `Panic::Frame` for detailed documentation.
/// - Used in conjunction with `Panicking` interfaces and
/// `HX_FN_Vi_Q5_13_helixpanic_handler_Q3_5_5_stdPanicFrame_C_PK_Rv`.
class Frame;

namespace Interface {
    /// \concept Panicking
    ///
    /// A concept to determine whether a type `T` can invoke the panic operator.
    /// A type satisfies the `Panicking` concept if it implements either:
    /// - A static panic operator (`T::operator$panic`).
    /// - An instance panic operator (`T.operator$panic`).
    ///
    /// ### Details
    /// - The panic operator must return a `string` or a type convertible to `string`.
    ///
    /// ### Example
    /// ```cpp
    /// class Error {
    /// public:
    ///     string operator$panic() { return "Error occurred"; }
    /// };
    ///
    /// static_assert(Panic::Interface::Panicking<Error>);
    /// ```
    template <typename T>
    concept Panicking = requires(T obj) {
        { obj.operator$panic() } -> std::Meta::convertible_to<string>;
    } || requires {
        { T::operator$panic() } -> std::Meta::convertible_to<string>;
    };

    /// \concept PanickingStatic
    ///
    /// A concept to determine whether a type `T` implements a static panic operator
    /// (`T::operator$panic`).
    ///
    /// ### Details
    /// - The static panic operator must return a `string` or a type convertible to `string`.
    ///
    /// ### Example
    /// ```cpp
    /// class Error {
    /// public:
    ///     static string operator$panic() { return "Static error occurred"; }
    /// };
    ///
    /// static_assert(Panic::Interface::PanickingStatic<Error>);
    /// ```
    template <typename T>
    concept PanickingStatic = requires(T obj) {
        { T::operator$panic() } -> std::Meta::convertible_to<string>;
    };

    /// \concept PanickingInstance
    ///
    /// A concept to determine whether a type `T` implements an instance panic operator
    /// (`T.operator$panic`).
    ///
    /// ### Details
    /// - The instance panic operator must return a `string` or a type convertible to `string`.
    ///
    /// ### Example
    /// ```cpp
    /// class Error {
    /// public:
    ///     string operator$panic() { return "Instance error occurred"; }
    /// };
    ///
    /// static_assert(Panic::Interface::PanickingInstance<Error>);
    /// ```
    template <typename T>
    concept PanickingInstance = requires(T obj) {
        { obj.operator$panic() } -> std::Meta::convertible_to<string>;
    };
}  // namespace Interface
}  // namespace Panic

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M16PANIC_INTERFACES
