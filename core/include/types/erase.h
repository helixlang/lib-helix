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

#ifndef __$LIBHELIX_ERASE__
#define __$LIBHELIX_ERASE__

#include "../config.h"
#include "../types.h"
#include "../types/errors.h"

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

/// \class TypeErasure
///
/// `TypeErasure` is an abstract base class that provides a mechanism for type erasure.
/// This class serves as the foundation for managing objects of arbitrary types without
/// exposing their concrete implementation details. It is particularly useful for
/// generic systems requiring heterogeneous collections, dynamic polymorphism, or
/// runtime type inspection.
///
/// ### Design Details
/// - **Abstract Interface**: `TypeErasure` defines an interface with pure virtual
///   methods that subclasses must implement. These methods handle object destruction,
///   type retrieval, cloning, and access.
/// - **Dynamic Lifetime Management**: It ensures safe destruction and cleanup of
///   objects, allowing proper lifecycle management within a type-erased context.
/// - **Runtime Type Information**: The `type_info` method provides RTTI for the
///   stored object, enabling dynamic type checks.
///
/// ### Usage
/// `TypeErasure` is not meant to be used directly. Instead, it is subclassed to wrap
/// specific object types. For example, `TypeErasureImpl<T>` is a concrete implementation
/// that can store objects of type `T`.
///
/// ### Responsibilities
/// - Provide an interface for type-erased storage and manipulation.
/// - Abstract away the underlying type of stored objects while maintaining access
///   to their functionality.
///
/// ### Interactions with FrameContext
/// `TypeErasure` is a key component of the `FrameContext` class. It allows `FrameContext`
/// to store and manage objects of arbitrary types while ensuring proper cleanup and
/// exception handling.
///
/// ### Key Methods
/// - `destroy()`: Safely destroys the stored object.
/// - `type_info()`: Retrieves runtime type information of the stored object.
/// - `clone()`: Creates a deep copy of the type-erased object.
///
/// ### Example
/// \code{.cpp}
/// // creating a typeerasure instance
/// TypeErasure *erased = new TypeErasureImpl<MyType>(new MyType());
///
/// // accessing type information
/// const LIBCXX_NAMESPACE::type_info *info = erased->type_info();
///
/// // destroying the object
/// erased->destroy();
/// delete erased;
/// \endcode
class TypeErasure {
  public:
    virtual ~TypeErasure() = default;

    TypeErasure()                                   = default;
    TypeErasure(const TypeErasure &)                = default;
    TypeErasure &operator=(const TypeErasure &)     = default;
    TypeErasure(TypeErasure &&) noexcept            = default;
    TypeErasure &operator=(TypeErasure &&) noexcept = default;

    virtual void                                   destroy()         = 0;
    virtual void                                  *operator*()       = 0;
    [[nodiscard]] virtual const LIBCXX_NAMESPACE::type_info *type_info() const = 0;
    [[nodiscard]] virtual TypeErasure             *clone() const     = 0;
};

/// \class TypeErasureImpl
///
/// \warning This class does not free the memory of the object it holds, it is the
///          responsibility of the caller to free the memory.
///
/// `TypeErasureImpl` is a templated implementation of the `TypeErasure` interface. It
/// provides type-erased storage and management for objects of type `T`. This class
/// encapsulates the object, allowing it to be manipulated without exposing its type.
///
/// ### Design Details
/// - **Type-Specific Storage**: `TypeErasureImpl` wraps an object of type `T`,
///   providing type-erased functionality.
/// - **Dynamic Behavior**: Implements methods for destruction, cloning, and access,
///   ensuring the stored object can be safely managed within a type-erased context.
/// - **Integration with FrameContext**: This class is used by `FrameContext` to
///   manage objects dynamically, supporting panic handling and exception throwing.
///
/// ### Responsibilities
/// - Manage the lifetime of an object of type `T`.
/// - Provide a mechanism to retrieve runtime type information for `T`.
/// - Enable cloning of the type-erased wrapper.
///
/// ### Implementation Notes
/// - The `destroy()` method ensures the wrapped object is properly deleted.
/// - The `clone()` method creates a deep copy of the wrapped object, maintaining
///   type safety within the type-erased system.
///
/// ### Interactions
/// `TypeErasureImpl` is used internally by `FrameContext` and is not typically
/// exposed to end users. It enables `FrameContext` to work with heterogeneous types
/// while maintaining strong exception safety guarantees.
///
/// \warning This class is NOT memory safe.
///
/// ### Example
/// \code{.cpp}
/// // wrapping an object of type mytype
/// MyType *myObj = new MyType();
/// TypeErasureImpl<MyType> erased(myObj);
///
/// // accessing runtime type information
/// const LIBCXX_NAMESPACE::type_info *info = erased.type_info();
///
/// // safely destroying the object
/// erased.destroy();
/// \endcode
template <typename T>
class TypeErasureImpl : public TypeErasure {
  private:
    T                       *object;
    const LIBCXX_NAMESPACE::type_info *type;
    bool                     is_destroyed = false;  // Guard against double deletion

  public:
    TypeErasureImpl(const TypeErasureImpl &other)
        : object(new T(*other.object))
        , type(other.type)
        , is_destroyed(other.is_destroyed) {}

    TypeErasureImpl &operator=(const TypeErasureImpl &other) {
        if (this != &other) {
            if (object != nullptr) {  // NOLINT(readability-delete-null-pointer)
                delete object;
            }

            object       = new T(*other.object);  // NOLINT(cppcoreguidelines-owning-memory)
            is_destroyed = other.is_destroyed;
            type         = other.type;
        }
        return *this;
    }

    TypeErasureImpl(TypeErasureImpl &&other) noexcept
        : object(other.object)
        , type(other.type)
        , is_destroyed(other.is_destroyed) {
        other.object       = nullptr;
        other.is_destroyed = true;
    }

    TypeErasureImpl &operator=(TypeErasureImpl &&other) noexcept {
        if (this != &other) {
            if (object != nullptr) { // NOLINT(readability-delete-null-pointer)
                delete object;
            }

            object             = other.object;
            is_destroyed       = other.is_destroyed;
            other.object       = nullptr;
            other.is_destroyed = true;
            type               = other.type;
        }
        return *this;
    }

    explicit TypeErasureImpl(T *obj)
        : object(obj)
        , type(&typeid(T)) {}

    void destroy() override {
        if (!is_destroyed && object != nullptr) {
            delete object;
            object       = nullptr;
            is_destroyed = true;
        }
    }

    void *operator*() override { return object; }

    [[nodiscard]] const LIBCXX_NAMESPACE::type_info *type_info() const override { return type; }

    [[nodiscard]] TypeErasure *clone() const override {
        if (object == nullptr) {
            throw errors::RuntimeError("Cannot clone a null object.");
        }

        return new TypeErasureImpl<T>(new T(*object));  // NOLINT(cppcoreguidelines-owning-memory)
    }

    ~TypeErasureImpl() override { object = nullptr; }
};

/// \fn erase_type
///
/// Erases the type of an object `obj` and returns a `TypeErasure` wrapper.
///
/// ### Purpose
/// The `erase_type` function creates a type-erased wrapper for an object, allowing
/// it to be stored and manipulated without exposing its concrete type. This is useful
/// for working with heterogeneous collections or dynamic polymorphism.
///
/// ### Parameters
/// - `obj`: A pointer to the object to be type-erased.
///
/// ### Returns
/// A `TypeErasure` pointer that wraps the object, providing type-erased access.
///
/// ### Example
/// ```cpp
/// MyType *obj = new MyType();
/// TypeErasure *erased = erase_type(obj);
/// ```
template <typename T>
[[nodiscard]] TypeErasure *erase_type(T *obj) {
    return new TypeErasureImpl<T>(obj);  // NOLINT(cppcoreguidelines-owning-memory)
}

H_STD_NAMESPACE_END
H_NAMESPACE_END
#endif