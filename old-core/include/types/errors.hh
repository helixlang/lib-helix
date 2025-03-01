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

#ifndef __$LIBHELIX_ERRORS__
#define __$LIBHELIX_ERRORS__

#include "../config.h"
#include "../libcxx.h"
#include "../memory.h"

#include "forward.hh"

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

    [[nodiscard]] virtual string operator$panic() const { return "An error occurred."; }
    [[nodiscard]] virtual string operator$cast(string * /*unused*/) const = 0;  // string cast
};

class NullValueError : public Error {
  public:
    NullValueError() = default;

    NullValueError(string msg)  // NOLINT(google-explicit-constructor)
        : msg(H_STD_NAMESPACE::Memory::move(msg)) {}

    NullValueError(const NullValueError &other)     = default;
    NullValueError(NullValueError &&other) noexcept = default;

    NullValueError &operator=(const NullValueError &other)     = default;
    NullValueError &operator=(NullValueError &&other) noexcept = default;

    ~NullValueError() override = default;

    [[nodiscard]] string operator$panic() const override { return msg; }
    [[nodiscard]] string operator$cast(string * /*unused*/) const override { return msg; }

  private:
    string msg;
};

class RuntimeError : public Error {
  public:
    RuntimeError() = default;

    RuntimeError(string msg)  // NOLINT(google-explicit-constructor)
        : msg(H_STD_NAMESPACE::Memory::move(msg)) {}

    RuntimeError(const RuntimeError &other)     = default;
    RuntimeError(RuntimeError &&other) noexcept = default;

    RuntimeError &operator=(const RuntimeError &other)     = default;
    RuntimeError &operator=(RuntimeError &&other) noexcept = default;

    ~RuntimeError() override = default;

    [[nodiscard]] string operator$panic() const override { return msg; }
    [[nodiscard]] string operator$cast(string * /*unused*/) const override { return msg; }

  private:
    string msg;
};

class StateMismatchError : public Error {
  public:
    StateMismatchError() = default;

    StateMismatchError(string msg)  // NOLINT(google-explicit-constructor)
        : msg(H_STD_NAMESPACE::Memory::move(msg)) {}

    StateMismatchError(const StateMismatchError &other)     = default;
    StateMismatchError(StateMismatchError &&other) noexcept = default;

    StateMismatchError &operator=(const StateMismatchError &other)     = default;
    StateMismatchError &operator=(StateMismatchError &&other) noexcept = default;

    ~StateMismatchError() override = default;

    [[nodiscard]] string operator$panic() const override { return msg; }
    [[nodiscard]] string operator$cast(string * /*unused*/) const override { return msg; }

  private:
    string msg;
};

class TypeMismatchError : public Error {
  public:
    TypeMismatchError() = default;

    TypeMismatchError(string msg)  // NOLINT(google-explicit-constructor)
        : msg(H_STD_NAMESPACE::Memory::move(msg)) {}

    TypeMismatchError(const TypeMismatchError &other)     = default;
    TypeMismatchError(TypeMismatchError &&other) noexcept = default;

    TypeMismatchError &operator=(const TypeMismatchError &other)     = default;
    TypeMismatchError &operator=(TypeMismatchError &&other) noexcept = default;

    ~TypeMismatchError() override = default;

    [[nodiscard]] string operator$panic() const override { return msg; }
    [[nodiscard]] string operator$cast(string * /* unused */) const override { return msg; }

  private:
    string msg;
};
}  // namespace errors

H_STD_NAMESPACE_END
H_NAMESPACE_END
#endif