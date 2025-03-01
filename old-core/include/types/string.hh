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
/// Ideology: make sure this is highly optimized, a basic_string string must have the following grantees:
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

#include <csetjmp>
#include "../config.h"
#include "../interfaces.h"
#include "../lang/generator.hh"
#include "../libc.h"
#include "../libcxx.h"
#include "../memory.h"
#include "../meta.h"
#include "../primitives.h"
#include "forward.hh"
#include "slice.hh"

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN
namespace String {
template <typename T = wchar_t>
class basic_string {
  private:
    static constexpr size_t SBO_SIZE = 24;
    using str_t = LIBCXX_NAMESPACE::basic_string<T>;

    union {
        struct {
            T sbo_buffer[SBO_SIZE];  // Small Buffer Optimization
        };
        struct {
            T     *heap_buffer;
            size_t capacity;
        };
    };

    size_t size;
    u8     flags;

  public:
    using slice = basic_slice<T>;

    // --- Constructors ---
    constexpr basic_string() noexcept
        : size(0)
        , flags(0) {
        sbo_buffer[0] = '\0';
    }

    constexpr basic_string(const basic_string &other) noexcept { assign(other); }

    constexpr basic_string(basic_string &&other) noexcept { move_assign(std::Memory::move(other)); }

    constexpr explicit basic_string(const T *str) noexcept { assign(str); }

    constexpr explicit basic_string(const T *str, size_t len) noexcept { assign(str, len); }

    constexpr explicit basic_string(const str_t &str) noexcept { assign(str.c_str(), str.size()); }
    constexpr explicit basic_string(const slice &str) noexcept { assign(str.raw(), str.size()); }

    constexpr ~basic_string() {
        if (!is_sbo()) {
            delete[] heap_buffer;
        }
    }

    constexpr basic_string &operator=(const basic_string &other) noexcept {
        if (this != &other) {
            assign(other);
        }
        return *this;
    }

    constexpr basic_string &operator=(basic_string &&other) noexcept {
        if (this != &other) {
            move_assign(std::Memory::move(other));
        }
        return *this;
    }

    // --- Internal Helpers ---
  private:
    [[nodiscard]] constexpr bool is_sbo() const noexcept { return (flags & 1) == 0; }

    constexpr void assign(const T *str, size_t len = 0) noexcept {
        if (len == 0) {
            len = std::String::length(str);
        }
        this->size = len;

        if (len < SBO_SIZE) {
            std::Memory::copy(sbo_buffer, str, (len + 1) * sizeof(T));
            flags &= ~1;  // Mark as SBO
        } else {
            heap_buffer = new T[len + 1];
            std::Memory::copy(heap_buffer, str, (len + 1) * sizeof(T));
            capacity = len;
            flags |= 1;  // Mark as Heap
        }
    }

    constexpr void assign(const basic_string &other) noexcept { assign(other.raw(), other.size); }

    constexpr void move_assign(basic_string &&other) noexcept {
        std::Memory::copy(this, &other, sizeof(basic_string));
        other.flags = 0;
        other.size  = 0;
    }

    constexpr void ensure_mutable() {
        if ((flags & 2) == 0) {
            return;  // Not ROM
        }
        assign(raw(), size);  // Copy to mutable storage
    }

  public:
    // --- Data Access ---
    [[nodiscard]] constexpr size_t   length() const noexcept { return size; }
    [[nodiscard]] constexpr bool     empty() const noexcept { return size == 0; }
    [[nodiscard]] constexpr const T *raw() const noexcept {
        return is_sbo() ? sbo_buffer : heap_buffer;
    }

    // --- Mutation ---
    constexpr void append(const T *str) {
        ensure_mutable();
        size_t len = std::String::length(str);
        if (size + len < SBO_SIZE) {
            std::Memory::copy(sbo_buffer + size, str, (len + 1) * sizeof(T));
        } else {
            size_t new_size   = size + len;
            T     *new_buffer = new T[new_size + 1];
            std::Memory::copy(new_buffer, raw(), size * sizeof(T));
            std::Memory::copy(new_buffer + size, str, (len + 1) * sizeof(T));
            if (!is_sbo())
                delete[] heap_buffer;
            heap_buffer = new_buffer;
            capacity    = new_size;
            flags |= 1;  // Heap flag
        }
        size += len;
    }

    constexpr void clear() {
        size          = 0;
        sbo_buffer[0] = '\0';
        flags &= ~1;
    }

    // --- String Operations ---
    [[nodiscard]] constexpr basic_string<T> to_upper() const noexcept {
        basic_string<T> result(*this);
        for (auto &c : result) {
            c =
            #ifndef _MSC_VER
                LIBCXX_NAMESPACE::towupper(c)
            #else
                towupper(c)
            #endif
            ;
        }

        return result;
    }

    /*
    [[nodiscard]] constexpr basic upper() const {
        basic result(*this);
        for (size_t i = 0; i < result.size; ++i) {
            result.sbo_buffer[i] = LIBCXX_NAMESPACE::towupper(result.sbo_buffer[i]);
        }
        return result;
    }

    [[nodiscard]] constexpr basic lower() const {
        basic result(*this);
        for (size_t i = 0; i < result.size; ++i) {
            result.sbo_buffer[i] = LIBCXX_NAMESPACE::towlower(result.sbo_buffer[i]);
        }
        return result;
    }
    */

    [[nodiscard]] constexpr basic_string<T> to_lower() const noexcept {
        basic_string<T> result(*this);
        for (auto &c : result) {
            c =
            #ifndef _MSC_VER
                LIBCXX_NAMESPACE::towlower(c)
            #else
                towupper(c)
            #endif
            ;
        }

        return result;
    }

    [[nodiscard]] constexpr basic_string strip() const {
        size_t start = 0;
        size_t end   = size;
        while (start < end && LIBCXX_NAMESPACE::iswspace(raw()[start])) {
            ++start;
        }
        while (end > start && LIBCXX_NAMESPACE::iswspace(raw()[end - 1])) {
            --end;
        }
        return basic_string(raw() + start, end - start);
    }

    [[nodiscard]] constexpr basic_string<T> replace(T old_c, T new_c) const noexcept {
        LIBCXX_NAMESPACE::wstring result(raw().begin(), raw().end());
        for (auto &c : result) {
            if (c == old_c) {
                c = new_c;
            }
        }

        return basic_string<T>(result);
    }

    [[nodiscard]] constexpr std::Questionable<size_t> find(T c) const noexcept {
        for (size_t i = 0; i < size; ++i) {
            if (raw()[i] == c) {
                return i;
            }
        }
        return null;
    }

    // --- Operators ---
    [[nodiscard]] constexpr std::Questionable<T> operator[](size_t i) const noexcept {
        if (i >= size) {
            return null;
        }
        return raw()[i];
    }

    [[nodiscard]] constexpr bool operator==(const basic_string &other) const noexcept {
        return size == other.size && std::Memory::copy(raw(), other.raw(), size * sizeof(T)) == 0;
    }

    [[nodiscard]] constexpr bool operator!=(const basic_string &other) const noexcept {
        return !(*this == other);
    }

    [[nodiscard]] constexpr bool operator<(const basic_string &other) const noexcept {
        return std::String::compare(raw(), other.raw(), LIBCXX_NAMESPACE::min(size, other.size)) <
               0;
    }

    [[nodiscard]] constexpr bool operator>(const basic_string &other) const noexcept {
        return std::String::compare(raw(), other.raw(), LIBCXX_NAMESPACE::min(size, other.size)) >
               0;
    }
};
}  // namespace String


template <>
class String::basic_string<wchar_t> {
  public:
    basic_string(const char *rom_ptr);
    basic_string(char *rom_ptr);
    basic_string(basic_string<char> rom_ptr);

    basic_string operator=(const char *);
    basic_string operator+(const char *);
    basic_string operator+=(const char *);

    basic_string operator=(char *);
    basic_string operator+(char *);
    basic_string operator+=(char *);

    basic_string operator=(basic_string<char>);
    basic_string operator+(basic_string<char>);
    basic_string operator+=(basic_string<char>);
};

H_STD_NAMESPACE_END

using string = std::String::basic_string<wchar_t>;

H_NAMESPACE_END
#endif
