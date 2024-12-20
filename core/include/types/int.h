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

#ifndef __$LIBHELIX_INT__
#define __$LIBHELIX_INT__

#include "../config.h"
#include "../libcxx.h"
#include "../libc.h"
#include "../refs.h"
#include "../primitives.h"

H_NAMESPACE_BEGIN

using byte   = LIBCXX_NAMESPACE::byte;
using string = LIBCXX_NAMESPACE::string;

H_STD_NAMESPACE_BEGIN

template <typename _Ty>
constexpr string to_string(_Ty &&t);  // forward declaration

H_STD_NAMESPACE_END
/**
 * @brief sized_int class
 *
 * @tparam _MaxSize `0` for dyn (heap allocated) and `n` for fixed size (stack allocated, unless n
 * exceeds stack limits)
 * @tparam _SlabSize Only needs to be changed for fine-grained slab control; defaults are
 * platform-dependent.
 * @tparam _ElemT The type of the elements in the slab (platform dependent).
 */
template <const usize _MaxSize, const usize _SlabSize = 16, typename _ElemT = usize>  // NOLINT
class sized_int {
    // Safe stack allocation threshold (platform dependent)
    static constexpr usize STACK_LIMIT = 1024;

    class alignas(sizeof(_ElemT)) SlabT {
      public:
        usize  size = 0;
        _ElemT data[_SlabSize]{};  // NOLINT
        SlabT *next = nullptr;

        static SlabT *_allocate_slab() { return new SlabT(); }  // NOLINT
        static void   _free_slab(SlabT *slab) { delete slab; }  // NOLINT
    };

    union Storage {
        _ElemT stackData[_MaxSize > STACK_LIMIT ? 1 : _MaxSize];  // Stack storage if _MaxSize <=
                                                                  // STACK_LIMIT
        SlabT *head;                                              // Head pointer for dynamic slabs

        Storage()
            : head(nullptr) {}  // Initialize as null by default
        ~Storage() {}           // Destruct only what's active
    } storage;

    SlabT *tail      = nullptr;
    usize  _size     = 0;
    usize  _capacity = (_MaxSize > 0 && _MaxSize <= STACK_LIMIT) ? _MaxSize : 0;

    #define IS_HEAP_ALLOCATED  (_MaxSize == 0 || _MaxSize  > STACK_LIMIT)
    #define IS_STACK_ALLOCATED (_MaxSize != 0 && _MaxSize <= STACK_LIMIT)

    bool _negative        = false;

    // Allocate storage based on mode
    void _allocate_slab() {
        if constexpr (IS_STACK_ALLOCATED) {
            return;  // No slabs for fixed stack allocation
        }

        auto *slab = SlabT::_allocate_slab();
        if (this->storage.head == nullptr) {
            this->storage.head = slab;
            this->tail         = slab;
        } else {
            this->tail->next = slab;
            this->tail       = slab;
        }

        this->_capacity += _SlabSize;
    }

    void _deallocate_slab() {
        if constexpr (IS_STACK_ALLOCATED || !this->storage.head) {
            return;
        }

        if (storage.head == this->tail) {
            SlabT::_free_slab(storage.head);
            storage.head = nullptr;
            this->tail   = nullptr;
        } else {
            auto *slab = storage.head;
            SlabT::_free_slab(this->tail);
            // Find new tail
            while (slab->next && slab->next != this->tail) {
                slab = slab->next;
            }
            this->tail       = slab;
            this->tail->next = nullptr;
        }
        this->_capacity -= _SlabSize;
    }

    void _resize_heap(usize n, _ElemT val) {
        if (n > this->_size) {  // Expand
            usize diff  = n - this->_size;
            usize slabs = (diff + _SlabSize - 1) / _SlabSize;  // Ceiling division
            for (usize i = 0; i < slabs; ++i) {
                _allocate_slab();
            }
        } else {  // Shrink
            usize diff  = this->_size - n;
            usize slabs = (diff + _SlabSize - 1) / _SlabSize;
            for (usize i = 0; i < slabs; ++i) {
                _deallocate_slab();
            }
        }

        if (this->tail) {  // Fill last slab with `val` if needed
            for (usize i = 0; i < _SlabSize; ++i) {
                this->tail->data[i] = val;
            }
        }

        this->_size = n;
    }

    void _resize_stack(usize n, _ElemT val) {
        if (n > _MaxSize) {
            raise(SIGSEGV);
            exit(139);
        }

        if (n > this->_size) {
            for (usize i = this->_size; i < n; ++i) {
                storage.stackData[i] = val;
            }
        }

        this->_size = n;
    }

  public:
    sized_int() = default;

    ~sized_int() {
        if constexpr (IS_HEAP_ALLOCATED) {
            auto *slab = storage.head;
            while (slab) {
                auto *next = slab->next;
                SlabT::_free_slab(slab);
                slab = next;
            }
        }
    }

    void resize(usize n, _ElemT val = 0) {
        if constexpr (IS_HEAP_ALLOCATED) {
            _resize_heap(n, val);
        } else {
            _resize_stack(n, val);
        }
    }

    _ElemT &operator[](usize index) {
        if constexpr (IS_HEAP_ALLOCATED) {
            auto *slab   = storage.head;
            usize offset = index;
            while (slab) {
                if (offset < _SlabSize) {
                    return slab->data[offset];
                }
                offset -= _SlabSize;
                slab = slab->next;
            }
        } else {
            if (index >= _MaxSize) {
                raise(SIGSEGV);
                exit(139);
            }
            return storage.stackData[index];
        }

        raise(SIGSEGV);
        exit(139);
    }
};

class $int : public sized_int<0> {
    /// this is the int class, this class is a class that is used to represent a dynamicly sized int
    // the internal data structure is a slab allocated linked list, where the head and tail are
    // tracked the slab is a fixed size array of 8 _ElemT integers, the slab is allocated on the
    // heap the slab is allocated in a linked list fashion, where each slab points to the next slab
    // this class is also SIMD optimized, allowing for very fast operations on the int class

    /// a few notes about this class:
    /// - the class has to be SIMD optimized and cache friendly and thread safe
    /// - the class has to be able to handle very large numbers (min up to 2^8496)
    /// - the class has to be able to handle very small numbers (min down to 0)
    /// - the class has to be able to handle negative numbers
    /// - the class should be made efficient for the most common use cases
    /// so lets say we have [[1231]] in the int calss, and now we want to make it 123134
    /// rather than reallocating the entire array or adding this as a new element, we can see the
    /// last slabs 0'th element has a size less then 64, so we can just add the number to that slab
    /// resulting in [[123134]] rather than [[1231, 34]] this is a lot more efficient, and allows
    /// for very fast operations on the int class
};

// overlaod allowing for 12789381i
$int operator""i(char const *s, size_t n) {
    $int result;
    for (usize i = 0; i < n; i++) {
        result;
    }
    return result;
}

H_NAMESPACE_END
#endif
