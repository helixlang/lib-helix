## This Document outlines all the built-in functions that are available in the Helix language exposed directly by the compiler.

## Built-in Macros
- `builtins::file!()`: Returns the name of the file in which it is called.
- `builtins::line!()`: Returns the line number in which it is called.
- `builtins::column!()`: Returns the column number in which it is called (at `c`).
- `builtins::function!()`: Returns the name of the function in which it is called.
- `builtins::module!()`: Returns the name of the module in which it is called.
- `builtins::library!()`: Returns the name of the library in which it is called.
- `types::assigner_t!()`: Returns the type of the assigner. `let x: int = foo();` if `builtins::assigner_t!()` is called in `foo`, it will return `int`.
- `

Non-moduled macros:
- `typeof!()`: Returns the type of the expression.
- `sizeof!()`: Returns the size of the type in bytes.
- `alignof!()`: Returns the alignment of the type in bytes.
- `offsetof!()`: Returns the offset of the field in the struct.
- 