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

#ifndef _$_HX_CORE_M16NULL_VALUE_ERROR
#define _$_HX_CORE_M16NULL_VALUE_ERROR

#include <include/config/config.h>
#include <include/types/string/basic.hh>

#include "error_base.hh"

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Error {
class NullValueError : public Error {
  public:
    NullValueError() = default;

    NullValueError(string msg)  // NOLINT(google-explicit-constructor)
        : msg(std::Memory::move(msg)) {}

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
};  // namespace Error

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M16NULL_VALUE_ERROR
