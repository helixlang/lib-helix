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

#ifndef __$LIBHELIX_FUNCTION__
#define __$LIBHELIX_FUNCTION__

#include "../config.h"
#include "../libcxx.h"
#include "../refs.h"

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

template <typename _Sig>
class $function;

/// $function<void(int)> accepts any callable such as a lambda, function ptr, functors and more.
template <typename _Rt, typename... _Tp>
class $function<_Rt(_Tp...)> {
  private:
    struct $callable {
        $callable()                                 = default;
        $callable(const $callable &)                = default;
        $callable &operator=(const $callable &)     = default;
        $callable($callable &&) noexcept            = default;
        $callable &operator=($callable &&) noexcept = default;
        virtual ~$callable()                        = default;
        virtual _Rt        invoke(_Tp... args)      = 0;
        virtual $callable *clone() const            = 0;
    };

    template <typename T>
    struct Callable : $callable {
        T callable;

        Callable(typename ref::remove_ref<T> &&callable)
            : callable(H_STD_NAMESPACE::forward<T>(callable)) {}
        Callable(T &&$call_o)
            : callable(H_STD_NAMESPACE::forward<T>($call_o)) {}
        Callable(const T &callable)
            : callable(callable) {}

        _Rt invoke(_Tp... args) override {
            return callable(H_STD_NAMESPACE::forward<_Tp>(args)...);
        }
        $callable *clone() const override { return new Callable(callable); }
    };

    $callable *callable;

  public:
    $function()
        : callable(nullptr) {}

    $function($function &&other) noexcept
        : callable(other.callable) {
        other.callable = nullptr;
    }

    $function(const $function &other)
        : callable(other.callable ? other.callable->clone() : nullptr) {}

    template <typename T>
    $function(typename ref::remove_ref_t<T> $call_o)
        : callable(
              new Callable<LIBCXX_NAMESPACE::decay_t<T>>(H_STD_NAMESPACE::forward<T>($call_o))) {}

    template <typename T>
    $function(T $call_o)
        : callable(
              new Callable<LIBCXX_NAMESPACE::decay_t<T>>(H_STD_NAMESPACE::forward<T>($call_o))) {}

    $function(_Rt (*func)(_Tp...))
        : callable(func ? new Callable<ref::remove_cv_t<_Rt (*)(_Tp...)>>(func) : nullptr) {}

    ~$function() { reset(); }

    $function &operator=($function &&other) noexcept {
        if (this != &other) {
            delete callable;

            callable       = other.callable;
            other.callable = nullptr;
        }

        return *this;
    }

    $function &operator=(const $function &other) {
        if (this != &other) {
            delete callable;
            callable = other.callable ? other.callable->clone() : nullptr;
        }
        return *this;
    }

    template <typename T>
    $function &operator=(T $call_o) {
        delete callable;

        callable = new Callable<LIBCXX_NAMESPACE::decay_t<T>>(H_STD_NAMESPACE::forward<T>($call_o));
        return *this;
    }

    // Assignment for function pointers
    $function &operator=(_Rt (*func)(_Tp...)) {
        delete callable;
        callable = func ? new Callable<_Rt (*)(typename ref::remove_ref_t<_Tp>...)>(func) : nullptr;
        return *this;
    }

    explicit           operator bool() const noexcept { return callable != nullptr; }
    [[nodiscard]] bool operator$question() const noexcept { return callable != nullptr; }

    _Rt operator()(_Tp... args) {
        if (!callable) {
            throw "called an unset function pointer";
        }

        return callable->invoke(H_STD_NAMESPACE::forward<_Tp>(args)...);
    }

    void reset() noexcept {
        delete callable;
        callable = nullptr;
    }
};

H_STD_NAMESPACE_END
H_NAMESPACE_END
#endif