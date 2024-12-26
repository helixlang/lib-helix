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
///  fully implemented, the $question class is a class that is similar to the std::optional but  ///
///  also adds std::expected like functionality, the $question class is a class that can hold    ///
///  one of 3 states: value, an error, or be null.                                               ///
///                                                                                              ///
///  the $question class is a class that is to be used inside the core since its required by the ///
///  compiler to allow for `?` type syntax.                                                      ///
///                                                                                              ///
///----------------------------------------------------------------------------------------------///

#ifndef __$LIBHELIX_QUESTION__
#define __$LIBHELIX_QUESTION__

#include "../config.h"
#include "../types.h"
#include "../lang/panic.hh"
#include "../primitives.h"
#include "../print.h"
#include "../refs.h"
#include "../traits.h"
#include "../types/errors.h"

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

// FIXME: $internal_panic_handler should be a fuction pointer not a function...
void $internal_panic_handler(const Panic::Frame *f) {  // TODO: move to helix itself
    print("exit: ", (*f).file(), ":", (*f).line(), " - ", (*f).reason());
};

/// \class $question
///
/// Represents Helix's `?` type, also known as the **quantum type**. This type can hold either:
/// - A valid value of type `T`,
/// - A null state, or
/// - An error represented by a `Panic::Frame`.
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
/// - **Error**: Encapsulates an error using a `Panic::Frame`.
///
/// ### Key Features
/// - **Type Safety**: Enforces constraints such as copy and move constructibility for valid types.
/// - **Error Containment**: Encodes errors using `Panic::Frame`, enabling structured error
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
/// - **Error Initialization**: `T? x = Panic::Frame(...);` stores an error state.
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
/// 2. Errors are always stored as `Panic::Frame` objects.
/// 3. Errors and values are managed with proper resource allocation and destruction to avoid leaks
///    or undefined behavior.
///
/// ### Related
/// - `Panic::Frame`: Represents the error state.
/// - `Panic::Panicking`: Concept for types supporting the `operator$panic()` function.
/// - `Panic::PanickingInstance`: Concept for instances supporting `operator$panic()`.
/// - `Panic::PanickingStatic`: Concept for types supporting static `operator$panic()`.
///
/// ### Example Usage
/// ```helix
/// fn process_data() -> int? {
///     panic std::errors::RuntimeError("Data processing failed.");
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
    union $StorageT {  // NOLINT(cppcoreguidelines-special-member-functions)
        mutable Panic::Frame error;
        mutable T            value;

        $StorageT()
            : value() {}
        ~$StorageT() {}
    };

    mutable $State state = $State::Null;
    $StorageT      data;

    [[nodiscard]] bool is_null() const { return this->state == $State::Null; }
    [[nodiscard]] bool is_err() const { return this->state == $State::Error; }
    [[nodiscard]] bool is_err(const LIBCXX_NAMESPACE::type_info *type) const {
        if (this->state != $State::Error) {
            return false;
        }

        return data.error.get_context() == type;
    }

    void set_value(const T &value) { new (&this->data.value) T(value); }
    void set_value(T &&value) { new (&this->data.value) T(std::ref::move(value)); }
    void set_err(Panic::Frame &&error) {
        new (&this->data.error) Panic::Frame(std::ref::move(error));
    }

    void delete_error() { this->data.error.~Frame(); }
    void delete_value() { this->data.value.~T(); }

  public:
    // ----------------------------------- constructors (null) -----------------------------------
    $question()
        : state($State::Null) {}
    explicit $question(const null_t & /*unused*/)
        : state($State::Null) {};
    explicit $question(const null_t && /*unused*/)
        : state($State::Null) {}
    explicit $question(null_t && /*unused*/)
        : state($State::Null) {}

    /// ----------------------------------- constructors (value) -----------------------------------
    $question(T &&value)  // NOLINT(google-explicit-constructor)
        : state($State::Value) {
        set_value(std::ref::move(value));
    }

    $question(const T &&value)  // NOLINT(google-explicit-constructor)
        : state($State::Value) {
        set_value(std::ref::move(value));
    }

    $question(const T &value)  // NOLINT(google-explicit-constructor)
        : state($State::Value) {
        set_value(value);
    }

    /// ----------------------------------- constructors (error) -----------------------------------
    $question(Panic::Frame &&error)  // NOLINT(google-explicit-constructor)
        : state($State::Error) {
        set_err(std::ref::move(error));
    }

    $question(const Panic::Frame &&error)  // NOLINT(google-explicit-constructor)
        : state($State::Error) {
        set_err(std::ref::move(error));
    }

    $question(const Panic::Frame &error)  // NOLINT(google-explicit-constructor)
        : state($State::Error) {
        set_err(error);
    }

    /// ------------------------------------- move constructors ------------------------------------
    $question($question &&other) noexcept {
        if (other.state == $State::Error) {
            set_err(std::ref::move(other));
        } else if (other.state == $State::Value) {
            set_value(std::ref::move(other));
        }
    }
    $question &operator=($question &&other) noexcept {
        if (other.state == $State::Error) {
            set_err(std::ref::move(other));
            other.delete_error();
        } else if (other.state == $State::Value) {
            set_value(std::ref::move(other));
            other.~$question();
        }

        return *this;
    }

    /// ------------------------------------- copy constructors ------------------------------------
    $question(const $question &other) {
        if (other.state == $State::Error) {
            set_err(other);
        } else if (other.state == $State::Value) {
            set_value(other);
        }
    }
    $question &operator=(const $question &other) {
        if (other.state == $State::Error) {
            set_err(other);
        } else if (other.state == $State::Value) {
            set_value(other);
        }

        return *this;
    }

    /// ---------------------------------------- destructor ----------------------------------------
    ~$question() {
        if (this->state == $State::Error) {
            delete_error();
        } else if (this->state == $State::Value) {
            // call the destructor (if the type has a destructor)
            if constexpr (LIBCXX_NAMESPACE::is_destructible_v<T>) {
                delete_value();
            }
        }
    }

    /// ------------------------------------- equality (null) --------------------------------------
    constexpr bool operator==(const null_t & /*unused*/) const { return is_null(); }
    constexpr bool operator!=(const null_t & /*unused*/) const { return !is_null(); }

    // equality operators (error)
    // ... == T,x
    // this checks against the error type not an error instance (the eq op would check with E's eq
    // op if it has one to make sure only exact errors are caught) this allows for diffent errors to
    // be caught and handled differently (like, SomeCustomError(ThisIsAEnumValue) ==
    // SomeCustomError(AnotherEnumValue) would be false)
    // -------------------------------------- equality (error) -------------------------------------
    template <typename E>
    constexpr bool operator==(const E & /*unused*/) const {
        if constexpr (Panic::Panicking<E>) {
            return is_err(&typeid(E));
        }

        $CrashWithPanic(
            errors::TypeMismatchError("Invalid: error type does not match any panic state."));
    }

    template <typename E>
    constexpr bool operator==(E &&_) const {
        return *this == static_cast<const E &>(_);
    }

    template <typename E>
    constexpr bool operator!=(const E &_) const {
        return !(*this == _);
    }

    template <typename E>
    constexpr bool operator!=(E &&_) const {
        return !(*this == static_cast<const E &>(_));
    }

    // --------------------------------------- 'has' (null) ---------------------------------------
    constexpr bool operator$contains(const null_t &_) const { return is_null(); }

    // --------------------------------------- 'has' (error) ---------------------------------------
    template <typename E>
    constexpr bool operator$contains(const E &_) const {
        return *this == _;
    }

    template <typename E>
    constexpr bool operator$contains(E &&_) const {
        return *this == static_cast<const E &>(_);
    }

    // --------------------------------------- 'has' (value) ---------------------------------------
    constexpr bool operator$contains(const T &_) const { return this->state == $State::Value; }

    // ---------------------------------------- '?' operator ---------------------------------------
    [[nodiscard]] bool operator$question() const { return this->state == $State::Value; }

    // -------------------------------------------- 'as' -------------------------------------------
    template <typename E>
        requires Panic::Panicking<E>
    E operator$cast(E * /*unused*/) const {
        if (this->state == $State::Value) {
            if constexpr (traits::same_as_v<T, E>) {
                return this->data.value;
            }

            $CrashWithPanic(errors::TypeMismatchError("Invalid cast: value type does not"
                                                      "match requested type."));
        }

        if (this->state == $State::Error) {
            if (is_err(&typeid(E))) {
                auto *obj = this->data.error.get_context().object();

                if (obj != nullptr) {
                    return *reinterpret_cast<E *>(obj);  // NOLINT
                }

                $CrashWithPanic(
                    errors::NullValueError("Invalid Decay: error context object is null."));
            }

            /* no return */ this->data.error.operator$panic();
        }

        $CrashWithPanic(errors::NullValueError("Invalid Decay: value is null."));
    }

    T operator$cast(T * /*unused*/) const {
        if (this->state == $State::Value) {
            return this->data.value;
        }

        if (this->state == $State::Error) {
            /* no return */ this->data.error.operator$panic();
        }

        $CrashWithPanic(errors::NullValueError("Invalid Decay: value is null."));
    }

    [[nodiscard]] T &operator*() { return operator$cast(static_cast<T *>(nullptr)); }
    [[nodiscard]]   operator T() { return operator$cast(static_cast<T *>(nullptr)); } // NOLINT
    [[nodiscard]] explicit operator T &() { return operator*(); }
    [[nodiscard]] T       *operator->() { return &this->operator*(); }
};

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