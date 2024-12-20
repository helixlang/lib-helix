// this contians functions related to the ABI of the Helix system
// Helix has a custom ABI that is used to interface with the Helix runtime
// the namemangling is done in a way that is different from the C++ ABI but can bind to C++ code

// name mangling for the Helix ABI:
// - the name of the function is mangled to the following format:
// _H1 - prefix for Helix ABI
// _F | _C | _S | _I | _V | _E | _O - (F)unction, (C)lass, (S)truct, (I)nterface, (V)ariable, (E)num, (O)perator
// _<name> - the name of the mangled thing
// _R<num of requires>_<num named> - the number of requires in the function: requires <T> => _R1_0, requires <T, U = int> => _R1_1
// now it gets special:
// _P<num of params>_<num of named> - the number of params in the function: 'fn foo(a: int, b: int) -> void' => _P2_0, 'fn foo(a: int, b: int = 0) -> void' => _P2_1
// _RT<type>_VR - the type of the function, this is the return type of the function, mangle the type and add it here
