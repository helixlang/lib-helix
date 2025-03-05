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

#include <include/config/config.h>
#include <include/core.hh>

#define QUESTION $question<T>

H_NAMESPACE_BEGIN

template <class T>
[[nodiscard]] constexpr bool QUESTION::is_null() const noexcept {
    return state == $State::Null;
}
template <class T>
[[nodiscard]] constexpr bool QUESTION::is_err() const noexcept {
    return state == $State::Error;
}
template <class T>
[[nodiscard]] constexpr bool QUESTION::is_err(const libcxx::type_info *type) const noexcept {
    return state == $State::Error && (*data.error.get_context()) == type;
}

template <class T>
constexpr void QUESTION::set_value(const T &value) {
    new (&data.value) T(value);
}
template <class T>
constexpr void QUESTION::set_value(T &&value) {
    new (&data.value) T(std::Memory::move(value));
}
template <class T>
constexpr void QUESTION::set_err(std::Panic::Frame &&error) {
    new (&data.error) std::Panic::Frame(std::Memory::move(error));
}

template <class T>
constexpr void QUESTION::delete_error() noexcept {
    data.error.~Frame();
}
template <class T>
constexpr void QUESTION::delete_value() noexcept {
    data.value.~T();
}

/// ------------------------------- Constructors (Null) -------------------------------
template <class T>
constexpr QUESTION::$question() noexcept
    : state($State::Null) {}
template <class T>
constexpr QUESTION::$question(const std::null_t &) noexcept
    : state($State::Null) {}
template <class T>
constexpr QUESTION::$question(std::null_t &&) noexcept
    : state($State::Null) {}

/// ------------------------------- Constructors (Value) -------------------------------
template <class T>
constexpr QUESTION::$question(const T &value)
    : state($State::Value) {
    set_value(value);
}
template <class T>
constexpr QUESTION::$question(T &&value)
    : state($State::Value) {
    set_value(std::Memory::move(value));
}

/// ------------------------------- Constructors (Error) -------------------------------
template <class T>
constexpr QUESTION::$question(const std::Panic::Frame &error)
    : state($State::Error) {
    set_err(error);
}
template <class T>
constexpr QUESTION::$question(std::Panic::Frame &&error)
    : state($State::Error) {
    set_err(std::Memory::move(error));
}

/// ------------------------------- Move Constructor & Assignment
/// -------------------------------
template <class T>
constexpr QUESTION::$question($question &&other) noexcept
    : state(other.state) {
    if (state == $State::Error) {
        set_err(std::Memory::move(other.data.error));
    } else if (state == $State::Value) {
        set_value(std::Memory::move(other.data.value));
    }
}

template <class T>
constexpr QUESTION &QUESTION::operator=($question &&other) noexcept {
    if (this != &other) {
        if (state == $State::Error) {
            delete_error();
        } else if (state == $State::Value) {
            delete_value();
        }

        state = other.state;
        if (state == $State::Error) {
            set_err(std::Memory::move(other.data.error));
        } else if (state == $State::Value) {
            set_value(std::Memory::move(other.data.value));
        }
    }
    return *this;
}

/// --------------------- Copy Constructor & Assignment ----------------------
template <class T>
constexpr QUESTION::$question(const $question &other)
    : state(other.state) {
    if (state == $State::Error) {
        set_err(other.data.error);
    } else if (state == $State::Value) {
        set_value(other.data.value);
    }
}

template <class T>
constexpr QUESTION &QUESTION::operator=(const $question &other) {
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
template <class T>
constexpr QUESTION::~$question() {
    if (state == $State::Error) {
        delete_error();
    } else if (state == $State::Value) {
        if constexpr (std::Meta::is_destructible<T>) {
            delete_value();
        }
    }
}

/// ------------------------------- Operators -------------------------------
template <class T>
constexpr bool QUESTION::operator==(const std::null_t &) const noexcept {
    return is_null();
}
template <class T>
constexpr bool QUESTION::operator!=(const std::null_t &) const noexcept {
    return !is_null();
}

template <class T>
template <typename E>
constexpr bool QUESTION::operator==(const E &) const noexcept {
    if constexpr (std::Panic::Interface::Panicking<E>) {
        return is_err(&typeid(E));
    }

#if defined(_MSC_VER)
    if constexpr (false) {
#endif

        _HX_MC_Q7_INTERNAL_CRASH_PANIC_M(
            std::Error::TypeMismatchError(L"Invalid: error type does not match any panic state."));

#if defined(_MSC_VER)
    }
#endif
}

template <class T>
template <typename E>
constexpr bool QUESTION::operator!=(const E &other) const noexcept {
    return !(*this == other);
}

template <class T>
template <typename E>
constexpr bool QUESTION::operator$contains(const E &other) const noexcept {
    return *this == other;
}

template <class T>
[[nodiscard]] constexpr bool QUESTION::operator$question() const noexcept {
    return state == $State::Value;
}

/// ------------------------------- Casting -------------------------------
template <class T>
template <typename E>
    requires std::Panic::Interface::Panicking<E>
constexpr E QUESTION::operator$cast(E * /*unused*/) const {
    if (state == $State::Error) {
        if (is_err(&typeid(E))) {
            auto *obj = (*data.error.get_context()).object();
            if (obj) {
                return *reinterpret_cast<E *>(obj);
            }
            _HX_MC_Q7_INTERNAL_CRASH_PANIC_M(
                std::Error::NullValueError(L"Invalid Decay: error context object is null."));
        }
        data.error.operator$panic();
    }

    if (state == $State::Value) {
        if constexpr (std::Meta::same_as<T, E>) {
            return data.value;
        }

        _HX_MC_Q7_INTERNAL_CRASH_PANIC_M(std::Error::TypeMismatchError(L"Invalid cast: value type does not match requested type."));
    }

    _HX_MC_Q7_INTERNAL_CRASH_PANIC_M(std::Error::NullValueError(L"Invalid Decay: value is null."));
}

template <class T>
constexpr T QUESTION::operator$cast(T * /*unused*/) const {
    if (state == $State::Value) {
        return data.value;
    }

    if (state == $State::Error) {
        data.error.operator$panic();
    }

    _HX_MC_Q7_INTERNAL_CRASH_PANIC_M(std::Error::NullValueError(L"Invalid Decay: value is null."));
}

template <class T>
[[nodiscard]] constexpr T &QUESTION::operator*() {
    return operator$cast(static_cast<T *>(nullptr));
}
template <class T>
[[nodiscard]] constexpr QUESTION::operator T() {
    return operator$cast(static_cast<T *>(nullptr));
}

H_NAMESPACE_END