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
///  most of the implementations defined in this file can be converted to helix itself and will  ///
///  be done so as soon as the core language is stable enough to support these features.         ///
///                                                                                              ///
///----------------------------------------------------------------------------------------------///

#ifndef __$LIBHELIX_PANIC__
#define __$LIBHELIX_PANIC__

#include "../config.h"
#include "../lang/function.hh"
#include "../primitives.h"
#include "../memory.h"
#include "../meta.h"
#include "../types.h"
#include "../types/erase.h"
#include "../types/errors.h"
#include "../libc.h"

/// \def _HX_MC_Q7_INTERNAL_CRASH_PANIC_M
///
/// A macro for triggering an immediate and unrecoverable panic within the Helix runtime.
///
/// ### Purpose
/// This macro is designed to:
/// - Immediately construct a `Panic::Frame` object with the provided error, the current file, and
/// the line number.
/// - Directly invoke the panic operator (`operator$panic`) on the `Frame` object, propagating the
/// error and halting further execution.
///
/// ### Behavior
/// - Constructs a `Panic::Frame` using the provided error and contextual information (`__FILE__`,
/// `__LINE__`).
/// - Calls `operator$panic` on the constructed frame, invoking the Helix panic handler and throwing
/// the managed error object.
///
/// ### Example Usage
/// ```cpp
/// _HX_MC_Q7_INTERNAL_CRASH_PANIC_M(H_STD_NAMESPACE::errors::RuntimeError("Critical error occurred."));
/// ```
/// This results in an immediate panic, halting execution and propagating the error.
///
/// ### Notes
/// - The macro does not return to the caller. It causes the program to terminate or transfer
/// control to a panic handler.
/// - Should be used in critical failure paths where recovery is not possible.
#ifndef _HX_MC_Q7_INTERNAL_CRASH_PANIC_M
#define _HX_MC_Q7_INTERNAL_CRASH_PANIC_M(err) ::helix::std::Panic::Frame(err, __FILE__, __LINE__).operator$panic();
#endif

/// \def $panic
///
/// A macro for returning a `Panic::Frame` object from a function, encapsulating an error with
/// contextual details.
///
/// ### Purpose
/// This macro simplifies the creation and return of a `Panic::Frame` object, embedding the provided
/// error along with the current file and line number. It integrates seamlessly into Helix's
/// panic-handling mechanism.
///
/// ### Behavior
/// - Constructs a `Panic::Frame` object using the provided error and contextual information
/// (`__FILE__`, `__LINE__`).
/// - Returns the constructed `Frame` object, enabling the caller to handle or propagate the error.
///
/// ### Example Usage
/// ```cpp
/// $panic(H_STD_NAMESPACE::errors::RuntimeError("Recoverable error occurred."));
/// ```
/// This creates a `Panic::Frame` with the given error and contextual details and returns it to the
/// caller.
///
/// ### Notes
/// - This macro is intended for use in functions where errors can be propagated or handled further
/// up the call stack.
/// - It does not invoke the panic operator directly, allowing the caller to determine the next
/// course of action.
#ifndef _HX_MC_Q7_PANIC_M
#define _HX_MC_Q7_PANIC_M(err) return ::helix::std::Panic::Frame(err, __FILE__, __LINE__);
#endif

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Panic {
class Frame;
}

H_STD_NAMESPACE_END

/// \fn _HX_FN_Vi_Q5_13_helixpanic_handler_Q3_5_5_stdPanicFrame_C_PK_Rv
///
/// \brief Handles panic events triggered during runtime.
///
/// \details The `_HX_FN_Vi_Q5_13_helixpanic_handler_Q3_5_5_stdPanicFrame_C_PK_Rv` function is
///          invoked when a panic is triggered. It performs necessary operations, such as invoking
///          registered panic hooks or logging the error context.
///
/// \param f A pointer to a `Panic::Frame` object that encapsulates the panic context.
/// \return This function does not return a value.
///
/// \note This function is a forward declaration and is typically invoked by Helix's internal panic
///       mechanisms. It is not intended for direct use by developers.
///
/// \see Panic::Frame
/// \see ../../panic_handler.hlx
///
/// \note The function name follows a specific naming convention to ensure consistency and
///       readability, the ABI scheme is as follows:
///           [_HX] - Helix prefix.
///           [_FN] - Function entity.
///           [_Vi] - Visibility internal.
///           [_Q5_13_helixpanic_handler]:
///               [_Q]  - Qualified.
///               [5_]  - Length of the namespace (`helix`).
///               [13_] - Length of the function name (`panic_handler`).
///           [_Q3_5_5_stdPanicFrame_C_PK]:
///               [_Q]  - Qualified parameter.
///               [3_]  - Length of the namespace (`std`).
///               [5_]  - Length of the nested namespace (`Panic`).
///               [5_]  - Length of the entity (`Frame`).
///               [_C]  - Const.
///               [_PK] - Pointer.
///           [_Rv]: Return prefix (`void`).
///
/// \example
/// \code{.hlx}
/// // This sets the panic handler function for Helix's panic system, this can be changed by the
/// //      user to provide custom panic handling.
/// // Add the following directive to the main file of your Helix program:
/// //     the passed parameter can be any function pointer that satisfies the signature:
/// //     fn (*H_STD_NAMESPACE::Panic::Frame) -> void
/// #[panic(&_HX_FN_Vi_Q5_13_helixpanic_handler_Q3_5_5_stdPanicFrame_C_PK_Rv)]
/// \endcode
extern "C" [[noreturn]]
void _HX_FN_Vi_Q5_13_helixpanic_handler_Q3_5_5_stdPanicFrame_C_PK_Rv(const H_STD_NAMESPACE::Panic::Frame * /* f */); // NOLINT(bugprone-reserved-identifier)

H_STD_NAMESPACE_BEGIN 

/// \namespace Panic
///
/// The `Panic` namespace provides mechanisms for handling panics in Helix.
/// A panic represents a critical error state that halts normal program execution and invokes
/// predefined error-handling routines.
///
/// ### Features
/// - `Panic::Frame`: Represents a context for managing and propagating panic errors.
/// - Concepts for identifying and validating panic-capable types (`Panicking`, `PanickingStatic`,
/// `PanickingInstance`).
/// - `_HX_FN_Vi_Q5_13_helixpanic_handler_Q3_5_5_stdPanicFrame_C_PK_Rv`: A function to manage panic events
/// triggered during runtime.
///
/// ### Overview
/// The `Panic` namespace is foundational to Helix's error-handling architecture. It ensures that
/// panic events are tracked, managed, and propagated with proper context and type safety.
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
/// `_HX_FN_Vi_Q5_13_helixpanic_handler_Q3_5_5_stdPanicFrame_C_PK_Rv`.
class Frame;

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
/// static_assert(Panic::Panicking<Error>);
/// ```
template <typename T>
concept Panicking = requires(T obj) {
    { obj.operator$panic() } -> H_STD_NAMESPACE::Meta::convertible_to<string>;
} || requires {
    { T::operator$panic() } -> H_STD_NAMESPACE::Meta::convertible_to<string>;
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
/// static_assert(Panic::PanickingStatic<Error>);
/// ```
template <typename T>
concept PanickingStatic = requires(T obj) {
    { T::operator$panic() } -> H_STD_NAMESPACE::Meta::convertible_to<string>;
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
/// static_assert(Panic::PanickingInstance<Error>);
/// ```
template <typename T>
concept PanickingInstance = requires(T obj) {
    { obj.operator$panic() } -> H_STD_NAMESPACE::Meta::convertible_to<string>;
};
}  // namespace Panic

namespace Panic {
/// \class FrameContext
///
/// `FrameContext` is a core component of the Helix language's panic handling system.
/// It provides dynamic management for objects, integrating type-erasure techniques
/// with panic handling and exception propagation.
///
/// ### Design Details
/// - **Dynamic Management**: Stores objects of arbitrary types using the `TypeErasure`
///   interface, allowing for flexible and type-safe storage.
/// - **Panic Integration**: Supports Helix's panic system by associating objects
///   with panic messages and enabling their propagation as exceptions.
/// - **Safe Cleanup**: Ensures all managed objects are properly destroyed when
///   the `FrameContext` is destructed or reassigned.
///
/// ### Responsibilities
/// - Manage the lifecycle of dynamically allocated objects.
/// - Facilitate panic handling and exception throwing for managed objects.
/// - Provide interfaces for cloning, comparison, and access to the stored object.
///
/// ### Usage in Helix
/// `FrameContext` is typically used within Helix's panic handling system. When a
/// panic occurs, `FrameContext` manages the offending object, ensuring it is
/// correctly propagated and destroyed.
///
/// ### Key Features
/// - **Type Erasure**: Uses `TypeErasure` to manage objects of arbitrary types.
/// - **Panic Propagation**: Supports throwing the managed object as an exception
///   using the Helix panic mechanism.
/// - **Lifecycle Management**: Ensures proper cleanup of objects, preventing memory
///   leaks and undefined behavior.
///
/// ### Implementation Notes
/// - The constructor accepts any object type that satisfies the constraints
///   defined by Helix's `Panicking` concept.
/// - The `crash()` method propagates the managed object as an exception, terminating
///   the current context.
/// - Copy and move constructors ensure the `FrameContext` can be safely duplicated
///   or transferred.
///
/// ### Example
/// \code{.cpp}
/// // Creating a FrameContext with a custom object
/// MyError *error = new MyError();
/// FrameContext context(error);
///
/// // Accessing the managed object
/// void *obj = context.object();
///
/// // Throwing the managed object as an exception
/// context.crash();
/// \endcode
class FrameContext {
  private:
    H_STD_NAMESPACE::TypeErasure                   *$object;
    $function<void(H_STD_NAMESPACE::TypeErasure *)> $throw;

    template <typename T>
    [[noreturn]] constexpr static void throw_object(H_STD_NAMESPACE::TypeErasure *$object) {
        if ($object == nullptr) {
            throw errors::RuntimeError("Tried to crash with a null object.");
        }

        if ($object->type_info() != &typeid(T)) {
            throw errors::TypeMismatchError("Type mismatch when crashing.");
        }

        T *object = static_cast<T *>(**$object);
        throw *object;
    }

  public:
    constexpr ~FrameContext() {
        if ($object != nullptr) {
            $object->destroy();
            delete $object;
            $object = nullptr;
        }
    }

    constexpr FrameContext()
        : $object(nullptr) {}

    constexpr FrameContext(const FrameContext &other)
        : $object((other.$object != nullptr) ? other.$object->clone() : nullptr)
        , $throw(other.$throw) {}

    constexpr FrameContext &operator=(const FrameContext &other) {
        if (this != &other) {
            if ($object != nullptr) {
                $object->destroy();
                delete $object;
            }
            $object = (other.$object != nullptr) ? other.$object->clone() : nullptr;
            $throw  = other.$throw;
        }
        return *this;
    }

    constexpr FrameContext(FrameContext &&other) noexcept
        : $object(other.$object)
        , $throw(H_STD_NAMESPACE::Memory::move(other.$throw)) {
        other.$object = nullptr;
    }

    constexpr FrameContext &operator=(FrameContext &&other) noexcept {
        if (this != &other) {
            if ($object != nullptr) {
                $object->destroy();
                delete $object;
            }
            $object       = other.$object;
            $throw        = H_STD_NAMESPACE::Memory::move(other.$throw);
            other.$object = nullptr;
        }
        return *this;
    }

    template <typename T>
    constexpr explicit FrameContext(T *obj)
        : $object(H_STD_NAMESPACE::erase_type(obj)) {
        if constexpr (!Panic::Panicking<T>) {
            static_assert(Panic::Panicking<T>,
                          "Frame invoked with an object that does not have a panic method, add "
                          "`class ... with Panic::Panicking` "
                          "to the definition, and implement 'op panic fn(self) -> string' or the "
                          "static variant.");
        }

        $throw = &FrameContext::throw_object<T>;
    }

    [[nodiscard]] constexpr void *object() const { return ($object != nullptr) ? **$object : nullptr; }

    [[noreturn]] void crash() {
        $throw($object);
        throw errors::RuntimeError("Object '" + string($object->type_info()->name()) +
                                   "' failed to panic.");
    }

    constexpr bool operator!=(const LIBCXX_NAMESPACE::type_info *rhs) const { return !(*this == rhs); }
    constexpr bool operator==(const LIBCXX_NAMESPACE::type_info *rhs) const {
        if (this->$object != nullptr) {
            return this->$object->type_info() == rhs;
        }

        return false;
    }

    [[nodiscard]] constexpr string type_name() const {
        if ($object != nullptr) {
        #if defined(_MSC_VER)
            return $object->type_info()->name();
        #else
            const char* mangled_name = $object->type_info()->name();

            if ((mangled_name == nullptr) || (*mangled_name == 0)) {
                return {};
            }

            int status = 0;

            // __cxa_demangle returns a malloc-allocated string
            char* demangle = LIBC_NAMESPACE::abi::__cxa_demangle(mangled_name, nullptr, nullptr, &status);
            if (status == 0 && (demangle != nullptr)) {
                string result(demangle);
            
                free(demangle); // NOLINT
                return result;
            }
            
            return mangled_name;
        #endif
        }

        return "null";
    }
};

/// \class Frame
///
/// `Frame` is a core component of Helix's panic-handling system, encapsulating the details
/// of an error state (panic). It is designed to store critical information about the panic,
/// such as the reason, file, and line number, and propagate the error through the system.
///
/// ### Design Details
/// - **Immediate Panic**: The `Frame` constructor is designed to immediately invoke a panic
///   on the provided object. This ensures that invalid states are caught and handled as early
///   as possible.
/// - **Static and Instance Support**: The `initialize` function determines whether the
///   provided type supports static or instance-level panic methods and invokes them
///   accordingly.
/// - **Type Constraints**: The `Frame` class enforces strong constraints on the provided type.
///   It ensures that the type is either copy or move constructible and implements the Helix
///   panic interface (`Panicking` concept).
///
/// ### Responsibilities
/// - Store details about the panic, including:
///   - The reason for the panic.
///   - The file and line number where the panic occurred.
/// - Manage the lifetime of the panic object using `FrameContext`.
/// - Provide methods to retrieve panic details.
/// - Propagate the panic by invoking a panic handler and throwing the managed object.
///
/// ### Constructor Use
/// This class is typically constructed using the `panic` keyword in Helix:
/// ```helix
/// panic H_STD_NAMESPACE::Panic::Frame(H_STD_NAMESPACE::errors::RuntimeError(), "yes.hlx", 12);
/// ```
/// The `Frame` class can also be extracted into a variable and then passed to the `panic`
/// system. These are the only valid ways to create and utilize `Frame` objects.
///
/// ### Panic Propagation
/// When a `Frame` object is panicked:
/// - The internal panic handler is invoked.
/// - The associated object is thrown as an exception.
///
/// ### Key Features
/// - **Static Enforcement**: Ensures only types satisfying the `Panicking` concept are accepted.
/// - **Dynamic Propagation**: Integrates tightly with `FrameContext` to manage and throw objects
///   as part of the panic process.
/// - **Contextual Information**: Provides file, line, and reason details for debugging purposes.
///
/// ### Example Usage
/// ```cpp
/// // Triggering a panic with a custom error
/// Frame frame(H_STD_NAMESPACE::errors::RuntimeError("Some error occurred"), "example.hlx", 42);
/// frame.operator$panic();
/// ```
///
/// ### Notes
/// - The default constructor is explicitly deleted to prevent creating invalid `Frame` instances.
/// - Copy operations are also deleted to maintain unique ownership of the panic context.
/// - Move operations are supported, allowing transfer of ownership.
///
/// ### Interactions with FrameContext
/// The `Frame` class heavily relies on `FrameContext` to manage the lifecycle of the panic object.
/// This ensures proper cleanup and exception safety.
///
/// ### Implementation Details
/// - **`initialize<T>`**:
///   - Determines whether the type supports static (`T::operator$panic`) or instance-level
///     (`obj.operator$panic`) panic methods.
///   - Constructs the panic object dynamically and wraps it in a `FrameContext`.
/// - **`operator$panic`**:
///   - Invokes the internal panic handler.
///   - Calls `FrameContext::crash()` to throw the managed object.
///
/// ### API Summary
/// - Constructors:
///   - `Frame(T obj, const char *filename, usize lineno)`
///   - `Frame(T obj, string filename, usize lineno)`
///   - Deleted: default and copy constructors.
///   - Defaulted: move constructors.
/// - Accessors:
///   - `file()`: Returns the file name associated with the panic.
///   - `line()`: Returns the line number associated with the panic.
///   - `reason()`: Returns the panic reason.
///   - `get_context()`: Returns the underlying `FrameContext`.
/// - Methods:
///   - `operator$panic()`: Propagates the panic.
class Frame {
  private:
    mutable FrameContext context;
    string               _reason;
    string               _file;
    usize                _line = 0;

    template <typename T>
        requires __is_class
    (T) constexpr inline void initialize(const T &obj) {
        if constexpr (PanickingStatic<T>) {
            _reason = T::operator$panic();
        } else if constexpr (PanickingInstance<T>) {
            _reason = obj.operator$panic();
        } else {
            static_assert(Panic::Panicking<T>,
                          "Frame invoked with a type that lacks a panic operator. Ensure the type "
                          "declares `class ... with Panic::Panicking` and implements either "
                          "`op panic fn(self) -> string` or the static equivalent.");
        }

        T *object = nullptr;

        static_assert(LIBCXX_NAMESPACE::is_copy_constructible_v<T> ||
                          LIBCXX_NAMESPACE::is_move_constructible_v<T>,
                      "Frame invoked with a type that is not copy or move constructible.");

        try {
            if constexpr (LIBCXX_NAMESPACE::is_move_constructible_v<T>) {
                object        = new T(H_STD_NAMESPACE::Memory::move(obj));  // NOLINT
                this->context = FrameContext(object);
            } else if constexpr (LIBCXX_NAMESPACE::is_copy_constructible_v<T>) {
                object        = new T(obj);  // NOLINT
                this->context = FrameContext(object);
            }
        } catch (...) {
            delete object;  // NOLINT
            throw errors::RuntimeError("Failed to initialize panic frame.");
        }
    }

  public:
    template <typename T>
    constexpr Frame(T obj, const char *filename, usize lineno)
        : _file(filename)
        , _line(lineno) {
        initialize<T>(obj);
    }

    template <typename T>
    constexpr Frame(T obj, string filename, usize lineno)
        : _file(H_STD_NAMESPACE::Memory::move(filename))
        , _line(lineno) {
        initialize<T>(obj);
    }

    constexpr Frame()  = delete;
    constexpr ~Frame() = default;

    constexpr Frame(const Frame &)            = delete;
    constexpr Frame &operator=(const Frame &) = delete;

    Frame(Frame &&other) noexcept            = default;
    Frame &operator=(Frame &&other) noexcept = default;

    [[noreturn]] Frame(Frame  &obj, const string &, usize) { obj.operator$panic(); }
    [[noreturn]] Frame(Frame &&obj, const string &, usize) { obj.operator$panic(); }

    [[nodiscard]] constexpr string        file()        const { return this->_file; }
    [[nodiscard]] constexpr usize         line()        const { return this->_line; }
    [[nodiscard]] constexpr string        reason()      const { return this->_reason; }
    [[nodiscard]] constexpr FrameContext &get_context() const { return context; }

    [[noreturn]] void operator$panic() const { _HX_FN_Vi_Q5_13_helixpanic_handler_Q3_5_5_stdPanicFrame_C_PK_Rv(this); }
};
}  // namespace Panic

H_STD_NAMESPACE_END
H_NAMESPACE_END
#endif