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

#include "../config.h"
#include "../memory.h"

H_NAMESPACE_BEGIN


/// \class $generator
///
/// A core component of the Helix runtime, `$generator` provides support for generator semantics,
/// enabling `yield` types and `yield return` statements in Helix programs. Generators are an
/// abstraction for managing lazily-evaluated sequences, providing an efficient and expressive way
/// to produce and consume elements in a coroutine-based workflow.
///
/// ### Overview
/// `$generator` is implemented using C++ coroutines and integrates seamlessly with the Helix
/// runtime. It supports the definition of functions that can produce multiple values lazily using
/// the `yield` keyword. Generators simplify writing iterators, state machines, and other
/// sequence-producing constructs.
///
/// ### Key Features
/// - **Generator Semantics**:
///   - Allows the definition of generator functions with `fn ... -> yield T;`.
///   - Supports the `yield` keyword to produce values one at a time.
///   - Compatible with range-based loops and manual iteration.
/// - **Coroutines Integration**:
///   - Leverages C++ coroutines for efficient state management.
///   - Manages coroutine lifecycle, including proper destruction and cleanup.
/// - **Iterators**:
///   - Provides an `Iter` class to navigate through the yielded values.
///   - Supports standard iteration constructs, such as `for` loops and `begin`/`end` methods.
///
/// ### Generator Lifecycle
/// 1. **Initialization**:
///    - A generator function initializes a `$generator` object when called.
///    - The coroutine state is suspended initially and resumes upon iteration.
/// 2. **Yielding Values**:
///    - Each `yield` statement suspends the coroutine and provides a value to the caller.
///    - The coroutine resumes execution on the next iteration.
/// 3. **Completion**:
///    - When the generator function completes, the coroutine is destroyed.
///    - All resources used by the generator are cleaned up.
///
/// ### Integration with `yield`
/// In Helix, functions with a `yield` return type (`fn ... -> yield T;`) automatically generate a
/// `$generator<T>` as their return type. This allows developers to use `yield` to produce values
/// incrementally:
///
/// ```helix
/// fn range(start: int, end: int) -> yield int {
///     for i in start..end {
///         yield i;
///     }
/// }
///
/// let g = range(1, 5);
/// for val in g {
///     print(val); // Output: 1, 2, 3, 4
/// }
/// ```
///
/// ### Components
/// #### `promise_type`
/// - Represents the coroutine promise associated with the generator.
/// - Manages the yielded values and coroutine state.
/// - Provides the `yield_value` method for handling `yield` expressions.
/// - Responsible for the coroutine's initial and final suspension points.
///
/// #### `Iter`
/// - Provides iterator support for navigating the generator's yielded values.
/// - Implements increment (`operator++`), dereference (`operator*`), and comparison operators.
///
/// #### `Handle`
/// - Alias for the coroutine handle associated with the generator.
/// - Facilitates the management and destruction of the coroutine.
///
/// ### Functions
/// #### `next($generator<T> &gen)`
/// - Retrieves the next value from the generator.
/// - Automatically resumes the coroutine and fetches the current value:
///   ```helix
///   let g = range(1, 5);
///   print(next(g)); // Output: 1
///   ```
///
/// ### Example Usage
/// ```helix
/// fn some_generator(start: int, end: int) -> yield int {
///     for i in start..end {
///         yield i;
///     }
/// }
///
/// let g = some_generator(10, 15);
/// for val in g {
///     print(val); // Output: 10, 11, 12, 13, 14
/// }
/// ```
///
/// ### Notes
/// - `$generator` is a Helix runtime feature and is part of the standard library.
/// - The implementation currently relies on `libc++` and may be subject to refactoring to remove
///   external dependencies in future versions.
///
/// ### Related Concepts
/// - `yield`: Used in Helix to produce values in generator functions.
/// - `Iter`: The iterator class for navigating generator results.
template <LIBCXX_NAMESPACE::movable T>
class $generator {
public:
    struct promise_type {
        constexpr static LIBCXX_NAMESPACE::suspend_always initial_suspend() noexcept { return {}; }
        constexpr static LIBCXX_NAMESPACE::suspend_always final_suspend() noexcept { return {}; }

        $generator get_return_object() noexcept {
            return $generator{Handle::from_promise(*this)};
        }

        LIBCXX_NAMESPACE::suspend_always yield_value(T value) noexcept {
            current_value = H_STD_NAMESPACE::memory::move(value);
            return {};
        }

        void return_void() noexcept {}
        void await_transform() = delete;
        [[noreturn]] static void unhandled_exception() { throw; }

        LIBCXX_NAMESPACE::optional<T> current_value;
    };

    using Handle = LIBCXX_NAMESPACE::coroutine_handle<promise_type>;

    constexpr $generator() noexcept = default;

    explicit $generator(Handle coroutine) noexcept
        : m_coroutine(coroutine) {}

    $generator(const $generator &) = delete;
    $generator &operator=(const $generator &) = delete;

    $generator($generator &&other) noexcept
        : m_coroutine(H_STD_NAMESPACE::memory::exchange(other.m_coroutine, {})) {}

    $generator &operator=($generator &&other) noexcept {
        if (this != &other) {
            if (m_coroutine) {
                m_coroutine.destroy();
            }
            m_coroutine = H_STD_NAMESPACE::memory::exchange(other.m_coroutine, {});
        }
        return *this;
    }

    ~$generator() {
        if (m_coroutine) {
            m_coroutine.destroy();
        }
    }

    class Iter {
    public:
        explicit Iter(Handle coroutine) noexcept
            : m_coroutine(coroutine) {}

        void operator++() noexcept { m_coroutine.resume(); }

        const T &operator*() const noexcept {
            return *m_coroutine.promise().current_value;
        }

        constexpr bool operator==(LIBCXX_NAMESPACE::default_sentinel_t) const noexcept {
            return !m_coroutine || m_coroutine.done();
        }

    private:
        Handle m_coroutine;
    };

    Iter begin() noexcept {
        if (m_coroutine) {
            m_coroutine.resume();
        }
        return Iter{m_coroutine};
    }

    constexpr LIBCXX_NAMESPACE::default_sentinel_t end() noexcept { return {}; }

private:
    Handle m_coroutine = nullptr;
};

H_STD_NAMESPACE_BEGIN

template <LIBCXX_NAMESPACE::movable T>
using Generator = $generator<T>;

template <LIBCXX_NAMESPACE::movable T>
inline T next($generator<T> &gen) {
    auto iter = gen.begin();
    return *iter;
}

H_STD_NAMESPACE_END
H_NAMESPACE_END
#endif