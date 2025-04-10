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

#ifndef _$_HX_CORE_M13FRAME_CONTEXT
#define _$_HX_CORE_M13FRAME_CONTEXT

#include <include/config/config.h>

#include <include/runtime/__function/function_impl.hh>
#include <include/types/type_erasure/erasure_base.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

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
    std::TypeErasure                   *error;    ///< The error object.
    $function<void(std::TypeErasure *)> handler;  ///< The panic handler.

    template <typename T>
    [[noreturn]] static constexpr void throw_object(std::TypeErasure *);

  public:
    FrameContext();
    FrameContext(const FrameContext &other);
    FrameContext &operator=(const FrameContext &other);
    FrameContext(FrameContext &&other) noexcept;
    FrameContext &operator=(FrameContext &&other) noexcept;
    ~FrameContext();

    template <typename T>
    explicit FrameContext(T *obj);

    [[noreturn]] void    crash();
    [[nodiscard]] void  *object() const;
    [[nodiscard]] string type_name() const;

    bool operator!=(const libcxx::type_info *rhs) const;
    bool operator==(const libcxx::type_info *rhs) const;
};
};  // namespace Panic

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M13FRAME_CONTEXT
