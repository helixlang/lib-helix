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
///                                                                                              ///
///  this is still a work in progress, the $question class is a work in progress and is not yet  ///
///  fully implemented, the $question class is a class that is similar to the
///  H_STD_NAMESPACE::optional but  /// also adds H_STD_NAMESPACE::expected like functionality, the
///  $question class is a class that can hold    /// one of 3 states: value, an error, or be null.
///  ///
///                                                                                              ///
///  the $question class is a class that is to be used inside the core since its required by the ///
///  compiler to allow for `?` type syntax.                                                      ///
///                                                                                              ///
///----------------------------------------------------------------------------------------------///

#ifndef __$LIBHELIX_QUESTION__
#define __$LIBHELIX_QUESTION__

#include "../config.h"
#include "../lang/panic.hh"
#include "../memory.h"
#include "../meta.h"
#include "../primitives.h"
#include "../types.h"
#include "../types/errors.h"


H_NAMESPACE_BEGIN

/// \class $question
///
/// Represents Helix's `?` type, also known as the **quantum type**. This type can hold either:
/// - A valid value of type `T`,
/// - A null state, or
/// - An error represented by a `H_STD_NAMESPACE::Panic::Frame`.
///
/// ### Unique Concept
/// The `$question` type is unique to Helix and is a core component of Helix's type system, coined
/// as the **quantum type** because its state  (value, null, or error) remains unknown until
/// explicitly checked or "collapsed." This makes it more versatile than traditional nullable or
/// optional types. It enables developers to work with quantum-like values (`?`) that
/// simplify null checks, error propagation, and value retrieval while ensuring type safety.
///
/// ### States
/// `$question` operates in one of three states:
/// - **Value**: Holds a valid object of type `T`.
/// - **Null**: Represents the absence of a value.
/// - **Error**: Encapsulates an error using a `H_STD_NAMESPACE::Panic::Frame`.
///
/// ### Key Features
/// - **Type Safety**: Enforces constraints such as copy and move constructibility for valid types.
/// - **Error Containment**: Encodes errors using `H_STD_NAMESPACE::Panic::Frame`, enabling
/// structured error
///                          handling.
/// - **State Inspection**: Provides methods to determine whether the instance is null, contains a
///                         value, or holds an error of a specific type.
/// - **Casting**: Supports safe casting of stored values and errors with explicit checks to prevent
///                type mismatches.
/// - **Integration**: Designed to work seamlessly with Helix's type system and error-handling
///                    mechanisms.
///
/// ### Use Cases
/// `$question` is particularly useful in scenarios where:
/// - A value may or may not be present.
/// - Functions can fail, and errors must be propagated or inspected.
/// - Null states need to be represented explicitly without relying on sentinel values.
///
/// ### Construction
/// `$question` supports the following constructions:
/// - **Default (Null)**: `T? x;` or `T? x = null;` creates a null instance.
/// - **Value Initialization**: `T? x = value;` stores a valid value.
/// - **Error Initialization**: `T? x = H_STD_NAMESPACE::Panic::Frame(...);` stores an error state.
///
/// ### Core Behaviors
/// - **State Queries**:
///   - `bool operator$question() const`: Returns `true` if the state is `Value`.
///   - `bool is_null() const`: Checks if the state is `Null`.
///   - `bool is_err() const`: Checks if the state is `Error`.
///   - `bool is_err(const type_info *type) const`: Checks if the error matches a specific type.
/// - **Value Access**:
///   - `T operator*()`: Retrieves the value, or throws if null or in an error state.
///   - `T operator$cast(T * /*unused*/) const`: Casts the value to `T`, or throws if null or in an
///   error state.
/// - **Error Inspection**:
///   - `bool operator$contains(const E & /* unused */) const`: Returns `true` if the error matches
///   the type `E`.
/// - **Equality**:
///   - `bool operator==(const null_t & /*unused*/) const`: Checks for null equality.
///   - `bool operator==(const E & /*unused*/) const`: Checks if the error matches a specific type.
///
/// ### Guarantees
/// 1. Default initialization creates a null state.
/// 2. Errors are always stored as `H_STD_NAMESPACE::Panic::Frame` objects.
/// 3. Errors and values are managed with proper resource allocation and destruction to avoid leaks
///    or undefined behavior.
///
/// ### Related
/// - `H_STD_NAMESPACE::Panic::Frame`: Represents the error state.
/// - `H_STD_NAMESPACE::Panic::Panicking`: Concept for types supporting the `operator$panic()`
/// function.
/// - `H_STD_NAMESPACE::Panic::PanickingInstance`: Concept for instances supporting
/// `operator$panic()`.
/// - `H_STD_NAMESPACE::Panic::PanickingStatic`: Concept for types supporting static
/// `operator$panic()`.
///
/// ### Example Usage
/// ```helix
/// fn process_data() -> int? {
///     panic H_STD_NAMESPACE::errors::RuntimeError("Data processing failed.");
/// }
///
/// fn main() -> i32 {
///     let x: int? = 42;                   // Holds a valid value.
///     let y: int? = null;                 // Null state.
///
///     let z: int? = process_data();       // Holds an error.
///
///     if x? {
///         print(x as i32 /* or '*x' */);  // Collapse the value. if the value is invalid the
///                                         // error gets thrown and can be caught using 'try-catch'
///     } else if x $contains RuntimeError {
///         print("Error occurred.");       // Handle specific error types.
///     }
///
///     return 0;
/// }
/// ```
///
/// ### Notes
/// `$question` is designed for developers to handle nullable and error-prone states seamlessly. It
/// ensures type safety and enforces proper construction and destruction of resources.
template <class T>
class $question {
  private:
    enum class $State : char { Value, Null, Error };

    union $StorageT {
        mutable H_STD_NAMESPACE::Panic::Frame error;
        mutable T                             value;

        constexpr $StorageT() noexcept
            : value() {}
        constexpr ~$StorageT() noexcept {}
    };

    mutable $State state = $State::Null;
    $StorageT      data;

    [[nodiscard]] constexpr bool is_null() const noexcept { return state == $State::Null; }
    [[nodiscard]] constexpr bool is_err() const noexcept { return state == $State::Error; }
    [[nodiscard]] constexpr bool is_err(const LIBCXX_NAMESPACE::type_info *type) const noexcept {
        return state == $State::Error && data.error.get_context() == type;
    }

    constexpr void set_value(const T &value) { new (&data.value) T(value); }
    constexpr void set_value(T &&value) {
        new (&data.value) T(H_STD_NAMESPACE::memory::move(value));
    }
    constexpr void set_err(H_STD_NAMESPACE::Panic::Frame &&error) {
        new (&data.error) H_STD_NAMESPACE::Panic::Frame(H_STD_NAMESPACE::memory::move(error));
    }

    constexpr void delete_error() noexcept { data.error.~Frame(); }
    constexpr void delete_value() noexcept { data.value.~T(); }

  public:
    /// ------------------------------- Constructors (Null) -------------------------------
    constexpr $question() noexcept
        : state($State::Null) {}
    constexpr $question(const null_t &) noexcept
        : state($State::Null) {}
    constexpr $question(null_t &&) noexcept
        : state($State::Null) {}

    /// ------------------------------- Constructors (Value) -------------------------------
    constexpr $question(const T &value)
        : state($State::Value) {
        set_value(value);
    }
    constexpr $question(T &&value)
        : state($State::Value) {
        set_value(H_STD_NAMESPACE::memory::move(value));
    }

    /// ------------------------------- Constructors (Error) -------------------------------
    constexpr $question(const H_STD_NAMESPACE::Panic::Frame &error)
        : state($State::Error) {
        set_err(error);
    }
    constexpr $question(H_STD_NAMESPACE::Panic::Frame &&error)
        : state($State::Error) {
        set_err(H_STD_NAMESPACE::memory::move(error));
    }

    /// ------------------------------- Move Constructor & Assignment
    /// -------------------------------
    constexpr $question($question &&other) noexcept
        : state(other.state) {
        if (state == $State::Error) {
            set_err(H_STD_NAMESPACE::memory::move(other.data.error));
        } else if (state == $State::Value) {
            set_value(H_STD_NAMESPACE::memory::move(other.data.value));
        }
    }

    constexpr $question &operator=($question &&other) noexcept {
        if (this != &other) {
            if (state == $State::Error) {
                delete_error();
            } else if (state == $State::Value) {
                delete_value();
            }

            state = other.state;
            if (state == $State::Error) {
                set_err(H_STD_NAMESPACE::memory::move(other.data.error));
            } else if (state == $State::Value) {
                set_value(H_STD_NAMESPACE::memory::move(other.data.value));
            }
        }
        return *this;
    }

    /// ------------------------------- Copy Constructor & Assignment
    /// -------------------------------
    constexpr $question(const $question &other)
        : state(other.state) {
        if (state == $State::Error) {
            set_err(other.data.error);
        } else if (state == $State::Value) {
            set_value(other.data.value);
        }
    }

    constexpr $question &operator=(const $question &other) {
        if (this != &other) {
            if (state == $State::Error) {
                delete_error();
            } else if (state == $State::Value) {
                delete_value();
            }

            state = other.state;
            if (state == $State::Error) {
                set_err(other.data.error);
            } else if (state == $State::Value) {
                set_value(other.data.value);
            }
        }
        return *this;
    }

    /// ------------------------------- Destructor -------------------------------
    constexpr ~$question() {
        if (state == $State::Error) {
            delete_error();
        } else if (state == $State::Value) {
            if constexpr (LIBCXX_NAMESPACE::is_destructible_v<T>) {
                delete_value();
            }
        }
    }

    /// ------------------------------- Operators -------------------------------
    constexpr bool operator==(const null_t &) const noexcept { return is_null(); }
    constexpr bool operator!=(const null_t &) const noexcept { return !is_null(); }

    template <typename E>
    constexpr bool operator==(const E &) const noexcept {
        if constexpr (H_STD_NAMESPACE::Panic::Panicking<E>) {
            return is_err(&typeid(E));
        }

#if defined(_MSC_VER)
        if constexpr (false) {
#endif

            _HX_MC_Q7_INTERNAL_CRASH_PANIC_M(H_STD_NAMESPACE::errors::TypeMismatchError(
                "Invalid: error type does not match any panic state."));

#if defined(_MSC_VER)
        }
#endif
    }

    template <typename E>
    constexpr bool operator!=(const E &other) const noexcept {
        return !(*this == other);
    }

    template <typename E>
    constexpr bool operator$contains(const E &other) const noexcept {
        return *this == other;
    }

    [[nodiscard]] constexpr bool operator$question() const noexcept {
        return state == $State::Value;
    }

    /// ------------------------------- Casting -------------------------------
    template <typename E>
        requires H_STD_NAMESPACE::Panic::Panicking<E>
    constexpr E operator$cast(E * /*unused*/) const {
        if (state == $State::Error) {
            if (is_err(&typeid(E))) {
                auto *obj = data.error.get_context().object();
                if (obj) {
                    return *reinterpret_cast<E *>(obj);
                }
                _HX_MC_Q7_INTERNAL_CRASH_PANIC_M(H_STD_NAMESPACE::errors::NullValueError(
                    "Invalid Decay: error context object is null."));
            }
            data.error.operator$panic();
        }

        if (state == $State::Value) {
            if constexpr (H_STD_NAMESPACE::meta::same_as<T, E>) {
                return data.value;
            }

            _HX_MC_Q7_INTERNAL_CRASH_PANIC_M(H_STD_NAMESPACE::errors::TypeMismatchError(
                "Invalid cast: value type does not match requested type."));
        }

        _HX_MC_Q7_INTERNAL_CRASH_PANIC_M(
            H_STD_NAMESPACE::errors::NullValueError("Invalid Decay: value is null."));
    }

    constexpr T operator$cast(T * /*unused*/) const {
        if (state == $State::Value) {
            return data.value;
        }

        if (state == $State::Error) {
            data.error.operator$panic();
        }

        _HX_MC_Q7_INTERNAL_CRASH_PANIC_M(
            H_STD_NAMESPACE::errors::NullValueError("Invalid Decay: value is null."));
    }

    [[nodiscard]] constexpr T &operator*() { return operator$cast(static_cast<T *>(nullptr)); }
    [[nodiscard]] constexpr    operator T() {
        return operator$cast(static_cast<T *>(nullptr));
    }  // NOLINT
};

H_STD_NAMESPACE_BEGIN

/// \typedef Questionable
///
/// A type alias for the `$question` class, providing a more concise and readable syntax for
/// working with quantum types (`?`). This alias also allows developers to use the `Questionable`
/// type in either C++ or Helix code. exposing the `Questionable` type for interoperability.
template <typename T>
using Questionable = $question<T>;

H_STD_NAMESPACE_END
H_NAMESPACE_END
#endif