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

#include <any>
#include "../config.h"
#include "../dtypes.h"
#include "../libcxx.h"
#include "../primitives.h"
#include "../print.h"
#include "../refs.h"
#include "../traits.h"
#include "function.h"

#define $PANIC(err) panic_handler(Panic::Frame(new err, __FILE__, __LINE__));

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace errors {
class Error /* with Panicking */ {
  public:
    Error() = default;

    Error(const Error &other)     = default;
    Error(Error &&other) noexcept = default;

    Error &operator=(const Error &other)     = default;
    Error &operator=(Error &&other) noexcept = default;

    virtual ~Error() = default;

    [[nodiscard]] virtual string $panic() const { return "An error occurred."; }

    [[nodiscard]] virtual string $cast(string * /*unused*/) const = 0;  // string cast
};

class NullAccessError : public Error {
  public:
    NullAccessError() = default;

    NullAccessError(const string &msg)
        : msg(msg) {}

    NullAccessError(const NullAccessError &other)     = default;
    NullAccessError(NullAccessError &&other) noexcept = default;

    NullAccessError &operator=(const NullAccessError &other)     = default;
    NullAccessError &operator=(NullAccessError &&other) noexcept = default;

    ~NullAccessError() override = default;

    [[nodiscard]] string $panic() const override { return msg; }

    [[nodiscard]] string $cast(string * /*unused*/) const override { return msg; }

  private:
    string msg;
};

class RuntimeError : public Error {
  public:
    RuntimeError() = default;

    RuntimeError(const string &msg)
        : msg(msg) {}

    RuntimeError(const RuntimeError &other)     = default;
    RuntimeError(RuntimeError &&other) noexcept = default;

    RuntimeError &operator=(const RuntimeError &other)     = default;
    RuntimeError &operator=(RuntimeError &&other) noexcept = default;

    ~RuntimeError() override = default;

    [[nodiscard]] string $panic() const override { return msg; }

    [[nodiscard]] string $cast(string * /*unused*/) const override { return msg; }

  private:
    string msg;
};
}  // namespace errors

/*
 PanicFrame(
        "this is a panic",
        "source/helix.hlx",
        1
    );
 */

namespace Panic {
template <typename T>
class Frame;

template <typename T>
concept Panicking = requires(T obj) {
    { obj.$panic() } -> std::traits::convertible_to<string>;
} || requires {
    { T::$panic() } -> std::traits::convertible_to<string>;
};
}  // namespace Panic

template <typename T>
[[noreturn]] void panic_handler(Panic::Frame<T> f) {
    if constexpr (requires { f.$panic(); }) {
        print("exit: ", f.file, ":", f.line, " - ", f.$panic());
    } else if constexpr (requires { T::$panic(); }) {
        print("exit: ", f.file, ":", f.line, " - ", T::$panic());
    } else {
        static_assert(Panic::Panicking<T>,
                      "object does not have a panic method, add `class ... with Panic::Panicking` "
                      "to the definition, and implement either ('op panic fn(self) -> string' or "
                      "'static op panic fn() -> string')");
    }

    throw f.$cast(static_cast<T*>(nullptr));
}

// class Error with Panicking - This can be panicked
namespace Panic {
template <typename T>
class Frame {
  public:
    Frame() = delete;

    Frame(T *object, string file, usize line)
        requires Panicking<T>
        : object(static_cast<void *>(object))
        , file(std::ref::move(file))
        , line(line) {
        if constexpr (!Panicking<T>) {
            static_assert(
                Panicking<T>,
                "object does not have a panic method, add `class ... with Panic::Panicking` "
                "to the definition, and implement either ('op panic fn(self) -> string' or "
                "'static op panic fn() -> string')");
        } else {
            if (object == nullptr) {
                $PANIC(errors::NullAccessError("null access"));
            }
        }
    }

    ~Frame() {
        if (object != nullptr) {
            delete static_cast<T *>(object);
        }
    }

    void panic() const {
        if (object == nullptr) {
            $PANIC(errors::NullAccessError("Frame object not found"));
        }

        panic_handler(*this);
    }

    /// cast op to allow for the object to be casted to the type
    T &$cast(T * /*unused*/) const {
        if (object == nullptr) {
            $PANIC(errors::NullAccessError("null access"));
        }

        return *static_cast<T *>(object);
    }

    void  *object;  // strip the type
    string file;
    usize  line{};

    using error_type = T;
};
}  // namespace Panic

class null_t {};
inline constexpr null_t null;

// template <class T, typename... Es>
// class $question;

// template <class T, typename... Es>
//     requires std::traits::is_class_v<T> && (std::traits::is_class_v<Es> && ...)
// class $question<T, Es...> : public T {
//   public:
//     /// this is for classes
// };

// template <class T, typename... Es>
//     requires(std::traits::is_class_v<Es> && ...)
// class $question<T, Es...> {
//   public:
//     enum class State : char { Value, Null, Error };

//     // all Et must impolment Panicking
//     template <typename... Et> requires(Panic::Panicking<Et> && ...)
//     union LikelyError {
//         LikelyError() {}
//         ~LikelyError() {}
//         LikelyError(const LikelyError &other)                = default;
//         LikelyError &operator=(const LikelyError &other)     = default;
//         LikelyError(LikelyError &&other) noexcept            = default;
//         LikelyError &operator=(LikelyError &&other) noexcept = default;

//         alignas(Et...) array<byte, libcxx::max({sizeof(Et)...})> storage{};
//     };

//     // either a panicFrame or a directly pass the error

//     using ErrorT = libcxx::conditional_t<sizeof...(Es) == 0, LikelyError<typename T::error_type>,
//     LikelyError<Es...>>;

//     /// \brief defines the '?' operator to see if the value is null
//     [[nodiscard]] constexpr bool $question() const { return state == State::Value; }
//     constexpr T                 &operator*() {
//         switch (state) {
//             case State::Value:
//                 return value;
//             case State::Null:
//                 throw NullAccessError("null access");
//             case State::Error:
//                 throwStoredError();
//         }

//         throw NullAccessError("memory corruption");
//     }

//     constexpr operator T &() {  // NOLINT(google-explicit-constructor)
//         return operator*();
//     }

//     // Const version for read-only contexts
//     constexpr operator const T &() const {  // NOLINT(google-explicit-constructor)
//         return operator*();
//     }

//     constexpr bool operator==(const null_t & /*unused*/) const { return state == State::Null; }
//     constexpr bool operator!=(const null_t & /*unused*/) const { return state != State::Null; }

//     constexpr T &$cast(T * /*unused*/) { return operator*(); }

//     template <typename E,
//               typename = libcxx::enable_if_t<(std::traits::is_same_v<E, Error> ||
//                                               (std::traits::is_same_v<E, Es> || ...))>>
//     constexpr bool $contains(const E & /* unused */)  // NOLINT(google-explicit-constructor)
//     {
//         return ((state == State::Error) && error_type_index == type_index<E>());
//     }
//     template <typename E,
//               typename = libcxx::enable_if_t<(std::traits::is_same_v<E, Error> ||
//                                               (std::traits::is_same_v<E, Es> || ...))>>
//     constexpr bool $contains(E && /* unused */)  // NOLINT(google-explicit-constructor)
//     {
//         return ((state == State::Error) && error_type_index == type_index<E>());
//     }

//     constexpr $question()
//         : state(State::Null) {}

//     constexpr $question(const T &value)  // NOLINT(google-explicit-constructor)
//         : state(State::Value) {
//         this->value = value;
//     }

//     constexpr $question(T &&value)  // NOLINT(google-explicit-constructor)
//         : state(State::Value) {
//         this->value = std::ref::move(value);
//     }

//     template <typename E,
//               typename = libcxx::enable_if_t<(std::traits::is_same_v<E, Error> ||
//                                               (std::traits::is_same_v<E, Es> || ...))>>
//     constexpr $question(const E &err)  // NOLINT(google-explicit-constructor)
//         : state(State::Error) {
//         new (&error.storage) E(err);
//     }

//     template <typename E,
//               typename = libcxx::enable_if_t<(std::traits::is_same_v<E, Error> ||
//                                               (std::traits::is_same_v<E, Es> || ...))>>
//     constexpr $question(E &&err)  // NOLINT(google-explicit-constructor)
//         : state(State::Error)
//         , error_type_index(type_index<E>()) {
//         new (&error.storage) E(std::forward<E>(err));
//     }

//     constexpr ~$question() {
//         if (state == State::Error) {
//             destroy_error();
//         }
//     }

//     constexpr $question(const null_t & /*unused*/)  // NOLINT(google-explicit-constructor)
//         : state(State::Null) {}

//     constexpr $question(null_t && /*unused*/)  // NOLINT(google-explicit-constructor)
//         : state(State::Null) {}

//     constexpr $question(const $question &other) {
//         switch (other.state) {
//             case State::Value:
//                 value = other.value;
//                 break;
//             case State::Null:
//                 break;
//             case State::Error:
//                 error            = other.error;
//                 error_type_index = other.error_type_index;
//                 break;
//         }

//         state = other.state;
//     }

//     constexpr $question($question &&other) noexcept {
//         switch (other.state) {
//             case State::Value:
//                 value = std::ref::move(other.value);
//                 break;
//             case State::Null:
//                 break;
//             case State::Error:
//                 error            = std::ref::move(other.error);
//                 error_type_index = other.error_type_index;
//                 break;
//         }

//         state = other.state;
//     }

//     constexpr $question &operator=(const T &val) {
//         this->value = val;
//         state       = State::Value;
//         return *this;
//     }

//     constexpr $question &operator=(T &&val) {
//         this->value = std::ref::move(val);
//         state       = State::Value;
//         return *this;
//     }

//     constexpr $question &operator=(const null_t & /*unused*/) {
//         state = State::Null;
//         return *this;
//     }

//     constexpr $question &operator=(null_t && /*unused*/) {
//         state = State::Null;
//         return *this;
//     }

//     constexpr $question &operator=(const $question &other) {
//         switch (other.state) {
//             case State::Value:
//                 value = other.value;
//                 break;
//             case State::Null:
//                 break;
//             case State::Error:
//                 error            = other.error;
//                 error_type_index = other.error_type_index;
//                 break;
//         }

//         state = other.state;
//         return *this;
//     }

//     constexpr $question &operator=($question &&other) noexcept {
//         switch (other.state) {
//             case State::Value:
//                 value = std::ref::move(other.value);
//                 break;
//             case State::Null:
//                 break;
//             case State::Error:
//                 error            = std::ref::move(other.error);
//                 error_type_index = other.error_type_index;
//                 break;
//         }

//         state = other.state;
//         return *this;
//     }

//   private:
//     T      value;
//     ErrorT error;
//     State  state            = State::Null;
//     usize  error_type_index = 0;

//     constexpr void destroy_error() {
//         visit_error([&](auto *errPtr) {
//             if constexpr (requires { errPtr->~decltype (*errPtr)(); }) {
//                 errPtr->~decltype (*errPtr)();
//             }
//         });
//     }

//     // Helper function to throw the stored error
//     constexpr void throwStoredError() const {
//         visit_error([&](auto *errPtr) { throw *errPtr; });
//     }

//     template <typename E>
//     static constexpr usize type_index() {
//         return typeid(E).hash_code();
//     }

//     template <typename F>
//     constexpr void visit_error(F &&func) const {
//         if constexpr (sizeof...(Es) > 0) {
//             // For each type in Es..., check if it matches the stored type
//             (..., ([&] {
//                  if (error_type_index == type_index<Es>()) {
//                      func(reinterpret_cast /* NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
//                      */
//                           <const Es *>(&error.storage));
//                  }
//              }()));

//         } else {
//             func(reinterpret_cast /* NOLINT(cppcoreguidelines-pro-type-reinterpret-cast) */
//                  <const Error *>(&error.storage));
//         }
//     }
// };

template <class T>
class $question : public libcxx::conditional_t<libcxx::is_class_v<T>, T, null_t> {
  private:
    enum class $State : char { Value, Null, Error };
    union $Data_u {
        // error type, destructor, panic function, error buffer
        mutable tuple<const libcxx::type_info *,
                      void (*)(void *),
                      void (*)(void *),
                      char[sizeof(Panic::Frame<errors::Error>)]>
                  error;
        mutable T value;

        $Data_u()
            : value() {}
        ~$Data_u() {}
    };

    bool check_error_type(const libcxx::type_info &type) const {
        if (this->state != $State::Error) {
            return false;
        }

        return *::std::get<0>(this->data.error) == type;
    }

    bool check_null() const { return this->state == $State::Null; }

    template <typename E>
        requires Panic::Panicking<E>
    void set_error(const Panic::Frame<E> &&error) {
        using Frame_t = Panic::Frame<E>;

        // validate that the error type is large enough to hold the error
        static_assert(sizeof(::std::get<3>(this->data.error)) >= sizeof(Frame_t),
                      "invalid error type.");

        // set the error type
        ::std::get<0>(this->data.error) = &typeid(E);

        // set the fn pointer to the destructor
        ::std::get<1>(this->data.error) = [](void *ptr) {
            auto *error = static_cast<Frame_t *>(ptr);
            delete error;
        };

        // set the fn pointer to the panic function
        ::std::get<2>(this->data.error) = [](void *ptr) {
            auto *error = static_cast<Frame_t *>(ptr);
            error->panic();
        };

        // copy the error into the buffer
        new (::std::get<3>(this->data.error)) libcxx::any(Frame_t(std::ref::move(error)));
    }

    template <typename E>
        requires Panic::Panicking<E>
    void set_error(const Panic::Frame<E> &error) {
        using Frame_t = Panic::Frame<E>;

        // validate that the error type is large enough to hold the error
        static_assert(sizeof(::std::get<3>(this->data.error)) >= sizeof(Frame_t),
                      "invalid error type.");

        // set the error type
        ::std::get<0>(this->data.error) = &typeid(E);

        // set the fn pointer to the destructor
        ::std::get<1>(this->data.error) = [](void *ptr) {
            auto *error = static_cast<Frame_t *>(ptr);
            delete error;
        };

        // set the fn pointer to the panic function
        ::std::get<2>(this->data.error) = [](void *ptr) {
            auto *error = static_cast<Frame_t *>(ptr);
            error->panic();
        };

        // copy the error into the buffer
        new (::std::get<3>(this->data.error)) libcxx::any(Frame_t(error));
    }

    void set_value(const T &value) { new (&this->data.value) T(value); }
    void set_value(T &&value) { new (&this->data.value) T(std::ref::move(value)); }

    void delete_error() {
        if (this->state == $State::Error) {
            // call the destructor
            ::std::get<1>(this->data.error)(::std::get<3>(this->data.error));
        }
    }

    void panic_error() {
        if (this->state == $State::Error) {
            // call the panic function
            ::std::get<2>(this->data.error)(::std::get<3>(this->data.error));
        }
    }

  public:
    // ----------------------------------- constructors (null) ---------------------------------- //

    $question()
        : state($State::Null) {}
    $question(const null_t & /*unused*/)
        : state($State::Null){};
    $question(const null_t && /*unused*/)
        : state($State::Null) {}
    $question(null_t && /*unused*/)
        : state($State::Null) {}

    /// ---------------------------------- constructors (value) --------------------------------- //
    $question(T &&value)
        : state($State::Value) {
        set_value(std::ref::move(value));
    }

    $question(const T &&value)
        : state($State::Value) {
        set_value(std::ref::move(value));
    }

    $question(const T &value)
        : state($State::Value) {
        set_value(value);
    }

    /// --------------------------------- constructors (error) ---------------------------------- //
    template <typename E>
    $question(Panic::Frame<E> &&error)
        : state($State::Error) {
        this->set_error(std::ref::move(error));
    }

    template <typename E>
    $question(const Panic::Frame<E> &&error)
        : state($State::Error) {
        this->set_error(std::ref::move(error));
    }

    template <typename E>
    $question(const Panic::Frame<E> &error)
        : state($State::Error) {
        this->set_error(error);
    }

    /// ----------------------------------- move constructors ----------------------------------- //
    $question($question &&other) noexcept {
        if (other.state == $State::Error) {
            this->set_error(std::ref::move(other));
            other.delete_error();

            return;
        }

        if (other.state == $State::Value) {
            this->set_value(std::ref::move(other));
            other.~$question();

            return;
        }
    }
    $question &operator=($question &&other) noexcept {
        if (other.state == $State::Error) {
            this->set_error(std::ref::move(other));
            other.delete_error();

            return *this;
        }

        if (other.state == $State::Value) {
            this->set_value(std::ref::move(other));
            other.~$question();

            return *this;
        }

        return *this;
    }

    /// copy constructors
    $question(const $question &other) {
        if (other.state == $State::Error) {
            this->set_error(other);
            return;
        }

        if (other.state == $State::Value) {
            this->set_value(other);
            return;
        }
    }
    $question &operator=(const $question &other) {
        if (other.state == $State::Error) {
            this->set_error(other);
            return *this;
        }

        if (other.state == $State::Value) {
            this->set_value(other);
            return *this;
        }

        return *this;
    }

    /// destructor
    ~$question() {
        if (this->state == $State::Error) {
            delete_error();

            return;
        }

        if (this->state == $State::Value) {
            // call the destructor (if the type has a destructor)
            if constexpr (libcxx::is_destructible_v<T>) {
                this->data.value.~T();
            }

            return;
        }
    }

    /// equality operators (null)
    constexpr bool operator==(const null_t & /*unused*/) const { return check_null(); }
    constexpr bool operator==(const null_t && /*unused*/) const { return check_null(); }
    constexpr bool operator==(null_t & /*unused*/) const { return check_null(); }
    constexpr bool operator==(null_t && /*unused*/) const { return check_null(); }

    constexpr bool operator!=(const null_t & /*unused*/) const { return check_null(); }
    constexpr bool operator!=(const null_t && /*unused*/) const { return check_null(); }
    constexpr bool operator!=(null_t & /*unused*/) const { return check_null(); }
    constexpr bool operator!=(null_t && /*unused*/) const { return check_null(); }

    // equality operators (error)
    // ... == T,
    // this checks against the error type not an error instance
    template <typename E>
        requires Panic::Panicking<E>
    constexpr bool operator==(E && /*unused*/) const {
        return check_error_type(typeid(E));
    }

    template <typename E>
        requires Panic::Panicking<E>
    constexpr bool operator==(const E & /*unused*/) const {
        return check_error_type(typeid(E));
    }

    template <typename E>
        requires Panic::Panicking<E>
    constexpr bool operator!=(E && /*unused*/) const {
        return !check_error_type(typeid(E));
    }

    template <typename E>
        requires Panic::Panicking<E>
    constexpr bool operator!=(const E & /*unused*/) const {
        return !check_error_type(typeid(E));
    }

    template <typename E>
        requires Panic::Panicking<E>
    constexpr bool $contains(E && /* unused */) const {
        return check_error_type(typeid(E));
    }

    template <typename E>
        requires Panic::Panicking<E>
    constexpr bool $contains(const E & /* unused */) const {
        return check_error_type(typeid(E));
    }

    /// cast operator
    // can cast to either an error or a value
    operator T() {
        if (this->state == $State::Value) {
            return this->data.value;
        }

        if (this->state == $State::Error) {
            panic_error();
        }

        $PANIC(errors::NullAccessError("tried to access a null value"));
    }

    T &$cast(T * /*unused*/) const {
        if (this->state == $State::Value) {
            return this->data.value;
        }

        if (this->state == $State::Error) {
            panic_error();
        }

        $PANIC(errors::NullAccessError("tried to access a null value"));
    }

    template <typename E> requires Panic::Panicking<E>
    E &$cast(E * /*unused*/) const {
        if (this->state == $State::Error && check_error_type(typeid(E))) {
            auto err = static_cast<E*>(libcxx::any_cast<Panic::Frame<errors::Error>*>(::std::get<3>(this->data.error)).object);
            return *err;
        }

        $PANIC(errors::NullAccessError("Error specified doesn't match Error held in type."));
    }


  private:
    mutable $State  state = $State::Null;
    mutable $Data_u data;
};

// specialization for the `?` type that can hold an error

template <typename T>
struct answer {
    using type = T;
};

template <typename T>
struct answer<$question<T>> {
    using type = T;
};

template <typename T>
using answer_t = typename answer<T>::type;
// basicly:
// copule of garuntees:
// 1. `let: x: int?` - default value is null even if not specified or the type has a default
// constructor
// 2. `let: x: int? = 0`
// 3. `let: x: int? = null`
// 4. `let: x: int? = Panic::Frame(Panic::normalize(errors::NullAccessError("null access")),
// "source/helix.hlx", 1)` the above 4 are the only ways to create a `?` type a `?` type can
// only hold 3 states: value, null, error optionally theres a type called `Questionable` that
// takes a type and a list of errors that can be held `Questionable<int,
// errors::NullAccessError, T>` - this is a `?` type that can hold an int, a NullAccessError, or
// a RuntimeError or null this allows the user to know what errors can be thrown by the function
// and handle them accordingly the `?` type must implemnt the follow helix specific functions:
// 1. `bool $question() const` - returns true if the value is not null
// 2. `T &$cast(T * /*unused*/) const` - returns the value if its not null, otherwise throws a
// NullAccessError
// 3. `bool $contains(const E & /* unused */) const` - returns true if the error is of type E
// 4. `bool $contains(E && /* unused */) const` - returns true if the error is of type E
// 5. `T &operator*()` - returns the value if its not null, otherwise throws a NullAccessError
// a $contains function is required for each error type that can be held

template <typename T>
using Questionable = $question<T>;

H_STD_NAMESPACE_END
H_NAMESPACE_END
#endif