# this documents all the parts of the langauge core thats written in helix
# the core is also the prelude
# the core is split into 2 parts
# the first part is the C++ part
# the second part is the Helix part
# both parts are globally accessible in the Helix language
# both parts are equally important to the language

# this document specifies the C++ and Helix parts of the core
## C++ part - Exposed Functions
### Language Features
#### Casting `... as ...`
- `as_cast`   | `template <typename _Ty, typename _Up> _Ty        as_cast(_Up &value);`
- `as_cast`   | `template <typename _Ty, typename _Up> _Ty        as_cast(const _Up &value);`
- `as_const`  | `template <typename _Ty, typename _Up> const _Ty &as_const(_Up &value);`
- `as_const`  | `template <typename _Ty, typename _Up> const _Ty &as_const(const _Up &value);`
- `as_unsafe` | `template <typename _Ty, typename _Up> _Ty        as_unsafe(_Up value);`
- `as_unsafe` | `template <typename _Ty, typename _Up> const _Ty  as_unsafe(const _Up value)`

#### Finally `finally { ... }`
The `$finally` class cannot be used directly, it only works with the `finally` keyword.
- `$finally` | `class $finally`

#### Generators `yield ...`
The `$generator` class cannot be used directly, Helix provides a `Range` class that can be used to create generators.
- `$generator` | `template <LIBCXX_NAMESPACE::movable T> class $generator`

#### Questionable `...?`
`$question`           | `template <class T, typename... Es> class $question;`
`null`                | `null`
`std::null_t`         | `class null_t;`

### Panic
- `std::Panic::Frame`         | `class Frame;`
- `std::Panic::FrameContext`  | `class FrameContext;`
- `std::Panic::Frame`         | `class Frame;`

### Helpers
#### Concepts (helpers)
- `std::concepts::HasToString`         | `template <typename T>                 HasToString`
- `std::concepts::SupportsOStream`     | `template <typename T>                 SupportsOStream`
- `std::concepts::SupportsPointerCast` | `template <typename _Ty, typename _Up> SupportsPointerCast`
- `std::concepts::SafelyCastable`      | `template <typename _Ty, typename _Up> SafelyCastable`
- `std::concepts::ConvertibleToString` | `template <typename T>                 ConvertibleToString`
- `std::Panic::Panicking`
- `std::Panic::PanickingStatic`
- `std::Panic::PanickingInstance`

#### Concepts (traits)
- `std::meta::integral_constant` | `template <class _Tp, _Tp __v>   struct integral_constant;`
- `std::meta::__declval`         | `template <class _Tp>            _Tp &&__declval(int);`
- `std::meta::__declval`         | `template <class _Tp>            _Tp __declval(long);`
- `std::meta::declval`           | `template <class _Tp>            decltype(__declval<_Tp>(0)) declval() _NOEXCEPT;`
- `std::meta::is_convertible_v`  | `template <class _Up, class _Tp> constexpr bool is_convertible_v;`
- `std::meta::convertible_to`    | `template <class _Up, class _Tp> concept convertible_to;`
- `std::meta::same_as`         | `template <class _Tp, class _Up> constexpr bool same_as;`
- `std::meta::same_as`           | `template <class _Tp, class _Up> concept same_as;`
- `std::meta::same_as`         | `template <class _Tp, class _Up> inline constexpr bool same_as;`
- `std::meta::is_derived_of`     | `template <class _Bp, class _Dp> struct is_derived_of;`
- `std::meta::is_derived_of_v`   | `template <class _Bp, class _Dp> inline constexpr bool is_derived_of_v;`
- `std::meta::is_class`          | `template <class _Tp>            struct is_class;`
- `std::meta::is_class_v`        | `template <class _Tp>            constexpr bool is_class_v;`
- `std::meta::is_const_v`        | `template <class>                constexpr bool is_const_v;`
- `std::meta::is_const_v`        | `template <class _Tp>            constexpr bool is_const_v<const _Tp>;`
- `std::meta::is_const`          | `template <class _Tp>            struct is_const;`

##### Reference Traits
- `std::remove_ref`     | `template <typename T> struct remove_ref;`
- `std::remove_ref`     | `template <typename T> struct remove_ref<T &>;`
- `std::remove_ref`     | `template <typename T> struct remove_ref<T &&>;`
- `std::remove_cv`      | `template <typename T> struct remove_cv;`
- `std::remove_cv`      | `template <typename T> struct remove_cv<const T>;`
- `std::remove_cv`      | `template <typename T> struct remove_cv<volatile T>;`
- `std::remove_cv`      | `template <typename T> struct remove_cv<const volatile T>;`
- `std::remove_cvref`   | `template <typename T> struct remove_cvref;`
- `std::remove_ref_t`   | `template <typename T> using remove_ref_t = typename remove_ref<T>::t;`
- `std::remove_cv_t`    | `template <typename T> using remove_cv_t = typename remove_cv<T>::t;`
- `std::remove_cvref_t` | `template <typename T> using remove_cvref_t = typename remove_cvref<T>::t;`

### Types
#### Core Derived Data Types (these are types that cannot be defined in helix since helix relies on them)
Example a list: `[1, 2, 3]` is either a array or a list type (undecided for now), but the type is a core type, since without it, the `[..., ...]` syntax would not work.

`to_string` | `template <typename _Ty> string to_string(_Ty &&t)`
`string`    | `using string = LIBCXX_NAMESPACE::string;`
`array`     | `template <typename _Ty, usize _Size>  using array;`
`tuple`     | `template <typename... _Tv>            class tuple;`
`list`      | `template <typename _Ty>               class list;`
`set`       | `template <typename _Ty>               class set;`
`map`       | `template <typename _Kt, typename _Vt> class map;`
`int`       | `class $int;`
`float`     | `class $float;`

#### Primitive Data Types
- `byte`  | `using byte;`
- `u8`    | `using u8;`
- `u16`   | `using u16;`
- `u32`   | `using u32;`
- `u64`   | `using u64;`
- `i8`    | `using i8;`
- `i16`   | `using i16;`
- `i32`   | `using i32;`
- `i64`   | `using i64;`
- `f32`   | `using f32;`
- `f64`   | `using f64;`
- `f80`   | `using f80;`
- `usize` | `using usize;`
- `isize` | `using isize;`

### Functions
#### Built-in Functions
- `print`   | `template <typename... Args> inline constexpr void print(Args &&...t)`
- `next`    | `template <LIBCXX_NAMESPACE::movable T> inline T next($generator<T> &gen)`

- `std::endl`      | `class endl`
- `std::to_string` | `template <typename _Ty>     constexpr string to_string(_Ty &&t)`
- `std::stringf`   | `template <typename... _Ty>  constexpr string stringf(string s, _Ty &&...t)`

- `std::move` | `template <class _Ty>  constexpr remove_ref_t<_Ty> &&move(_Ty &&_Arg) noexcept;`
- `std::forward`   | `template <typename T> T &&forward(typename remove_ref_t<T> &t) noexcept;`
- `std::forward`   | `template <typename T> T &&forward(typename remove_ref_t<T> &&t) noexcept;`

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

### Interfaces
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
- `Interfaces::ErrorHandle`