# Helix Core Documentation

## Core Overview
The Helix Core provides essential utilities and constructs central to the language’s runtime and type system. These components are designed for efficient integration, robust error handling, and advanced type management, ensuring a solid foundation for building high-performance applications.

---

#### The Core is Also the Prelude
#### The Core is Split into 2 Parts
#### The First Part is the C++ Part
#### The Second Part is the Helix Part
#### Both Parts are Globally Accessible in the Helix Language
#### Both Parts are Equally Important to the Language

---

## C++ part - Exposed Functions
### Casting Utilities
#### `std::as_cast`
- **Purpose**: Casts a value from type `Up` to `Ty`, choosing the most appropriate casting mechanism.
- **Variants**:
  - `Ty std::as_cast(Up &value);`
  - `Ty std::as_cast(const Up &value);`
- **Behavior**: Removes `const` qualifiers where necessary.

#### `std::as_const`
- **Purpose**: Performs a `const`-correct cast from `Up` to `Ty`.
- **Variants**:
  - `const Ty &std::as_const(Up &value);`
  - `const Ty &std::as_const(const Up &value);`
- **Behavior**: Returns a `const`-qualified reference to the input value.

#### `std::as_unsafe`
- **Purpose**: Executes an unsafe reinterpretation cast from `Up` to `Ty`.
- **Variants**:
  - `Ty std::as_unsafe(Up value);`
  - `const Ty std::as_unsafe(const Up value)` (with `requires` clause).

---

### Control Flow Constructs
#### `$finally`
- **Purpose**: Guarantees the execution of a cleanup function at the end of a scope.
- **Features**:
  - Executes upon scope exit, regardless of exceptions.
  - Designed for integration with `try-catch-finally` constructs.
- **Implementation**: Wraps a callable and invokes it in the destructor.

#### `$function`
- **Purpose**: Type-erased wrapper for callable entities.
- **Features**:
  - Supports lambdas, function pointers, and functors.
  - Seamless integration with Helix’s `fn` type.
  - Copyable and movable.
  - Evaluates to `true` if a callable is present.
- **Aliases**:
  - `std::Function`: Concise alias for `$function`.

---

### Generators
#### `$generator`
- **Purpose**: Implements generator semantics using C++ coroutines.
- **Features**:
  - Supports `yield` for lazy value production.
  - Provides `Iter` for iterator-style access.
- **Components**:
  - `promise_type`: Manages coroutine lifecycle and state.
  - `Iter`: Facilitates navigation of yielded values.
- **Aliases**:
  - `std::Generator`: Type alias for `$generator`.
- **Utility**:
  - `T std::next($generator<T> &gen);`: Fetches the next value from a generator.

---

### Error Handling
#### `_HX_MC_Q7_INTERNAL_CRASH_PANIC_M`
- **Purpose**: Triggers an immediate and unrecoverable panic with a `Panic::Frame`.
- **Usage**:
  - Constructs a `Frame` with contextual details and invokes `operator$panic`.

#### `_HX_MC_Q7_PANIC_M`
- **Purpose**: Returns a `Panic::Frame` encapsulating an error.
- **Usage**:
  - `_HX_MC_Q7_PANIC_M(err);`

#### `std::Panic::Frame`
- **Purpose**: Represents the context of a panic event.
- **Features**:
  - Stores file, line, and error details.
  - Manages the panic object lifecycle via `FrameContext`.

#### `std::Panic::FrameContext`
- **Purpose**: Manages dynamically allocated objects for panic propagation.
- **Features**:
  - Encapsulates objects using type erasure.
  - Supports cleanup and propagation of panic states.

#### Concepts
- `std::Panic::Panicking`: Validates if a type implements `operator$panic`.
- `std::Panic::PanickingStatic`: Checks for static panic operators.
- `std::Panic::PanickingInstance`: Checks for instance-level panic operators.

#### `_HX_FN_Vi_Q5_13_helixpanic_handler`
- **Purpose**: Helix’s internal panic handler function.
- **Behavior**:
  - Invoked during panic events to log and propagate errors.

---

### Quantum Type
#### `$question`
- **Purpose**: Represents a nullable or error-prone value (`T?`).
- **States**:
  - **Value**: Holds a valid `T`.
  - **Null**:  Represents absence.
  - **Error**: Encapsulates a `std::Panic::Frame`.
- **Features**:
  - Safe state queries (`...?`).
  - Value and error access with type safety.
  - Error type matching (`... in ...`).
- **Aliases**:
  - `std::Questionable`: Type alias for `$question`.

---

### Core Definitions and Utilities
#### Namespace Macros
- **Purpose**: Simplify namespace declarations and align with Helix standards.
- **Macros**:
  - `H_NAMESPACE_BEGIN` / `H_NAMESPACE_END`: Delimit Helix namespaces.
  - `H_STD_NAMESPACE_BEGIN` / `H_STD_NAMESPACE_END`: Delimit Helix's standard library namespace.
  - `H_STD_NAMESPACE`: Alias for `helix::std`.
  - `LIBCXX_NAMESPACE`: Alias for `std`.
  - `LIBC_NAMESPACE`: Alias for `libc`.

---

### Concepts
#### `std::Interface::SupportsOStream`
- **Purpose**: Ensures type `T` supports streaming to an output stream.
- **Requirements**:
  - `os << a` must be valid, where `os` is an `ostream`.

#### `std::Interface::SupportsPointerCast`
- **Purpose**: Verifies that `T` can be dynamically cast to `U`.
- **Requirements**:
  - `dynamic_cast<U>(T)` must be valid.

#### `std::Interface::Castable`
- **Purpose**: Checks if `T` can be cast to `U` via explicit or implicit operators.
- **Requirements**:
  - `T.operator$cast(U *)` or `T.operator U()` must be valid.

#### `std::Interface::ConvertibleToString`
- **Purpose**: Confirms `T` can be converted to a `string`.
- **Requirements**:
  - Satisfies `SupportsOStream` or `Castable<T, string>`.

---

### Memory Management
#### `std::_H_RESERVED$new`
- **Purpose**: Allocates and constructs an object of type `_Tp`.
- **Signature**:
  - `_Tp *std::_H_RESERVED$new(_Ty&&... t)`.
- **Behavior**: Uses perfect forwarding to invoke the constructor.

#### `std::make_aligned`
- **Purpose**: Dynamically allocates aligned memory for a type and constructs the object.
- **Behavior**:
  - Throws `std::bad_alloc` on allocation failure.

#### `std::destroy_aligned`
- **Purpose**: Destroys and deallocates an object created by `make_aligned`.
- **Behavior**:
  - Undefined behavior if used on non-`make_aligned` pointers.

#### `std::Memory::exchange`
- **Purpose**: Atomically replaces an object’s value.
- **Requirements**:
  - `T` must be nothrow-move-constructible and nothrow-assignable.

#### `std::Memory::forward`
- **Purpose**: Provides perfect forwarding of arguments.
- **Overloads**:
  - For lvalue and rvalue references.

#### `std::Memory::as_pointer`
- **Purpose**: Converts a reference to a pointer.
- **Constraints**:
  - Only for non-lvalue-reference types.

#### `std::Memory::as_reference`
- **Purpose**: Converts a pointer to a reference.

---

### Meta Utilities
#### Integral Constants
- **Definitions**:
  - `Meta::_types::integral_constant`: Base type for compile-time constants.
  - `Meta::true_t`, `Meta::false_t`: Boolean constants.

#### Type Traits
- **Key Traits**:
  - `Meta::remove_reference`, `Meta::remove_const`, `Meta::remove_cvref`: Strip type qualifiers.
  - `Meta::add_const`, `Meta::add_lvalue_reference`, `Meta::add_rvalue_reference`: Add type qualifiers.
  - `Meta::is_nothrow_move_constructible`, `Meta::is_nothrow_assignable`: Compile-time checks for move and assignment safety.
  - `Meta::is_copy_constructible`: Checks if a type is copy-constructible.

#### Concepts
- **Key Concepts**:
  - `Meta::convertible_to`: Validates implicit convertibility between types.
  - `Meta::same_as`: Checks for exact type equality.
  - `Meta::is_const`, `Meta::is_class`, `Meta::is_reference`: Type property checks.

---

### Primitive Types
- **Unsigned Integers**: `u8`, `u16`, `u32`, `u64`.
- **Signed Integers**: `i8`, `i16`, `i32`, `i64`.
- **Floating-Point Types**: `f32`, `f64`, `f80`.
- **Pointer-Sized Types**:
  - `usize`, `isize` (platform-dependent).
- **Validation**: Ensures pointer-sized types match platform bitness.

---

### Core Utilities
#### `std::endl`
- **Purpose**: Represents customizable line endings.
- **Usage**:
  - Streams `std::endl` to output.
- **Constructors**:
  - Accepts `string`, `const char*`, or `char` for line ending definition.

#### `std::to_string`
- **Purpose**: Converts any type to a `string`.
- **Mechanisms**:
  - Uses `to_string`, streaming, or type demangling.

#### `std::stringf`
- **Purpose**: Formats strings with placeholders (`{}`).
- **Behavior**:
  - Throws on mismatched arguments.

#### `print`
- **Purpose**: Prints arguments to standard output.
- **Behavior**:
  - Supports variadic arguments and adds a newline unless the last argument is `std::endl`.

---

### Platform Support
- **Platform Detection**:
  - Determines bitness and defines `usize`/`isize` accordingly.
  - Supports 64-bit, 32-bit, 16-bit, and 8-bit architectures.
- **Assertions**:
  - Verifies `usize` and `isize` align with pointer size.

---

### Error Handling
#### `std::null_t`
- **Purpose**: Represents a null value.
- **Usage**:
  - Inline constant: `null`.

## Helix part - Exposed Functions
- `std::assert`

### Classes
#### Libraries

---

### **Core Modules**
Z:\devolopment\helix\helix-lang\build\debug\x64-msvc-windows\bin\helix.exe
#### **Core Utilities**
- `std/core`:
  - `std::core::TypeId`
  - `std::core::Unreachable`
- `std/collections`:
  - `std::collections::Array`
  - `std::collections::Vector`
  - `std::collections::Set`
  - `std::collections::Map`
  - `std::collections::Queue`
  - `std::collections::Deque`
  - `std::collections::Stack`
- `std/iter`:
  - `std::iter::Iterator`
  - `std::iter::Range`
  - `std::iter::Enumerate`
  - `std::iter::Reverse`
- `std/strings`:
  - `std::strings::String`
  - `std::strings::StringView`
  - `std::strings::format`
  - `std::strings::split`
  - `std::strings::trim`
  - `std::strings::replace`
- `std/io`:
  - `std::io::InputStream`
  - `std::io::OutputStream`
  - `std::io::FileReader`
  - `std::io::FileWriter`
- `std/fs`:
  - `std::fs::Path`
  - `std::fs::File`
  - `std::fs::Directory`
- `std/threading`:
  - `std::threading::Thread`
  - `std::threading::Mutex`
  - `std::threading::Condition`
- `std/atomic`:
  - `std::atomic::Atomic`
- `std/time`:
  - `std::time::Duration`
  - `std::time::Clock`
  - `std::time::Timer`
- `std/net`:
  - `std::net::TcpSocket`
  - `std::net::UdpSocket`
  - `std::net::HttpClient`
  - `std::net::HttpRequest`
  - `std::net::HttpResponse`
- `std/math`:
  - `std::math::abs`
  - `std::math::sqrt`
  - `std::math::pow`
  - `std::math::sin`
  - `std::math::cos`
  - `std::math::tan`
  - `std::math::log`
  - /// everything else in c++ math library
- `std/sys`:
  - `std::sys::Process`
  - `std::sys::Env`
  - `std::sys::Process`
  - `std::sys::Env`
  - `std::sys::SysInfo`
  - `std::sys::FileDescriptor`
  - `std::sys::Signals`
  - `std::sys::ResourceUsage`
  - `std::sys::SystemClock`
  - `std::sys::User`
  - `std::sys::Permissions`
  - `std::sys::IOCtl`
  - `std::sys::SocketDescriptor`
  - `std::sys::Memory`
  - `std::sys::Scheduler`
  - `std::sys::Filesystem`
  - `std::sys::Kernel`
  - `std::sys::Pipe`
  - `std::sys::SharedMemory`
  - `std::sys::Event`
  - `std::sys::PowerManagement`
  - `std::sys::NetworkInterfaces`
- `std/json`:
  - `std::json::Json`
  - `std::json::parse`
  - `std::json::stringify`

### Interface
- `interfaces::Compare`
- `interfaces::EqualityCompare`
- `interfaces::Additive`
- `interfaces::Subtractive`
- `interfaces::Multiplicative`
- `interfaces::Divisible`
- `interfaces::Modulo`
- `interfaces::Increment`
- `interfaces::Decrement`
- `interfaces::Negate`
- `interfaces::Hash`
- `interfaces::Iter`
- `interfaces::DefaultConstructible`
- `interfaces::Print`
- `interfaces::CopyConstructible`
- `interfaces::MoveConstructible`
- `interfaces::Call`
- `interfaces::Index`
- `interfaces::Transform`
- `interfaces::PredicateFilter`
- `interfaces::ConditionEvaluate`
- `Interface::ErrorHandle`