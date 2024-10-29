// SPDX-License-Identifier: MPL-2.0
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2024-2024 Seagate Technology LLC and/or its Affiliates, All
// Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
//
// \file memory_safety.h
// \brief Implements functions to assist with managing memory as safely as
// possible

#pragma once

#include "code_attributes.h"
#include "common_types.h"
#include "type_conversion.h"
#include <stdlib.h>
#include <string.h> //to pull in memset for anything that includes this file

#if defined(_WIN32)
#include "windows_version_detect.h" //for determining if securezeromemory2 is available and setting the pragma to include the required library.
#elif defined(POSIX_1990) || defined(BSD4_2)
#include <dirent.h>
#endif

#if defined(__cplusplus)
extern "C"
{
#endif

    // malloc in standards leaves malloc'ing size 0 as a undefined behavior.
    // this version will always return a null pointer if the size is zero
    M_FUNC_ATTR_MALLOC void* safe_malloc(size_t size);

    // avoiding undefined behavior allocing zero size and avoiding alloc'ing less
    // memory due to an overflow If alloc'ing zero or alloc would overflow size_t
    // from count * size, then return a null pointer
    M_FUNC_ATTR_MALLOC void* safe_calloc(size_t count, size_t size);

    // if passed a null pointer, behaves as safe_Malloc
    // if size is zero, will perform free and return NULL ptr
    M_FUNC_ATTR_MALLOC void* safe_realloc(void* block, size_t size);

    // if pointer to block is NULL, returns NULL
    // if pointer to block is passed as a null pointer, behaves as safe_Malloc
    // if size is zero, will perform free and return NULL ptr
    // if realloc fails, free's original block
    // free's original block if realloc fails
    M_FUNC_ATTR_MALLOC void* safe_reallocf(void** block, size_t size);

    //-----------------------------------------------------------------------------
    //
    //  safe_Free()
    //
    //! \brief   Description:  Safely free dynamically allocated memory. This checks
    //! for a valid pointer, then frees it and set's it to M_NULLPTR.
    //
    //  Entry:
    //!   \param[in] mem - pointer to heap memory you want to free. Uses double
    //!   pointer so that upon completion it will automatically be set to M_NULLPTR
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    static M_INLINE void safe_Free(void** mem)
    {
        if (mem && *mem)
        {
            free(*mem);
            (*mem) = M_NULLPTR;
        }
    }

    static M_INLINE void safe_Free_char(char** mem)
    {
        safe_Free(M_REINTERPRET_CAST(void**, mem));
    }

    static M_INLINE void safe_Free_schar(signed char** mem)
    {
        safe_Free(M_REINTERPRET_CAST(void**, mem));
    }

    static M_INLINE void safe_Free_uchar(unsigned char** mem)
    {
        safe_Free(M_REINTERPRET_CAST(void**, mem));
    }

    static M_INLINE void safe_Free_wchar(wchar_t** mem)
    {
        safe_Free(M_REINTERPRET_CAST(void**, mem));
    }

    static M_INLINE void safe_Free_short(signed short** mem)
    {
        safe_Free(M_REINTERPRET_CAST(void**, mem));
    }

    static M_INLINE void safe_Free_ushort(unsigned short** mem)
    {
        safe_Free(M_REINTERPRET_CAST(void**, mem));
    }

    static M_INLINE void safe_Free_int(signed int** mem)
    {
        safe_Free(M_REINTERPRET_CAST(void**, mem));
    }

    static M_INLINE void safe_Free_uint(unsigned int** mem)
    {
        safe_Free(M_REINTERPRET_CAST(void**, mem));
    }

    static M_INLINE void safe_Free_long(signed long** mem)
    {
        safe_Free(M_REINTERPRET_CAST(void**, mem));
    }

    static M_INLINE void safe_Free_ulong(unsigned long** mem)
    {
        safe_Free(M_REINTERPRET_CAST(void**, mem));
    }

    static M_INLINE void safe_Free_longlong(signed long long** mem)
    {
        safe_Free(M_REINTERPRET_CAST(void**, mem));
    }

    static M_INLINE void safe_Free_ulonglong(unsigned long long** mem)
    {
        safe_Free(M_REINTERPRET_CAST(void**, mem));
    }

    static M_INLINE void safe_Free_float(float** mem)
    {
        safe_Free(M_REINTERPRET_CAST(void**, mem));
    }

    static M_INLINE void safe_Free_double(double** mem)
    {
        safe_Free(M_REINTERPRET_CAST(void**, mem));
    }

    static M_INLINE void safe_Free_longdouble(long double** mem)
    {
        safe_Free(M_REINTERPRET_CAST(void**, mem));
    }

#if defined(USING_C11) && defined(HAVE_C11_GENERIC_SELECTION)
// This is a generic selection macro.
// so based on the type of X, it will call the appropriate function for that
// type. similar to an overloaded function in C++ for different types. NOTE: Not
// using intX_t or uintX_t since these are type defs to one of the types in the
// macro below. NOTE: No default case so we can get a warning when this doesn't
// expand correctly.
#define safe_free(mem)                                                                                                 \
    _Generic((mem), \
                        char**: safe_Free_char,\
                        unsigned char**: safe_Free_uchar,\
                        signed short**: safe_Free_short,\
                        unsigned short**: safe_Free_ushort,\
                        signed int**: safe_Free_int,\
                        unsigned int**: safe_Free_uint,\
                        signed long**: safe_Free_long,\
                        unsigned long**: safe_Free_ulong,\
                        signed long long**: safe_Free_longlong,\
                        unsigned long long**: safe_Free_ulonglong,\
                        float**: safe_Free_float,\
                        double**: safe_Free_double,\
                        long double**: safe_Free_longdouble,\
                        void **: safe_Free, \
                        default: safe_Free\
                        )(mem)
#elif !defined(__cplusplus)
#define safe_free(mem) safe_Free(M_REINTERPRET_CAST(void**, mem))
#endif // C11

#if defined(POSIX_1990) || defined(BSD4_2)
    // special function for struct dirent since it is used often in the libs
    static M_INLINE void safe_free_dirent(struct dirent** ent)
    {
        safe_Free(M_REINTERPRET_CAST(void**, ent));
    }
#endif

    //-----------------------------------------------------------------------------
    //
    //  is_Empty(void *ptrData, size_t lengthBytes)
    //
    //! \brief   Description:  Checks if the provided pointer is cleared to zeros
    //
    //  Entry:
    //!   \param[in] ptrData = pointer to a memory block to check if zeros
    //!   \param[in] lengthBytes = size in bytes of the ptr to check
    //!
    //  Exit:
    //!   \return true = memory is filled with zeros. false = memory has nonzero
    //!   values in it.
    //
    //-----------------------------------------------------------------------------
    bool is_Empty(const void* ptrData, size_t lengthBytes);

    //-----------------------------------------------------------------------------
    //
    //  errno_t safe_memset(void* dest, rsize_t destsz, int ch, rsize_t count)
    //
    //! \brief   Description: Works like memset_s from C11 annex K. This will check
    //! parameters for errors and write count chars to dest without being optimized
    //! away.
    //!                       the function explicit_zeroes may be better in some
    //!                       cases where you want to zero out memory, whereas this
    //!                       can be used to memset any character.
    //
    //  Entry:
    //!   \param[in] dest = pointer to memory to write
    //!   \param[in] destsz = size of dest in bytes
    //!   \param[in] ch = value to write to memory
    //!   \param[in] count = number of bytes to write
    //!
    //  Exit:
    //!   \return M_NULLPTR = error occurred otherwise returns pointer to dest
    //
    //-----------------------------------------------------------------------------
    errno_t safe_memset(void* dest, rsize_t destsz, int ch, rsize_t count);

    //-----------------------------------------------------------------------------
    //
    //  void* explicit_zeroes(void* dest, size_t count)
    //
    //! \brief   Description:  Write zeroes to memory pointer that will not be
    //! optimized out by the compiler.
    //
    //  Entry:
    //!   \param[in] dest = pointer to memory to write with zeroes. Must be
    //!   non-M_NULLPTR \param[in] count = number of bytes to write to zero
    //!
    //  Exit:
    //!   \return M_NULLPTR = error occurred otherwise returns pointer to dest
    //
    //-----------------------------------------------------------------------------
    void* explicit_zeroes(void* dest, size_t count);

#if defined(_MSC_VER) && !defined(NO_HAVE_MSFT_SECURE_ZERO_MEMORY2) &&                                                 \
    (defined(HAVE_MSFT_SECURE_ZERO_MEMORY2) ||                                                                         \
     (defined(WIN_API_TARGET_VERSION) && WIN_API_TARGET_VERSION >= WIN_API_TARGET_WIN11_26100))
#pragma comment(lib, "volatileaccessu.lib") // for VS to pick up this dependency when
                                            // SecureZeroMemory2 is available
#endif

    //-----------------------------------------------------------------------------
    //
    //  malloc_aligned(size_t size, size_t alignment)
    //
    //! \brief   Description:  Allocates aligned memory based on the specified power
    //! of 2 alignement value
    //
    //  Entry:
    //!   \param[in] size = size of memory block in bytes to allocate
    //!   \param[in] alignment = alignment value required. This MUST be a power
    //!   of 2.
    //!
    //  Exit:
    //!   \return ptrToAlignedMemory
    //
    //-----------------------------------------------------------------------------
    M_FUNC_ATTR_MALLOC void* malloc_aligned(size_t size, size_t alignment);

    //-----------------------------------------------------------------------------
    //
    //  free_aligned(void* ptr)
    //
    //! \brief   Description:  Deallocates memory that was allocated with one of
    //! malloc_aligned, calloc_aligned, or realloc_aligned
    //
    //  Entry:
    //!   \param[in] ptr = pointer to the aligned memory to free
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void free_aligned(void* ptr);

    //-----------------------------------------------------------------------------
    //
    //  safe_Free_aligned()
    //
    //! \brief   Description:  Safely free dynamically aligned allocated memory.
    //! This checks for a valid pointer, then frees it and set's it to M_NULLPTR.
    //
    //  Entry:
    //!   \param[in] mem - pointer to heap memory you want to free. Uses double
    //!   pointer so that upon completion it will automatically be set to M_NULLPTR
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    static M_INLINE void safe_Free_aligned(void** mem)
    {
        if (mem && *mem)
        {
            free_aligned(*mem);
            (*mem) = M_NULLPTR;
        }
    }

    static M_INLINE void safe_Free_aligned_char(char** mem)
    {
        safe_Free_aligned(M_REINTERPRET_CAST(void**, mem));
    }

    static M_INLINE void safe_Free_aligned_wchar(wchar_t** mem)
    {
        safe_Free_aligned(M_REINTERPRET_CAST(void**, mem));
    }

    static M_INLINE void safe_Free_aligned_schar(signed char** mem)
    {
        safe_Free_aligned(M_REINTERPRET_CAST(void**, mem));
    }

    static M_INLINE void safe_Free_aligned_uchar(unsigned char** mem)
    {
        safe_Free_aligned(M_REINTERPRET_CAST(void**, mem));
    }

    static M_INLINE void safe_Free_aligned_short(signed short** mem)
    {
        safe_Free_aligned(M_REINTERPRET_CAST(void**, mem));
    }

    static M_INLINE void safe_Free_aligned_ushort(unsigned short** mem)
    {
        safe_Free_aligned(M_REINTERPRET_CAST(void**, mem));
    }

    static M_INLINE void safe_Free_aligned_int(signed int** mem)
    {
        safe_Free_aligned(M_REINTERPRET_CAST(void**, mem));
    }

    static M_INLINE void safe_Free_aligned_uint(unsigned int** mem)
    {
        safe_Free_aligned(M_REINTERPRET_CAST(void**, mem));
    }

    static M_INLINE void safe_Free_aligned_long(signed long** mem)
    {
        safe_Free_aligned(M_REINTERPRET_CAST(void**, mem));
    }

    static M_INLINE void safe_Free_aligned_ulong(unsigned long** mem)
    {
        safe_Free_aligned(M_REINTERPRET_CAST(void**, mem));
    }

    static M_INLINE void safe_Free_aligned_longlong(signed long long** mem)
    {
        safe_Free_aligned(M_REINTERPRET_CAST(void**, mem));
    }

    static M_INLINE void safe_Free_aligned_ulonglong(unsigned long long** mem)
    {
        safe_Free_aligned(M_REINTERPRET_CAST(void**, mem));
    }

    static M_INLINE void safe_Free_aligned_float(float** mem)
    {
        safe_Free_aligned(M_REINTERPRET_CAST(void**, mem));
    }

    static M_INLINE void safe_Free_aligned_double(double** mem)
    {
        safe_Free_aligned(M_REINTERPRET_CAST(void**, mem));
    }

    static M_INLINE void safe_Free_aligned_longdouble(long double** mem)
    {
        safe_Free_aligned(M_REINTERPRET_CAST(void**, mem));
    }

#if defined(USING_C11) && defined(HAVE_C11_GENERIC_SELECTION)
// This is a generic selection macro.
// so based on the type of X, it will call the appropriate function for that
// type. similar to an overloaded function in C++ for different types. NOTE: Not
// using intX_t or uintX_t since these are type defs to one of the types in the
// macro below. NOTE: No default case so we can get a warning when this doesn't
// expand correctly.
#define safe_free_aligned(mem)                                                                                         \
    _Generic((mem), \
                        char**: safe_Free_aligned_char,\
                        unsigned char**: safe_Free_aligned_uchar,\
                        signed short**: safe_Free_aligned_short,\
                        unsigned short**: safe_Free_aligned_ushort,\
                        signed int**: safe_Free_aligned_int,\
                        unsigned int**: safe_Free_aligned_uint,\
                        signed long**: safe_Free_aligned_long,\
                        unsigned long**: safe_Free_aligned_ulong,\
                        signed long long**: safe_Free_aligned_longlong,\
                        unsigned long long**: safe_Free_aligned_ulonglong,\
                        float**: safe_Free_aligned_float,\
                        double**: safe_Free_aligned_double,\
                        long double**: safe_Free_aligned_longdouble,\
                        void **: safe_Free_aligned, \
                        default: safe_Free_aligned\
                        )(mem)
#elif !defined(__cplusplus)
#define safe_free_aligned(mem) safe_Free_aligned(M_REINTERPRET_CAST(void**, mem))
#endif // C11

    //-----------------------------------------------------------------------------
    //
    //  calloc_aligned(size_t num, size_t size, size_t alignment)
    //
    //! \brief   Description:  Allocates aligned memory based on the specified power
    //! of 2 alignement value and zeroes it out.
    //
    //  Entry:
    //!   \param[in] num = # of elements to allocate
    //!   \param[in] size = size of each element
    //!   \param[in] alignment = alignment value required. This MUST be a power
    //!   of 2.
    //!
    //  Exit:
    //!   \return ptrToAlignedMemory
    //
    //-----------------------------------------------------------------------------
    M_FUNC_ATTR_MALLOC void* calloc_aligned(size_t num, size_t size, size_t alignment);

    //-----------------------------------------------------------------------------
    //
    //  realloc_aligned(void *alignedPtr, size_t size, size_t alignment)
    //
    //! \brief   Description:  Reallocates aligned memory based on the specified
    //! power of 2 alignement value and zeroes it out.
    //
    //  Entry:
    //!   \param[in] alignedPtr = pointer to a memory block previously allocated
    //!   with malloc_aligned, calloc_aligned, or realloc_aligned. If M_NULLPTR,
    //!   this is the same as malloc_aligned \param[in] originalSize = size in bytes
    //!   of the alignedPtr being passed in. This is used so that previous data can
    //!   be preserved. Can be set to 0 if there is no care about the original data.
    //!   \param[in] size = size of memory block in bytes to allocate \param[in]
    //!   alignment = alignment value required. This MUST be a power of 2.
    //!
    //  Exit:
    //!   \return ptrToAlignedMemory
    //
    //-----------------------------------------------------------------------------
    M_FUNC_ATTR_MALLOC void* realloc_aligned(void* alignedPtr, size_t originalSize, size_t size, size_t alignment);

    // malloc in standards leaves malloc'ing size 0 as a undefined behavior.
    // this version will always return a null pointer if the size is zero
    M_FUNC_ATTR_MALLOC void* safe_malloc_aligned(size_t size, size_t alignment);

    // avoiding undefined behavior allocing zero size and avoiding alloc'ing less
    // memory due to an overflow If alloc'ing zero or alloc would overflow size_t
    // from count * size, then return a null pointer
    M_FUNC_ATTR_MALLOC void* safe_calloc_aligned(size_t count, size_t size, size_t alignment);

    // if passed a null pointer, behaves as safe_Malloc
    // if size is zero, will perform free and return NULL ptr
    M_FUNC_ATTR_MALLOC void* safe_realloc_aligned(void* block, size_t originalSize, size_t size, size_t alignment);

    // if pointer to block is NULL, returns NULL
    // if pointer to block is passed as a null pointer, behaves as safe_Malloc
    // if size is zero, will perform free and return NULL ptr
    // if realloc fails, free's original block
    // free's original block if realloc fails
    M_FUNC_ATTR_MALLOC void* safe_reallocf_aligned(void** block, size_t originalSize, size_t size, size_t alignment);

    //-----------------------------------------------------------------------------
    //
    //  get_System_Pagesize(void)
    //
    //! \brief   Description:  Gets the memory page size from a system if possible.
    //
    //  Entry:
    //!
    //  Exit:
    //!   \return Pagesize of system OR -1 if it cannot be determined.
    //
    //-----------------------------------------------------------------------------
    size_t get_System_Pagesize(void);

    //-----------------------------------------------------------------------------
    //
    //  malloc_page_aligned(size_t size)
    //
    //! \brief   Description:  Allocates aligned memory based on the system page
    //! size. Same as calling malloc_aligned(size, get_System_Pagesize())
    //
    //  Entry:
    //!   \param[in] size = size of memory block in bytes to allocate
    //!
    //  Exit:
    //!   \return ptrToAlignedMemory
    //
    //-----------------------------------------------------------------------------
    M_FUNC_ATTR_MALLOC void* malloc_page_aligned(size_t size);

    //-----------------------------------------------------------------------------
    //
    //  free_aligned(void* ptr)
    //
    //! \brief   Description:  Deallocates memory that was allocated with one of
    //! malloc_page_aligned, calloc_page_aligned, or realloc_page_aligned
    //
    //  Entry:
    //!   \param[in] ptr = pointer to the aligned memory to free
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    static M_INLINE void free_page_aligned(void* ptr)
    {
        free_aligned(ptr);
    }

    //-----------------------------------------------------------------------------
    //
    //  safe_Free_page_aligned()
    //
    //! \brief   Description:  Safely free dynamically page alligned allocated
    //! memory. This checks for a valid pointer, then frees it and set's it to
    //! M_NULLPTR.
    //
    //  Entry:
    //!   \param[in] mem - pointer to heap memory you want to free. Uses double
    //!   pointer so that upon completion it will automatically be set to M_NULLPTR
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    static M_INLINE void safe_Free_page_aligned(void** mem)
    {
        if (mem && *mem)
        {
            free_page_aligned(*mem);
            (*mem) = M_NULLPTR;
        }
    }

#if defined(USING_C11) && defined(HAVE_C11_GENERIC_SELECTION)
// This is a generic selection macro.
// so based on the type of X, it will call the appropriate function for that
// type. similar to an overloaded function in C++ for different types. NOTE: Not
// using intX_t or uintX_t since these are type defs to one of the types in the
// macro below. NOTE: No default case so we can get a warning when this doesn't
// expand correctly.
#define safe_free_page_aligned(mem)                                                                                    \
    _Generic((mem), \
                        char**: safe_Free_aligned_char,\
                        unsigned char**: safe_Free_aligned_uchar,\
                        signed short**: safe_Free_aligned_short,\
                        unsigned short**: safe_Free_aligned_ushort,\
                        signed int**: safe_Free_aligned_int,\
                        unsigned int**: safe_Free_aligned_uint,\
                        signed long**: safe_Free_aligned_long,\
                        unsigned long**: safe_Free_aligned_ulong,\
                        signed long long**: safe_Free_aligned_longlong,\
                        unsigned long long**: safe_Free_aligned_ulonglong,\
                        float**: safe_Free_aligned_float,\
                        double**: safe_Free_aligned_double,\
                        long double**: safe_Free_aligned_longdouble,\
                        void **: safe_Free_aligned, \
                        default: safe_Free_aligned\
                        )(mem)
#elif !defined(__cplusplus)
#define safe_free_page_aligned(mem) safe_Free_page_aligned(M_REINTERPRET_CAST(void**, mem))
#endif // C11

    //-----------------------------------------------------------------------------
    //
    //  calloc_page_aligned(size_t num, size_t size)
    //
    //! \brief   Description:  Allocates aligned memory based on the system page
    //! size. Same as calling calloc_aligned(num, size, get_System_Pagesize());
    //
    //  Entry:
    //!   \param[in] num = # of elements to allocate
    //!   \param[in] size = size of each element
    //!
    //  Exit:
    //!   \return ptrToAlignedMemory
    //
    //-----------------------------------------------------------------------------
    M_FUNC_ATTR_MALLOC void* calloc_page_aligned(size_t num, size_t size);

    //-----------------------------------------------------------------------------
    //
    //  realloc_page_aligned(void *alignedPtr, size_t size)
    //
    //! \brief   Description:  Reallocates aligned memory based on the system page
    //! size. Same as calling realloc_aligned(alignedPtr, size,
    //! get_System_Pagesize());
    //
    //  Entry:
    //!   \param[in] alignedPtr = pointer to a memory block previously allocated
    //!   with malloc_aligned, calloc_aligned, or realloc_aligned. If M_NULLPTR,
    //!   this is the same as malloc_aligned \param[in] originalSize = size in bytes
    //!   of the alignedPtr being passed in. This is used so that previous data can
    //!   be preserved. \param[in] size = size of memory block in bytes to allocate
    //!
    //  Exit:
    //!   \return ptrToAlignedMemory
    //
    //-----------------------------------------------------------------------------
    M_FUNC_ATTR_MALLOC void* realloc_page_aligned(void* alignedPtr, size_t originalSize, size_t size);

    // malloc in standards leaves malloc'ing size 0 as a undefined behavior.
    // this version will always return a null pointer if the size is zero
    M_FUNC_ATTR_MALLOC void* safe_malloc_page_aligned(size_t size);

    // avoiding undefined behavior allocing zero size and avoiding alloc'ing less
    // memory due to an overflow If alloc'ing zero or alloc would overflow size_t
    // from count * size, then return a null pointer
    M_FUNC_ATTR_MALLOC void* safe_calloc_page_aligned(size_t count, size_t size);

    // if passed a null pointer, behaves as safe_Malloc
    // if size is zero, will perform free and return NULL ptr
    M_FUNC_ATTR_MALLOC void* safe_realloc_page_aligned(void* block, size_t originalSize, size_t size);

    // if pointer to block is NULL, returns NULL
    // if pointer to block is passed as a null pointer, behaves as safe_Malloc
    // if size is zero, will perform free and return NULL ptr
    // if realloc fails, free's original block
    // free's original block if realloc fails
    M_FUNC_ATTR_MALLOC void* safe_reallocf_page_aligned(void** block, size_t originalSize, size_t size);

    // can be called to determine if memory regions overlap or not.
    // used inside safe_memcpy and other functions that do not support/allow
    // overlapped memory regions. returns zero when not overlapping otherwise
    // returns a non-zero value.
    M_INLINE int memory_regions_overlap(const void* M_RESTRICT ptr1,
                                        rsize_t                size1,
                                        const void* M_RESTRICT ptr2,
                                        rsize_t                size2)
    {
        // casting a null pointer to uintptr_t results in a zero. This should be
        // safe.
        return ((M_REINTERPRET_CAST(intptr_t, ptr1) -
                 (M_REINTERPRET_CAST(intptr_t, ptr2) + M_STATIC_CAST(intptr_t, size2))) &
                (M_REINTERPRET_CAST(intptr_t, ptr2) -
                 (M_REINTERPRET_CAST(intptr_t, ptr1) + M_STATIC_CAST(intptr_t, size1)))) < 0;
    }

    // Calls memmove_s if available, otherwise performs all checks that memmove_s
    // does before calling memmove
    errno_t safe_memmove(void* dest, rsize_t destsz, const void* src, rsize_t count);

    // calls memcpy_s if available, otherwise performs all checks that memcpy_s does
    // before calling memcpy
    errno_t safe_memcpy(void* M_RESTRICT dest, rsize_t destsz, const void* M_RESTRICT src, rsize_t count);

    // bounds checked version of memccpy
    errno_t safe_memccpy(void* M_RESTRICT dest, rsize_t destsz, const void* M_RESTRICT src, int c, rsize_t count);

    // bounds checked version of memccpy that allows overlapping ranges (like
    // memmove does)
    errno_t safe_memcmove(void* M_RESTRICT dest, rsize_t destsz, const void* M_RESTRICT src, int c, rsize_t count);

#if defined(__cplusplus)
}

#if !defined(safe_free)
template <typename T> M_INLINE void safe_free(T** mem)
{
    safe_Free(reinterpret_cast<void**>(mem));
}
#endif // safe_free

#if !defined(safe_free_aligned)
template <typename T> M_INLINE void safe_free_aligned(T** mem)
{
    safe_Free_aligned(reinterpret_cast<void**>(mem));
}
#endif // safe_free_aligned

#if !defined(safe_free_page_aligned)
template <typename T> M_INLINE void safe_free_page_aligned(T** mem)
{
    safe_Free_aligned(reinterpret_cast<void**>(mem));
}
#endif // safe_free_page_aligned

#endif
