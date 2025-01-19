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
/// Ideology: make sure this is highly optimized, a basic string must have the following grantees:
/// 1. const char*'s that point to the ROM of the file must not be moved to the heap unless modifed
///    even then to optimize the performance of strings, the allocation model should follow that of
///    a slab allocator, unless the string is truncated. In the case of trunaction the whole ROM
///    string must be moved to the heap and resized accordingly.
///
/// 2. char*'s that point to heap memory should be copied to a new location on the heap owned by
///    'string' instead of the original owner, this is to avoid any issue of dangling pointers or
///    use after frees
///
/// 3. string class maintains a record of the size of the string, in the case of an operation that
///    requires the O(n) operarion of incrementing though the entire string, there is an implicit
///    null char check to validate the string size matches the size of the internal pointer.
///
/// 4. string concat operations between 2 strings, just point the pointer of the last string to the
///    data of the new string (this is to avoid reallocation, while allowing full functionality)

#ifndef __$LIBHELIX_STRING__
#define __$LIBHELIX_STRING__

#include "../config.h"
#include "../libcxx.h"
#include "../libc.h"
#include "../memory.h"
#include "../primitives.h"

#include <string>


H_NAMESPACE_BEGIN

template <typename CharT>
class char_traits {

};

/// \class string_slice
/// \brief A string slice is a view into a string, it does not own the data, this can also be set
///        to read-only char pointers
/// \tparam _ElemT The element type of the string
/// \tparam _Size The size of the string slice (this is not the size of the string but the size of
///         the slice itself like a substring)
template <typename CharT>
class slice { // string::slice
    const CharT *m_data;
    usize        m_size;
};

template <typename CharT, const usize _SlabSize = 16>  // NOLINT
class basic {
    
};

H_NAMESPACE_END
#endif