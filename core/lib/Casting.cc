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

#include <include/core.hh>

H_NAMESPACE_BEGIN

template <typename Ty, typename Up>
constexpr Ty as_cast(Up &value) {
    if constexpr (std::Meta::is_const<std::Meta::reference_removed<Up>> &&
                  std::Meta::same_as<std::Meta::const_removed<Up>, Ty>) {
        return const_cast<Ty>(value);
    } else if constexpr (std::Meta::is_pointer<Ty>) {
        if constexpr (std::Interface::SupportsPointerCast<Up, Ty>) {
            return dynamic_cast<Ty>(value);
        } else {
            return static_cast<Ty>(value);
        }
    } else if constexpr (std::Interface::Castable<Up, Ty>) {
        return value.operator$cast(static_cast<Ty *>(nullptr));
    } else {
        return static_cast<Ty>(value);
    }
}

template <typename Ty, typename Up>
constexpr Ty as_cast(const Up &value) {
    return as_cast<Ty>(const_cast<Up &>(value));  // NOLINT
}

template <typename Ty, typename Up>
constexpr const Ty &as_const(Up &value) {
    return const_cast<const Ty &>(value);
}

template <typename Ty, typename Up>
constexpr const Ty &as_const(const Up &value) {
    return static_cast<const Ty &>(value);
}

template <typename Ty, typename Up>
constexpr Ty as_unsafe(Up value) {
    return reinterpret_cast<Ty>(value);  // NOLINT
}

template <typename Ty, typename Up>
constexpr const Ty as_unsafe(const Up value)  // NOLINT
    requires(std::Meta::is_const<Up> ||
             std::Meta::is_const<Ty>) {
    return reinterpret_cast<const Ty>(value);  // NOLINT
}


H_NAMESPACE_END