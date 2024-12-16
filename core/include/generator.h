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
///  TODO: REMOVE dependency on libc++ and remake the generator to work with only the helix rt   ///
///                                                                                              ///
///----------------------------------------------------------------------------------------------///

#ifndef __$LIBHELIX_GENERATOR__
#define __$LIBHELIX_GENERATOR__

#include "config.h"
#include "libcxx.h"
#include "refs.h"
#include "types.h"


H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

template <LIBCXX_NAMESPACE::movable T>
class $generator {
  public:
    struct promise_type {
        static LIBCXX_NAMESPACE::suspend_always initial_suspend() noexcept { return {}; }
        static LIBCXX_NAMESPACE::suspend_always final_suspend() noexcept { return {}; }
        $generator<T> get_return_object() { return $generator{Handle::from_promise(*this)}; }

        LIBCXX_NAMESPACE::suspend_always yield_value(T value) noexcept {
            current_value = H_STD_NAMESPACE::ref::move(value);
            return {};
        }

        void                     await_transform() = delete;
        [[noreturn]] static void unhandled_exception() { throw; }

        LIBCXX_NAMESPACE::optional<T> current_value;
    };

    using Handle = LIBCXX_NAMESPACE::coroutine_handle<promise_type>;

    $generator()                              = default;
    $generator(const $generator &)            = delete;
    $generator &operator=(const $generator &) = delete;
    explicit $generator(const Handle coroutine)
        : m_coroutine{coroutine} {}

    $generator($generator &&other) noexcept
        : m_coroutine{other.m_coroutine} {
        other.m_coroutine = {};
    }

    $generator &operator=($generator &&other) noexcept {
        if (this != &other) {
            if (m_coroutine) {
                m_coroutine.destroy();
            }

            m_coroutine       = other.m_coroutine;
            other.m_coroutine = {};
        }
        return *this;
    }

    ~$generator() {
        if (m_coroutine) {
            m_coroutine.destroy();
        }

        delete m_iter;
    }

    class Iter {
      public:
        void     operator++() { m_coroutine.resume(); }
        const T &operator*() const { return *m_coroutine.promise().current_value; }
        bool     operator==(LIBCXX_NAMESPACE::default_sentinel_t /*unused*/) const {
            return !m_coroutine || m_coroutine.done();
        }

        explicit Iter(const Handle coroutine)
            : m_coroutine{coroutine} {}

        usize index() const { return m_coroutine.promise().index; }

      private:
        Handle m_coroutine;
    };

    Iter begin() {
        if (m_coroutine) {
            m_coroutine.resume();
        }

        if (m_iter == nullptr) {
            m_iter = new Iter{m_coroutine};  // NOLINT
        }

        return *m_iter;
    }

    LIBCXX_NAMESPACE::default_sentinel_t end() { return {}; }

  private:
    Handle m_coroutine;
    Iter  *m_iter = nullptr;
};

template <LIBCXX_NAMESPACE::movable T>
inline T next($generator<T> &gen) {
    auto iter = gen.begin();
    return *iter;
}

H_STD_NAMESPACE_END
H_NAMESPACE_END
#endif