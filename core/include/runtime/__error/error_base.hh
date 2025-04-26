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

#ifndef _$_HX_CORE_M10ERROR_BASE
#define _$_HX_CORE_M10ERROR_BASE

#include <include/config/config.hh>

#include <include/runtime/__memory/memory.hh>
#include <include/types/string/basic.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Error {
class Error /* with Panicking */ {
  public:
    Error() = default;

    Error(const Error &other)     = default;
    Error(Error &&other) noexcept = default;

    Error &operator=(const Error &other)     = default;
    Error &operator=(Error &&other) noexcept = default;

    virtual ~Error() = default;

    [[nodiscard]] virtual string operator$panic() const { return L"An error occurred."; }
    [[nodiscard]] virtual string operator$cast(string * /*unused*/) const = 0;  // string cast
};
};  // namespace Error

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M10ERROR_BASE
