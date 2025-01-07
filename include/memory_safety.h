// SPDX-License-Identifier: MPL-2.0

//! \file memory_safety.h
//! \brief Implements functions to assist with managing memory safely and mitigate common
//! errors.
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2024 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "code_attributes.h"
#include "common_types.h"
#include "impl_memory_safety.h"
#include "type_conversion.h"

#include <stdlib.h>
#include <string.h> //to pull in memset for anything that includes this file
#include <wchar.h>

#if defined(_WIN32)
#    include "windows_version_detect.h" //for determining if securezeromemory2 is available and setting the pragma to include the required library.
#elif defined(POSIX_1990) || defined(BSD4_2)
#    include <dirent.h>
#endif

#if defined(__cplusplus)
extern "C"
{
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn M_FUNC_ATTR_MALLOC void* safe_malloc(size_t size)
    //! \brief Allocates memory with bounds checking.
    //!
    //! This function allocates \a size bytes of memory, with bounds checking to guard against allocating memory with a
    //! size of zero.
    //!
    //! \param[in] size The size of the memory block to allocate.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return A pointer to the allocated memory block, or NULL on failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a size is zero
    M_INLINE M_FUNC_ATTR_MALLOC void* safe_malloc(size_t size)
    {
        return safe_malloc_impl(size, __FILE__, __func__, __LINE__, "safe_malloc(size)");
    }
#else
//! \def safe_malloc(size)
//! \brief Allocates memory with bounds checking.
//!
//! This function allocates \a size bytes of memory, with bounds checking to guard against allocating memory with a
//! size of zero.
//!
//! \param[in] size The size of the memory block to allocate.
//! \param[in] file The source file name where this function is called.
//! \param[in] function The function name where this function is called.
//! \param[in] line The line number where this function is called.
//! \param[in] expression The expression being evaluated.
//! \return A pointer to the allocated memory block, or NULL on failure.
//!
//! \note The following errors are detected at runtime and call the installed constraint handler:
//!
//! - \a size is zero
#    define safe_malloc(size) safe_malloc_impl(size, __FILE__, __func__, __LINE__, "safe_malloc(" #    size ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn M_FUNC_ATTR_MALLOC void* safe_calloc_impl(size_t count, size_t size)
    //! \brief Allocates memory for an array with bounds checking.
    //!
    //! This function allocates memory for an array of \a count elements of \a size bytes each, with bounds checking to
    //! guard against an overflow in the \a count and \a size arguments.
    //!
    //! \param[in] count The number of elements to allocate.
    //! \param[in] size The size of each element.
    //! \return A pointer to the allocated memory block, or NULL on failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a count or \a size is zero
    //!
    //! - \a count * \a size results in an overflow
    M_INLINE M_FUNC_ATTR_MALLOC void* safe_calloc(size_t count, size_t size)
    {
        return safe_calloc_impl(count, size, __FILE__, __func__, __LINE__, "safe_calloc(count, size)");
    }
#else
//! \def safe_calloc(size_t count, size_t size)
//! \brief Allocates memory for an array with bounds checking.
//!
//! This function allocates memory for an array of \a count elements of \a size bytes each, with bounds checking to
//! guard against an overflow in the \a count and \a size arguments.
//!
//! \param[in] count The number of elements to allocate.
//! \param[in] size The size of each element.
//! \return A pointer to the allocated memory block, or NULL on failure.
//!
//! \note The following errors are detected at runtime and call the installed constraint handler:
//!
//! - \a count or \a size is zero
//!
//! - \a count * \a size results in an overflow
#    define safe_calloc(count, size)                                                                                   \
        safe_calloc_impl(count, size, __FILE__, __func__, __LINE__, "safe_calloc(" #count ", " #size ")")
#endif

    //! \fn M_FUNC_ATTR_MALLOC void* safe_realloc(void* block, size_t size)
    //! \brief allocates or reallocates memory pointed to by \a block
    //! \param[in] block pointer to existing memory block. If null this is the same as safe_malloc()
    //! \param[in] size number of bytes to allocate/reallocate to. If zero, free's memory pointed to by \a block
    //! \return pointer to allocated memory to be free'd by the caller with free()
    //! returns a null pointer on failure.
    M_FUNC_ATTR_MALLOC void* safe_realloc(void* block, size_t size);

    //! \fn M_FUNC_ATTR_MALLOC void* safe_reallocf(void** block, size_t size)
    //! \brief allocates or reallocates memory pointed to by \a block
    //! If reallocation fails, frees the original memory block.
    //! \details
    //! if pointer to block is NULL, returns NULL
    //! if pointer to block is passed as a null pointer, behaves as safe_Malloc
    //! if size is zero, will perform free and return NULL ptr
    //! if realloc fails, free's original block
    //! free's original block if realloc fails
    //! \param[in,out] block pointer to existing memory block.
    //! \param[in] size number of bytes to allocate/reallocate to. If zero, free's memory pointed to by \a block
    //! \return pointer to allocated memory to be free'd by the caller with free()
    //! returns a null pointer on failure.
    M_FUNC_ATTR_MALLOC void* safe_reallocf(void** block, size_t size);

    //! \fn void safe_free_core(void** mem)
    //! \brief Safely free dynamically allocated memory. This checks
    //! for a valid pointer, then frees it and sets it to M_NULLPTR.
    //! by setting it to null pointer after freeing this reduces the likelihood
    //! of a double-free error.
    //! \param[in,out] mem pointer to heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    static M_INLINE void safe_free_core(void** mem)
    {
        if (mem && *mem)
        {
            free(*mem);
            (*mem) = M_NULLPTR;
        }
    }

    //! \fn void safe_free_core_char(char** mem)
    //! \brief Helper function for safely freeing char* memory
    //! \param[in,out] mem pointer to heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! char* memory = safe_malloc(50);
    //!
    //! ...
    //!
    //! safe_free_char(&memory);
    //! \endcode
    static M_INLINE void safe_free_char(char** mem)
    {
        safe_free_core(M_REINTERPRET_CAST(void**, mem));
    }

    //! \fn void safe_free_schar(signed char** mem)
    //! \brief Helper function for safely freeing signed char* memory
    //! \param[in,out] mem pointer to heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! signed char* memory = safe_malloc(50);
    //!
    //! ...
    //!
    //! safe_free_schar(&memory);
    //! \endcode
    static M_INLINE void safe_free_schar(signed char** mem)
    {
        safe_free_core(M_REINTERPRET_CAST(void**, mem));
    }

    //! \fn void safe_free_uchar(unsigned char** mem)
    //! \brief Helper function for safely freeing unsigned char* memory
    //! \param[in,out] mem pointer to heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! unsigned char* memory = safe_malloc(50);
    //!
    //! ...
    //!
    //! safe_free_uchar(&memory);
    //! \endcode
    static M_INLINE void safe_free_uchar(unsigned char** mem)
    {
        safe_free_core(M_REINTERPRET_CAST(void**, mem));
    }

    //! \fn void safe_free_wchar(wchar_t** mem)
    //! \brief Helper function for safely freeing wchar_t* memory
    //! \param[in,out] mem pointer to heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! wchar_t* memory = safe_malloc(50);
    //!
    //! ...
    //!
    //! safe_free_wchar(&memory);
    //! \endcode
    static M_INLINE void safe_free_wchar(wchar_t** mem)
    {
        safe_free_core(M_REINTERPRET_CAST(void**, mem));
    }

    //! \fn void safe_free_short(signed short** mem)
    //! \brief Helper function for safely freeing signed short* memory
    //! \param[in,out] mem pointer to heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! signed short* memory = safe_malloc(50);
    //!
    //! ...
    //!
    //! safe_free_short(&memory);
    //! \endcode
    static M_INLINE void safe_free_short(signed short** mem)
    {
        safe_free_core(M_REINTERPRET_CAST(void**, mem));
    }

    //! \fn void safe_free_ushort(unsigned short** mem)
    //! \brief Helper function for safely freeing unsigned short* memory
    //! \param[in,out] mem pointer to heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! unsigned short* memory = safe_malloc(50);
    //!
    //! ...
    //!
    //! safe_free_ushort(&memory);
    //! \endcode
    static M_INLINE void safe_free_ushort(unsigned short** mem)
    {
        safe_free_core(M_REINTERPRET_CAST(void**, mem));
    }

    //! \fn void safe_free_int(signed int** mem)
    //! \brief Helper function for safely freeing signed int* memory
    //! \param[in,out] mem pointer to heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! signed int* memory = safe_malloc(50);
    //!
    //! ...
    //!
    //! safe_free_int(&memory);
    //! \endcode
    static M_INLINE void safe_free_int(signed int** mem)
    {
        safe_free_core(M_REINTERPRET_CAST(void**, mem));
    }

    //! \fn void safe_free_uint(unsigned int** mem)
    //! \brief Helper function for safely freeing unsigned int* memory
    //! \param[in,out] mem pointer to heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! unsigned int* memory = safe_malloc(50);
    //!
    //! ...
    //!
    //! safe_free_uint(&memory);
    //! \endcode
    static M_INLINE void safe_free_uint(unsigned int** mem)
    {
        safe_free_core(M_REINTERPRET_CAST(void**, mem));
    }

    //! \fn void safe_free_long(signed long** mem)
    //! \brief Helper function for safely freeing signed long* memory
    //! \param[in,out] mem pointer to heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! signed long* memory = safe_malloc(50);
    //!
    //! ...
    //!
    //! safe_free_long(&memory);
    //! \endcode
    static M_INLINE void safe_free_long(signed long** mem)
    {
        safe_free_core(M_REINTERPRET_CAST(void**, mem));
    }

    //! \fn void safe_free_ulong(unsigned long** mem)
    //! \brief Helper function for safely freeing unsigned long* memory
    //! \param[in,out] mem pointer to heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! unsigned long* memory = safe_malloc(50);
    //!
    //! ...
    //!
    //! safe_free_ulong(&memory);
    //! \endcode
    static M_INLINE void safe_free_ulong(unsigned long** mem)
    {
        safe_free_core(M_REINTERPRET_CAST(void**, mem));
    }

    //! \fn void safe_free_longlong(signed long long** mem)
    //! \brief Helper function for safely freeing signed long long* memory
    //! \param[in,out] mem pointer to heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! signed long long* memory = safe_malloc(50);
    //!
    //! ...
    //!
    //! safe_free_longlong(&memory);
    //! \endcode
    static M_INLINE void safe_free_longlong(signed long long** mem)
    {
        safe_free_core(M_REINTERPRET_CAST(void**, mem));
    }

    //! \fn void safe_free_ulonglong(unsigned long long** mem)
    //! \brief Helper function for safely freeing unsigned long long* memory
    //! \param[in,out] mem pointer to heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! unsigned long long* memory = safe_malloc(50);
    //!
    //! ...
    //!
    //! safe_free_ulonglong(&memory);
    //! \endcode
    static M_INLINE void safe_free_ulonglong(unsigned long long** mem)
    {
        safe_free_core(M_REINTERPRET_CAST(void**, mem));
    }

    //! \fn void safe_free_float(float** mem)
    //! \brief Helper function for safely freeing float* memory
    //! \param[in,out] mem pointer to heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! float* memory = safe_malloc(50);
    //!
    //! ...
    //!
    //! safe_free_float(&memory);
    //! \endcode
    static M_INLINE void safe_free_float(float** mem)
    {
        safe_free_core(M_REINTERPRET_CAST(void**, mem));
    }

    //! \fn void safe_free_double(double** mem)
    //! \brief Helper function for safely freeing double* memory
    //! \param[in,out] mem pointer to heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! double* memory = safe_malloc(50);
    //!
    //! ...
    //!
    //! safe_free_double(&memory);
    //! \endcode
    static M_INLINE void safe_free_double(double** mem)
    {
        safe_free_core(M_REINTERPRET_CAST(void**, mem));
    }

    //! \fn void safe_free_longdouble(long double** mem)
    //! \brief Helper function for safely freeing long double* memory
    //! \param[in,out] mem pointer to heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! long double* memory = safe_malloc(50);
    //!
    //! ...
    //!
    //! safe_free_longdouble(&memory);
    //! \endcode
    static M_INLINE void safe_free_longdouble(long double** mem)
    {
        safe_free_core(M_REINTERPRET_CAST(void**, mem));
    }

#if defined(USING_C11) && defined(HAVE_C11_GENERIC_SELECTION)
// clang-format off
//! \def safe_free(mem)
//! \brief Generic selection macro to select the correct helper function
//! for safely freeing memory for the type of \a mem
//! \param[in,out] mem pointer to heap memory you want to free. Uses double
//! pointer so that upon completion it will be set to M_NULLPTR
//! \code
//! type* memory = safe_malloc(50);
//!
//! ...
//!
//! safe_free(&memory);
//! \endcode
#define safe_free(mem)                                                                                                 \
    _Generic((mem),                                                                                                    \
             char**                  : safe_free_char,                                                                 \
             unsigned char**         : safe_free_uchar,                                                                \
             signed short**          : safe_free_short,                                                                \
             unsigned short**        : safe_free_ushort,                                                               \
             signed int**            : safe_free_int,                                                                  \
             unsigned int**          : safe_free_uint,                                                                 \
             signed long**           : safe_free_long,                                                                 \
             unsigned long**         : safe_free_ulong,                                                                \
             signed long long**      : safe_free_longlong,                                                             \
             unsigned long long**    : safe_free_ulonglong,                                                            \
             float**                 : safe_free_float,                                                                \
             double**                : safe_free_double,                                                               \
             long double**           : safe_free_longdouble,                                                           \
             void **                 : safe_free_core,                                                                 \
             default                 : safe_free_core                                                                  \
            )(mem)
// clang-format on
#elif !defined(__cplusplus)
//! \def safe_free(mem)
//! \brief Simple helper macro for calling safe_free_core without conversion warnings.
//! \warning Due to using a cast inside this version, be careful passing the
//! correct pointer to this macro!
//! The C11 generic version is more type safe for this and should be used instead
//! when C11 generic selection is available
//! \param[in,out] mem pointer to heap memory you want to free. Uses double
//! pointer so that upon completion it will be set to M_NULLPTR
//! \code
//! type* memory = safe_malloc(50);
//!
//! ...
//!
//! safe_free(&memory);
//! \endcode
#    define safe_free(mem) safe_free_core(M_REINTERPRET_CAST(void**, mem))
#endif // C11

#if defined(_WIN32)
    //! \fn void safe_free_tchar(TCHAR** mem)
    //! \brief Helper function for safely freeing TCHAR memory
    //! TCHAR's underlying type changes depending on the UNICODE
    //! macro in Windows. This will handle either version correctly
    //! \param[in,out] mem pointer to heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! TCHAR* memory = safe_malloc(50);
    //!
    //! ...
    //!
    //! safe_free_tchar(&memory);
    //! \endcode
    static M_INLINE void safe_free_tchar(TCHAR** str)
    {
        safe_free_core(M_REINTERPRET_CAST(void**, str));
    }
#endif

#if defined(POSIX_1990) || defined(BSD4_2)
    //! \fn void safe_free_dirent(struct dirent** mem)
    //! \brief Helper function for safely freeing struct dirent
    //! NOTE: This is good for individual structures.
    //! \param[in,out] mem pointer to heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    static M_INLINE void safe_free_dirent(struct dirent** ent)
    {
        safe_free_core(M_REINTERPRET_CAST(void**, ent));
    }

    static M_INLINE void safe_free_dirent_list(struct dirent*** list)
    {
        if (list)
        {
            safe_free_core(M_REINTERPRET_CAST(void**, &list));
        }
    }
#endif

    //! \fn bool is_Empty(const void* ptrData, size_t lengthBytes)
    //! \brief   checks if a memory block is set to zeros
    //
    //! \param[in] ptrData pointer to a memory block to check if zeros
    //! \param[in] lengthBytes size in bytes of \a ptrData to check if equal to zero
    //! \return true = memory is filled with zeros. false = memory has nonzero
    //! values in it.
    bool is_Empty(const void* ptrData, size_t lengthBytes);

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_memset(void* dest, rsize_t destsz, int ch, rsize_t count)
    //! \brief Sets a block of memory to a specified value.
    //
    //! This function is similar to memset_s in C11 annex K. It sets the first \a count bytes of the block of memory
    //! pointed to by \a dest to the specified value \a ch.
    //!
    //! \param[out] dest Pointer to the block of memory to fill.
    //! \param[in] destsz Size of the destination buffer.
    //! \param[in] ch Value to be set.
    //! \param[in] count Number of bytes to be set to the value.
    //! \return Zero on success, or an error code on failure.
    //!
    //! \note The following errors are detected at runtime and call the currently installed constraint handler function
    //! after storing \a ch in every location of the destination range [\a dest, \a dest + \a destsz) if \a dest and
    //! \a destsz are themselves valid:
    //!
    //! - \a dest is a null pointer
    //!
    //! - \a destsz or \a count is greater than \a RSIZE_MAX
    //!
    //! - \a count is greater than \a destsz (buffer overflow would occur)
    //!
    //! The behavior is undefined if the size of the character array pointed to by \a dest < \a count <= \a destsz; in
    //! other words, an erroneous value of \a destsz does not expose the impending buffer overflow.
    M_INLINE errno_t safe_memset(void* dest, rsize_t destsz, int ch, rsize_t count)
    {
        return safe_memset_impl(dest, destsz, ch, count, __FILE__, __func__, __LINE__,
                                "safe_memset(dest, destsz, ch, count)");
    }
#else
//! \def safe_memset(dest, destsz, ch, count)
//! \brief Sets a block of memory to a specified value using a macro.
//
//! This macro is similar to memset_s in C11 annex K. It sets the first \a count bytes of the block of memory
//! pointed to by \a dest to the specified value \a ch. This macro passes __FILE__, __func__, __LINE__ to
//! The internal implementation to provide more debug information at compile time to the constraint handler
//!
//! \param[out] dest Pointer to the block of memory to fill.
//! \param[in] destsz Size of the destination buffer.
//! \param[in] ch Value to be set.
//! \param[in] count Number of bytes to be set to the value.
//! \return Zero on success, or an error code on failure.
//!
//! \note The following errors are detected at runtime and call the currently installed constraint handler function
//! after storing \a ch in every location of the destination range [\a dest, \a dest + \a destsz) if \a dest and
//! \a destsz are themselves valid:
//!
//! - \a dest is a null pointer
//!
//! - \a destsz or \a count is greater than \a RSIZE_MAX
//!
//! - \a count is greater than \a destsz (buffer overflow would occur)
//!
//! The behavior is undefined if the size of the character array pointed to by \a dest < \a count <= \a destsz; in
//! other words, an erroneous value of \a destsz does not expose the impending buffer overflow.
#    define safe_memset(dest, destsz, ch, count)                                                                       \
        safe_memset_impl(dest, destsz, ch, count, __FILE__, __func__, __LINE__,                                        \
                         "safe_memset(" #dest ", " #destsz ", " #ch ", " #count ")")
#endif

    //! \fn void* explicit_zeroes(void* dest, size_t count)
    //! \brief Writes zeroes to a block of memory. Will not be optimized out.
    //
    //! This function is useful for erasing sensitive data from memory when it is no longer needed.
    //!
    //! \param[in] dest Pointer to the block of memory to zero fill. Must be non-NULL
    //! \param[in] count Number of bytes to write to zero at \a dest
    //! \return pointer to \a dest on Success and a NULL pointer on failure
    void* explicit_zeroes(void* dest, size_t count);

    // This ugly ifdef is to handle newer SecureZeroMemory2 support and visual studio can automatically detect this
    // dependency this way - TJE
#if defined(_MSC_VER) && !defined(NO_HAVE_MSFT_SECURE_ZERO_MEMORY2) &&                                                 \
    (defined(HAVE_MSFT_SECURE_ZERO_MEMORY2) ||                                                                         \
     (defined(WIN_API_TARGET_VERSION) && WIN_API_TARGET_VERSION >= WIN_API_TARGET_WIN11_26100))
#    pragma comment(lib, "volatileaccessu.lib") // for VS to pick up this dependency when
                                                // SecureZeroMemory2 is available
#endif

    //! \fn  M_FUNC_ATTR_MALLOC void* malloc_aligned(size_t size, size_t alignment)
    //! \brief Allocates aligned memory based on the specified power of 2 alignment value
    //!
    //! Different OS's/environments handle allocating aligned memory differently.
    //! This is a wrapper function around all those differences for a single, simple API call.
    //! \param[in] size size of memory block in bytes to allocate
    //! \param[in] alignment alignment value required. This MUST be a power of 2.
    //! \return pointer to aligned memory on success. null pointer on failure.
    //! the caller must free this memory using free_aligned().
    M_FUNC_ATTR_MALLOC void* malloc_aligned(size_t size, size_t alignment);

    //! \brief Frees memory that was allocated with one of malloc_aligned, calloc_aligned, or realloc_aligned
    //! \param[in] ptr pointer to the aligned memory to free
    //!
    //! Different OS's/environments handle freeing aligned memory differently.
    //! This is a wrapper function around all those differences for a single, simple API call.
    void free_aligned(void* ptr);

    //! \fn void safe_free_aligned_core(void** mem)
    //! \brief Safely free dynamically allocated memory that was aligned at allocation. This checks
    //! for a valid pointer, then frees it and sets it to M_NULLPTR.
    //! by setting it to null pointer after freeing this reduces the likelihood
    //! of a double-free error.
    //! \param[in,out] mem pointer to heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    static M_INLINE void safe_free_aligned_core(void** mem)
    {
        if (mem && *mem)
        {
            free_aligned(*mem);
            (*mem) = M_NULLPTR;
        }
    }

    //! \fn void safe_free_aligned_char(char** mem)
    //! \brief Helper function for safely freeing aligned char* memory
    //! \param[in,out] mem pointer to aligned heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! char* memory = safe_malloc_aligned(50, alignment);
    //!
    //! ...
    //!
    //! safe_free_aligned_char(&memory);
    //! \endcode
    static M_INLINE void safe_free_aligned_char(char** mem)
    {
        safe_free_aligned_core(M_REINTERPRET_CAST(void**, mem));
    }

    //! \fn void safe_free_aligned_wchar(wchar_t** mem)
    //! \brief Helper function for safely freeing aligned wchar_t* memory
    //! \param[in,out] mem pointer to aligned heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! wchar_t* memory = safe_malloc_aligned(50, alignment);
    //!
    //! ...
    //!
    //! safe_free_aligned_wchar(&memory);
    //! \endcode
    static M_INLINE void safe_free_aligned_wchar(wchar_t** mem)
    {
        safe_free_aligned_core(M_REINTERPRET_CAST(void**, mem));
    }

    //! \fn void safe_free_aligned_schar(signed char** mem)
    //! \brief Helper function for safely freeing aligned signed char* memory
    //! \param[in,out] mem pointer to aligned heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! signed char* memory = safe_malloc_aligned(50, alignment);
    //!
    //! ...
    //!
    //! safe_free_aligned_schar(&memory);
    //! \endcode
    static M_INLINE void safe_free_aligned_schar(signed char** mem)
    {
        safe_free_aligned_core(M_REINTERPRET_CAST(void**, mem));
    }

    //! \fn void safe_free_aligned_uchar(unsigned char** mem)
    //! \brief Helper function for safely freeing aligned unsigned char* memory
    //! \param[in,out] mem pointer to aligned heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! unsigned char* memory = safe_malloc_aligned(50, alignment);
    //!
    //! ...
    //!
    //! safe_free_aligned_uchar(&memory);
    //! \endcode
    static M_INLINE void safe_free_aligned_uchar(unsigned char** mem)
    {
        safe_free_aligned_core(M_REINTERPRET_CAST(void**, mem));
    }

    //! \fn void safe_free_aligned_short(signed short** mem)
    //! \brief Helper function for safely freeing aligned signed short* memory
    //! \param[in,out] mem pointer to aligned heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! signed short* memory = safe_malloc_aligned(50, alignment);
    //!
    //! ...
    //!
    //! safe_free_aligned_short(&memory);
    //! \endcode
    static M_INLINE void safe_free_aligned_short(signed short** mem)
    {
        safe_free_aligned_core(M_REINTERPRET_CAST(void**, mem));
    }

    //! \fn void safe_free_aligned_ushort(unsigned short** mem)
    //! \brief Helper function for safely freeing aligned unsigned short* memory
    //! \param[in,out] mem pointer to aligned heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! unsigned short* memory = safe_malloc_aligned(50, alignment);
    //!
    //! ...
    //!
    //! safe_free_aligned_ushort(&memory);
    //! \endcode
    static M_INLINE void safe_free_aligned_ushort(unsigned short** mem)
    {
        safe_free_aligned_core(M_REINTERPRET_CAST(void**, mem));
    }

    //! \fn void safe_free_aligned_int(signed int** mem)
    //! \brief Helper function for safely freeing aligned signed int* memory
    //! \param[in,out] mem pointer to aligned heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! signed int* memory = safe_malloc_aligned(50, alignment);
    //!
    //! ...
    //!
    //! safe_free_aligned_int(&memory);
    //! \endcode
    static M_INLINE void safe_free_aligned_int(signed int** mem)
    {
        safe_free_aligned_core(M_REINTERPRET_CAST(void**, mem));
    }

    //! \fn void safe_free_aligned_uint(unsigned int** mem)
    //! \brief Helper function for safely freeing aligned unsigned int* memory
    //! \param[in,out] mem pointer to aligned heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! unsigned int* memory = safe_malloc_aligned(50, alignment);
    //!
    //! ...
    //!
    //! safe_free_aligned_uint(&memory);
    //! \endcode
    static M_INLINE void safe_free_aligned_uint(unsigned int** mem)
    {
        safe_free_aligned_core(M_REINTERPRET_CAST(void**, mem));
    }

    //! \fn void safe_free_aligned_long(signed long** mem)
    //! \brief Helper function for safely freeing aligned signed long* memory
    //! \param[in,out] mem pointer to aligned heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! signed long* memory = safe_malloc_aligned(50, alignment);
    //!
    //! ...
    //!
    //! safe_free_aligned_long(&memory);
    //! \endcode
    static M_INLINE void safe_free_aligned_long(signed long** mem)
    {
        safe_free_aligned_core(M_REINTERPRET_CAST(void**, mem));
    }

    //! \fn void safe_free_aligned_ulong(unsigned long** mem)
    //! \brief Helper function for safely freeing aligned unsigned long* memory
    //! \param[in,out] mem pointer to aligned heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! unsigned long* memory = safe_malloc_aligned(50, alignment);
    //!
    //! ...
    //!
    //! safe_free_aligned_ulong(&memory);
    //! \endcode
    static M_INLINE void safe_free_aligned_ulong(unsigned long** mem)
    {
        safe_free_aligned_core(M_REINTERPRET_CAST(void**, mem));
    }

    //! \fn void safe_free_aligned_longlong(signed long long** mem)
    //! \brief Helper function for safely freeing aligned signed long long* memory
    //! \param[in,out] mem pointer to aligned heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! signed long long* memory = safe_malloc_aligned(50, alignment);
    //!
    //! ...
    //!
    //! safe_free_aligned_longlong(&memory);
    //! \endcode
    static M_INLINE void safe_free_aligned_longlong(signed long long** mem)
    {
        safe_free_aligned_core(M_REINTERPRET_CAST(void**, mem));
    }

    //! \fn void safe_free_aligned_ulonglong(unsigned long long** mem)
    //! \brief Helper function for safely freeing aligned unsigned long long* memory
    //! \param[in,out] mem pointer to aligned heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! unsigned long long* memory = safe_malloc_aligned(50, alignment);
    //!
    //! ...
    //!
    //! safe_free_aligned_ulonglong(&memory);
    //! \endcode
    static M_INLINE void safe_free_aligned_ulonglong(unsigned long long** mem)
    {
        safe_free_aligned_core(M_REINTERPRET_CAST(void**, mem));
    }

    //! \fn void safe_free_aligned_float(float** mem)
    //! \brief Helper function for safely freeing aligned float* memory
    //! \param[in,out] mem pointer to aligned heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! float* memory = safe_malloc_aligned(50, alignment);
    //!
    //! ...
    //!
    //! safe_free_aligned_float(&memory);
    //! \endcode
    static M_INLINE void safe_free_aligned_float(float** mem)
    {
        safe_free_aligned_core(M_REINTERPRET_CAST(void**, mem));
    }

    //! \fn void safe_free_aligned_double(double** mem)
    //! \brief Helper function for safely freeing aligned double* memory
    //! \param[in,out] mem pointer to aligned heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! double* memory = safe_malloc_aligned(50, alignment);
    //!
    //! ...
    //!
    //! safe_free_aligned_double(&memory);
    //! \endcode
    static M_INLINE void safe_free_aligned_double(double** mem)
    {
        safe_free_aligned_core(M_REINTERPRET_CAST(void**, mem));
    }

    //! \fn void safe_free_aligned_longdouble(long double** mem)
    //! \brief Helper function for safely freeing aligned long double* memory
    //! \param[in,out] mem pointer to aligned heap memory you want to free. Uses double
    //! pointer so that upon completion it will be set to M_NULLPTR
    //! \code
    //! long double* memory = safe_malloc_aligned(50, alignment);
    //!
    //! ...
    //!
    //! safe_free_aligned_longdouble(&memory);
    //! \endcode
    static M_INLINE void safe_free_aligned_longdouble(long double** mem)
    {
        safe_free_aligned_core(M_REINTERPRET_CAST(void**, mem));
    }

#if defined(USING_C11) && defined(HAVE_C11_GENERIC_SELECTION)
// clang-format off
//! \def safe_free_aligned(mem)
//! \brief Generic selection macro to select the correct helper function
//! for safely freeing aligned memory for the type of \a mem
//! \param[in,out] mem pointer to aligned heap memory you want to free. Uses double
//! pointer so that upon completion it will be set to M_NULLPTR
//! \code
//! type* memory = safe_malloc_aligned(50, alignment);
//!
//! ...
//!
//! safe_free_aligned(&memory);
//! \endcode
#define safe_free_aligned(mem)                                                                                         \
    _Generic((mem),                                                                                                    \
             char**                 : safe_free_aligned_char,                                                          \
             unsigned char**        : safe_free_aligned_uchar,                                                         \
             signed short**         : safe_free_aligned_short,                                                         \
             unsigned short**       : safe_free_aligned_ushort,                                                        \
             signed int**           : safe_free_aligned_int,                                                           \
             unsigned int**         : safe_free_aligned_uint,                                                          \
             signed long**          : safe_free_aligned_long,                                                          \
             unsigned long**        : safe_free_aligned_ulong,                                                         \
             signed long long**     : safe_free_aligned_longlong,                                                      \
             unsigned long long**   : safe_free_aligned_ulonglong,                                                     \
             float**                : safe_free_aligned_float,                                                         \
             double**               : safe_free_aligned_double,                                                        \
             long double**          : safe_free_aligned_longdouble,                                                    \
             void **                : safe_free_aligned_core,                                                          \
             default                : safe_free_aligned_core                                                           \
            )(mem)
// clang-format on
#elif !defined(__cplusplus)
//! \def safe_free_aligned(mem)
//! \brief Simple helper macro for calling safe_free_aligned without conversion warnings.
//! \warning Due to using a cast inside this version, be careful passing the
//! correct pointer to this macro!
//! The C11 generic version is more type safe for this and should be used instead
//! when C11 generic selection is available
//! \param[in,out] mem pointer to aligned heap memory you want to free. Uses double
//! pointer so that upon completion it will be set to M_NULLPTR
//! \code
//! type* memory = safe_malloc_aligned(50, alignment);
//!
//! ...
//!
//! safe_free_aligned(&memory);
//! \endcode
#    define safe_free_aligned(mem) safe_free_aligned_core(M_REINTERPRET_CAST(void**, mem))
#endif // C11

    //! \fn M_FUNC_ATTR_MALLOC void* calloc_aligned(size_t num, size_t size, size_t alignment)
    //! \brief Allocates aligned memory based on the specified power of 2 alignment value and zeroes it out.
    //! \param[in] num # of elements to allocate
    //! \param[in] size size of each element
    //! \param[in] alignment alignment value required. This MUST be a power of 2.
    //! \return pointer to aligned memory on success, nullpointer on failure.
    M_FUNC_ATTR_MALLOC void* calloc_aligned(size_t num, size_t size, size_t alignment);

    //! \brief Allocates/Reallocates aligned memory based on the specified
    //! power of 2 alignment value provided. Original aligned pointer is free'd upon success.
    //! \param[in] alignedPtr pointer to a memory block previously allocated
    //! with malloc_aligned, calloc_aligned, or realloc_aligned. If M_NULLPTR,
    //! this is the same as malloc_aligned
    //! \param[in] originalSize size in bytes of the alignedPtr being passed in.
    //! This is used so that previous data can be preserved. Can be set to 0 to discard original data.
    //! \param[in] size size of memory block in bytes to allocate
    //! \param[in] alignment alignment value required. This MUST be a power of 2.
    //! \return Pointer to aligned memory on success, null pointer on failure.
    M_FUNC_ATTR_MALLOC void* realloc_aligned(void* alignedPtr, size_t originalSize, size_t size, size_t alignment);

#if defined(DEV_ENVIRONMENT)
    //! \fn M_FUNC_ATTR_MALLOC void* safe_malloc_aligned(size_t size, size_t alignment))
    //! \brief Allocates aligned memory with bounds checking.
    //!
    //! This function allocates \a size bytes of memory aligned to \a alignment, with bounds checking to guard against
    //! allocating memory with a size of zero.
    //!
    //! \param[in] size The size of the memory block to allocate.
    //! \param[in] alignment The alignment of the memory block.
    //! \return A pointer to the allocated memory block, or NULL on failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a size is zero
    M_INLINE M_FUNC_ATTR_MALLOC void* safe_malloc_aligned(size_t size, size_t alignment)
    {
        return safe_malloc_aligned_impl(size, alignment, __FILE__, __func__, __LINE__,
                                        "safe_malloc_aligned(size, alignment)");
    }
#else
//! \def safe_malloc_aligned(size_t size, size_t alignment)
//! \brief Allocates aligned memory with bounds checking.
//!
//! This function allocates \a size bytes of memory aligned to \a alignment, with bounds checking to guard against
//! allocating memory with a size of zero.
//!
//! \param[in] size The size of the memory block to allocate.
//! \param[in] alignment The alignment of the memory block.
//! \return A pointer to the allocated memory block, or NULL on failure.
//!
//! \note The following errors are detected at runtime and call the installed constraint handler:
//!
//! - \a size is zero
#    define safe_malloc_aligned(size, alignment)                                                                       \
        safe_malloc_aligned_impl(size, alignment, __FILE__, __func__, __LINE__,                                        \
                                 "safe_malloc_aligned(" #size ", " #alignment ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn M_FUNC_ATTR_MALLOC void* safe_calloc_aligned_impl(size_t count, size_t size, size_t alignment)
    //! \brief Allocates aligned memory for an array with bounds checking.
    //!
    //! This function allocates memory for an array of \a count elements of \a size bytes each, aligned to \a alignment,
    //! with bounds checking to guard against an overflow in the \a count and \a size arguments.
    //!
    //! \param[in] count The number of elements to allocate.
    //! \param[in] size The size of each element.
    //! \param[in] alignment The alignment of the memory block.
    //! \return A pointer to the allocated memory block, or NULL on failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a count or \a size is zero
    //!
    //! - \a count * \a size results in an overflow
    M_INLINE M_FUNC_ATTR_MALLOC void* safe_calloc_aligned(size_t count, size_t size, size_t alignment)
    {
        return safe_calloc_aligned_impl(count, size, alignment, __FILE__, __func__, __LINE__,
                                        "safe_calloc_aligned(count, size, alignment)");
    }
#else
//! \def safe_calloc_aligned(size_t count, size_t size, size_t alignment)
//! \brief Allocates aligned memory for an array with bounds checking.
//!
//! This function allocates memory for an array of \a count elements of \a size bytes each, aligned to \a alignment,
//! with bounds checking to guard against an overflow in the \a count and \a size arguments.
//!
//! \param[in] count The number of elements to allocate.
//! \param[in] size The size of each element.
//! \param[in] alignment The alignment of the memory block.
//! \return A pointer to the allocated memory block, or NULL on failure.
//!
//! \note The following errors are detected at runtime and call the installed constraint handler:
//!
//! - \a count or \a size is zero
//!
//! - \a count * \a size results in an overflow
#    define safe_calloc_aligned(count, size, alignment)                                                                \
        safe_calloc_aligned_impl(count, size, alignment, __FILE__, __func__, __LINE__,                                 \
                                 "safe_calloc_aligned(" #count ", " #size ", " #alignment ")")
#endif

    //! \fn M_FUNC_ATTR_MALLOC void* safe_realloc_aligned(void* block,
    //!                                                   size_t originalSize,
    //!                                                   size_t size,
    //!                                                   size_t alignment)
    //! \brief allocates or reallocates memory pointed to by \a block
    //! \param[in] block pointer to existing memory block. If null this is the same as safe_malloc()
    //! \param[in] originalSize original number of bytes allocated in \a block. Can be zero if \a block is NULL or
    //! original contents of \a block do not need to be preserved during reallocation
    //! \param[in] size number of bytes to allocate/reallocate to. If zero, free's memory pointed to by \a block
    //! \param[in] alignment requested alignment of the memory allocation. Will be rounded up to the nearest power of
    //! two. \return pointer to allocated memory to be free'd by the caller with free() returns a null pointer on
    //! failure.
    M_FUNC_ATTR_MALLOC void* safe_realloc_aligned(void* block, size_t originalSize, size_t size, size_t alignment);

    //! \fn M_FUNC_ATTR_MALLOC void* safe_reallocf(void** block, size_t size)
    //! \brief allocates or reallocates memory pointed to by \a block
    //! If reallocation fails, frees the original memory block.
    //! \details
    //! if pointer to block is NULL, returns NULL
    //! if pointer to block is passed as a null pointer, behaves as safe_Malloc
    //! if size is zero, will perform free and return NULL ptr
    //! if realloc fails, free's original block
    //! free's original block if realloc fails
    //! \param[in,out] block pointer to existing memory block. If null, new memory is allocated.
    //! \param[in] originalSize original number of bytes allocated in \a block. Can be zero if \a block is NULL or
    //! original contents of \a block do not need to be preserved during reallocation
    //! \param[in] size number of bytes to allocate/reallocate to. If zero, free's memory pointed to by \a block
    //! \param[in] alignment requested alignment of the memory allocation. Will be rounded up to the nearest power of
    //! two. \return pointer to allocated memory to be free'd by the caller with free() returns a null pointer on
    //! failure.
    M_FUNC_ATTR_MALLOC void* safe_reallocf_aligned(void** block, size_t originalSize, size_t size, size_t alignment);

    //! \fn size_t get_System_Pagesize(void)
    //! \brief Gets the memory page size from a system for the current CPU (often 4096B)
    //! \return Pagesize of system. Returns 4096 as default safe value if cannot be determined.
    size_t get_System_Pagesize(void);

    //! \fn M_FUNC_ATTR_MALLOC void* malloc_page_aligned(size_t size)
    //! \brief allocates page aligned memory.
    //!
    //! This is a convenience wrapper around malloc_aligned
    //! \param[in] size number of bytes to allocate
    //! \return
    M_FUNC_ATTR_MALLOC void* malloc_page_aligned(size_t size);

    //! \fn M_INLINE void free_page_aligned(void* ptr)
    //! \brief convenience function around free_aligned.
    static M_INLINE void free_page_aligned(void* ptr)
    {
        free_aligned(ptr);
    }

    //! \fn M_INLINE void safe_free_page_aligned_core(void** mem)
    //! \brief convenience function around free_aligned.
    static M_INLINE void safe_free_page_aligned_core(void** mem)
    {
        if (mem && *mem)
        {
            free_page_aligned(*mem);
            (*mem) = M_NULLPTR;
        }
    }

#if defined(USING_C11) && defined(HAVE_C11_GENERIC_SELECTION)
// clang-format off
//! \def safe_free_page_aligned(mem)
//! \brief Generic selection macro to select the correct helper function
//! for safely freeing page aligned memory for the type of \a mem
//! \param[in,out] mem pointer to aligned heap memory you want to free. Uses double
//! pointer so that upon completion it will be set to M_NULLPTR
//! \code
//! type* memory = safe_malloc_page_aligned(50, alignment);
//!
//! ...
//!
//! safe_free_page_aligned(&memory);
//! \endcode
#define safe_free_page_aligned(mem)                                                                                    \
    _Generic((mem),                                                                                                    \
             char**                 : safe_free_aligned_char,                                                          \
             unsigned char**        : safe_free_aligned_uchar,                                                         \
             signed short**         : safe_free_aligned_short,                                                         \
             unsigned short**       : safe_free_aligned_ushort,                                                        \
             signed int**           : safe_free_aligned_int,                                                           \
             unsigned int**         : safe_free_aligned_uint,                                                          \
             signed long**          : safe_free_aligned_long,                                                          \
             unsigned long**        : safe_free_aligned_ulong,                                                         \
             signed long long**     : safe_free_aligned_longlong,                                                      \
             unsigned long long**   : safe_free_aligned_ulonglong,                                                     \
             float**                : safe_free_aligned_float,                                                         \
             double**               : safe_free_aligned_double,                                                        \
             long double**          : safe_free_aligned_longdouble,                                                    \
             void **                : safe_free_aligned_core,                                                          \
             default                : safe_free_aligned_core                                                           \
            )(mem)
// clang-format on
#elif !defined(__cplusplus)
//! \def safe_free_page_aligned(mem)
//! \brief Simple helper macro for calling safe_free_aligned without conversion warnings.
//! \warning Due to using a cast inside this version, be careful passing the
//! correct pointer to this macro!
//! The C11 generic version is more type safe for this and should be used instead
//! when C11 generic selection is available
//! \param[in,out] mem pointer to aligned heap memory you want to free. Uses double
//! pointer so that upon completion it will be set to M_NULLPTR
//! \code
//! type* memory = safe_malloc_page_aligned(50, alignment);
//!
//! ...
//!
//! safe_free_alsafe_free_page_alignedigned(&memory);
#    define safe_free_page_aligned(mem) safe_free_page_aligned(M_REINTERPRET_CAST(void**, mem))
#endif // C11

    //! \fn M_FUNC_ATTR_MALLOC void* calloc_page_aligned(size_t num, size_t size)
    //! \brief Allocated zeroed memory with memory page alignment
    //!
    //! \param[in] count The number of elements to allocate.
    //! \param[in] size The size of each element.
    //! \return A pointer to the allocated memory block, or NULL on failure.
    M_FUNC_ATTR_MALLOC void* calloc_page_aligned(size_t num, size_t size);

    //! \fn M_FUNC_ATTR_MALLOC void* realloc_page_aligned(void* alignedPtr, size_t originalSize, size_t size)
    //! \brief Allocates or reallocated a memory page aligned block of memory.
    //!
    //! \param[in] alignedPtr pointer to block or memory to reallocate. If null, allocates new memory
    //! as if calling malloc_page_aligned.
    //! \param[in] originalSize Original size of memory pointed to by \a alignedPtr. If zero original buffer may
    //! not be preserved.
    //! \param[in] size The size of each element.
    //! \return A pointer to the allocated memory block, or NULL on failure.
    M_FUNC_ATTR_MALLOC void* realloc_page_aligned(void* alignedPtr, size_t originalSize, size_t size);

    //! \fn M_FUNC_ATTR_MALLOC void* safe_malloc_page_aligned(size_t size)
    //! \brief Allocates aligned memory with bounds checking.
    //!
    //! This function allocates \a size bytes of memory aligned to a memory page, with bounds checking to guard against
    //! allocating memory with a size of zero.
    //!
    //! \param[in] size The size of the memory block to allocate.
    //! \return A pointer to the allocated memory block, or NULL on failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a size is zero
    M_FUNC_ATTR_MALLOC void* safe_malloc_page_aligned(size_t size);

    //! \fn M_FUNC_ATTR_MALLOC void* safe_calloc_page_aligned(size_t count, size_t size)
    //! \brief Allocates page aligned memory for an array with bounds checking.
    //!
    //! This function allocates memory for an array of \a count elements of \a size bytes each, aligned to a memory
    //! page, with bounds checking to guard against an overflow in the \a count and \a size arguments.
    //!
    //! \param[in] count The number of elements to allocate.
    //! \param[in] size The size of each element.
    //! \return A pointer to the allocated memory block, or NULL on failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a count or \a size is zero
    //!
    //! - \a count * \a size results in an overflow
    M_FUNC_ATTR_MALLOC void* safe_calloc_page_aligned(size_t count, size_t size);

    //! \fn M_FUNC_ATTR_MALLOC void* safe_realloc_page_aligned(void* block, size_t originalSize, size_t size)
    //! \brief Allocates or reallocated a memory page aligned block of memory. Performs extra bounds checking
    //! such as what is found in safe_malloc.
    //!
    //! \param[in] block pointer to block or memory to reallocate. If null, allocates new memory
    //! as if calling malloc_page_aligned.
    //! \param[in] originalSize Original size of memory pointed to by \a alignedPtr. If zero original buffer may
    //! not be preserved.
    //! \param[in] size The size of each element.
    //! \return A pointer to the allocated memory block, or NULL on failure.
    M_FUNC_ATTR_MALLOC void* safe_realloc_page_aligned(void* block, size_t originalSize, size_t size);

    //! \fn M_FUNC_ATTR_MALLOC void* safe_reallocf_page_aligned(void** block, size_t originalSize, size_t size)
    //! \brief Allocates or reallocated a memory page aligned block of memory. Performs extra bounds checking
    //! such as what is found in safe_malloc. If memory allocation fails, original block is freed.
    //!
    //! \param[in] block double pointer to block or memory to reallocate. Must pass a non-null pointer.
    //! If pointer points to null, allocates new memory as if calling malloc_page_aligned.
    //! \param[in] originalSize Original size of memory pointed to by \a alignedPtr. If zero original buffer may
    //! not be preserved.
    //! \param[in] size The size of each element.
    //! \return A pointer to the allocated memory block, or NULL on failure.
    M_FUNC_ATTR_MALLOC void* safe_reallocf_page_aligned(void** block, size_t originalSize, size_t size);

    //! \fn static M_INLINE int memory_regions_overlap(const void* M_RESTRICT ptr1,
    //!                                                rsize_t                size1,
    //!                                                const void* M_RESTRICT ptr2,
    //!                                                rsize_t                size2)
    //! \brief Checks if two memory regions overlap.
    //! \param[in] ptr1 pointer to the first memory region to check
    //! \param[in] size1 size of the first memory region
    //! \param[in] ptr2 pointer to the second memory region to check
    //! \param[in] size2 size of the second memory region
    //! \return 0 regions do not overlap. Nonzero means the regions overlap
    static M_INLINE int memory_regions_overlap(const void* M_RESTRICT ptr1,
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

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_memmove(void* dest, rsize_t destsz, const void* src, rsize_t count)
    //! \brief Moves a block of memory with bounds checking.
    //!
    //! This function moves \a count bytes from the block of memory pointed to by \a src to the block of memory pointed
    //! to by \a dest, with bounds checking as specified in C11 annex K.
    //!
    //! \param[out] dest Pointer to the destination block of memory.
    //! \param[in] destsz Size of the destination buffer.
    //! \param[in] src Pointer to the source block of memory.
    //! \param[in] count Number of bytes to move.
    //! \return Zero on success, or an error code on failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a dest is a null pointer
    //!
    //! - \a src is a null pointer
    //!
    //! - \a destsz or \a count is greater than \a RSIZE_MAX
    //!
    //! - \a count is greater than \a destsz (buffer overflow would occur)
    //!
    //! The behavior is undefined if the size of the character array pointed to by \a dest < \a count <= \a destsz; in
    //! other words, an erroneous value of \a destsz does not expose the impending buffer overflow.
    M_INLINE errno_t safe_memmove(void* dest, rsize_t destsz, const void* src, rsize_t count)
    {
        return safe_memmove_impl(dest, destsz, src, count, __FILE__, __func__, __LINE__,
                                 "safe_memmove(dest, destsz, src, count)");
    }
#else
//! \def safe_memmove(dest, destsz, src, count)
//! \brief Moves a block of memory with bounds checking.
//!
//! This function moves \a count bytes from the block of memory pointed to by \a src to the block of memory pointed
//! to by \a dest, with bounds checking as specified in C11 annex K.
//!
//! \param[out] dest Pointer to the destination block of memory.
//! \param[in] destsz Size of the destination buffer.
//! \param[in] src Pointer to the source block of memory.
//! \param[in] count Number of bytes to move.
//! \return Zero on success, or an error code on failure.
//!
//! \note The following errors are detected at runtime and call the installed constraint handler:
//!
//! - \a dest is a null pointer
//!
//! - \a src is a null pointer
//!
//! - \a destsz or \a count is greater than \a RSIZE_MAX
//!
//! - \a count is greater than \a destsz (buffer overflow would occur)
//!
//! The behavior is undefined if the size of the character array pointed to by \a dest < \a count <= \a destsz; in
//! other words, an erroneous value of \a destsz does not expose the impending buffer overflow.
#    define safe_memmove(dest, destsz, src, count)                                                                     \
        safe_memmove_impl(dest, destsz, src, count, __FILE__, __func__, __LINE__,                                      \
                          "safe_memmove(" #dest ", " #destsz ", " #src ", " #count ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_memcpy(void* M_RESTRICT dest, rsize_t destsz, const void* M_RESTRICT src, rsize_t count)
    //! \brief Copies a block of memory with bounds checking.
    //!
    //! This function copies \a count bytes from the block of memory pointed to by \a src to the block of memory pointed
    //! to by \a dest, with bounds checking as specified in C11 annex K.
    //!
    //! \param[out] dest Pointer to the destination block of memory.
    //! \param[in] destsz Size of the destination buffer.
    //! \param[in] src Pointer to the source block of memory.
    //! \param[in] count Number of bytes to copy.
    //! \return Zero on success, or an error code on failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a dest is a null pointer
    //!
    //! - \a src is a null pointer
    //!
    //! - \a destsz or \a count is greater than \a RSIZE_MAX
    //!
    //! - \a count is greater than \a destsz (buffer overflow would occur)
    //!
    //! - \a the source and destination objects overlap
    //!
    //! The behavior is undefined if the size of the character array pointed to by \a dest < \a count <= \a destsz; in
    //! other words, an erroneous value of \a destsz does not expose the impending buffer overflow.
    M_INLINE errno_t safe_memcpy(void* M_RESTRICT dest, rsize_t destsz, const void* M_RESTRICT src, rsize_t count)
    {
        return safe_memcpy_impl(dest, destsz, src, count, __FILE__, __func__, __LINE__,
                                "safe_memcpy(dest, destsz, src, count)");
    }
#else
//! \def safe_memcpy(dest, destsz, src, count)
//! \brief Copies a block of memory with bounds checking.
//!
//! This function copies \a count bytes from the block of memory pointed to by \a src to the block of memory pointed
//! to by \a dest, with bounds checking as specified in C11 annex K.
//!
//! \param[out] dest Pointer to the destination block of memory.
//! \param[in] destsz Size of the destination buffer.
//! \param[in] src Pointer to the source block of memory.
//! \param[in] count Number of bytes to copy.
//! \return Zero on success, or an error code on failure.
//!
//! \note The following errors are detected at runtime and call the installed constraint handler:
//!
//! - \a dest is a null pointer
//!
//! - \a src is a null pointer
//!
//! - \a destsz or \a count is greater than \a RSIZE_MAX
//!
//! - \a count is greater than \a destsz (buffer overflow would occur)
//!
//! - \a the source and destination objects overlap
//!
//! The behavior is undefined if the size of the character array pointed to by \a dest < \a count <= \a destsz; in
//! other words, an erroneous value of \a destsz does not expose the impending buffer overflow.
#    define safe_memcpy(dest, destsz, src, count)                                                                      \
        safe_memcpy_impl(dest, destsz, src, count, __FILE__, __func__, __LINE__,                                       \
                         "safe_memcpy(" #dest ", " #destsz ", " #src ", " #count ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_memccpy(void* M_RESTRICT dest, rsize_t destsz, const void* M_RESTRICT src, int c, rsize_t
    //! count)
    //! \brief Copies a block of memory until a specified character is found, with bounds checking.
    //!
    //! This function copies \a count bytes from the block of memory pointed to by \a src to the block of memory pointed
    //! to by \a dest, stopping when the character \a c is found, with bounds checking as specified in C11 annex K.
    //!
    //! \param[out] dest Pointer to the destination block of memory.
    //! \param[in] destsz Size of the destination buffer.
    //! \param[in] src Pointer to the source block of memory.
    //! \param[in] c Character to stop copying at.
    //! \param[in] count Number of bytes to copy.
    //! \return Zero on success, or an error code on failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a dest is a null pointer
    //!
    //! - \a src is a null pointer
    //!
    //! - \a destsz or \a count is greater than \a RSIZE_MAX
    //!
    //! - \a count is greater than \a destsz (buffer overflow would occur)
    //!
    //! - \a the source and destination objects overlap
    //!
    //! The behavior is undefined if the size of the character array pointed to by \a dest < \a count <= \a destsz; in
    //! other words, an erroneous value of \a destsz does not expose the impending buffer overflow.
    M_INLINE errno_t
    safe_memccpy(void* M_RESTRICT dest, rsize_t destsz, const void* M_RESTRICT src, int c, rsize_t count)
    {
        return safe_memccpy_impl(dest, destsz, src, c, count, __FILE__, __func__, __LINE__,
                                 "safe_memccpy(dest, destsz, src, c, count)");
    }
#else
//! \def safe_memccpy(dest, destsz, src, c, count)
//! \brief Copies a block of memory until a specified character is found, with bounds checking.
//!
//! This function copies \a count bytes from the block of memory pointed to by \a src to the block of memory pointed
//! to by \a dest, stopping when the character \a c is found, with bounds checking as specified in C11 annex K.
//!
//! \param[out] dest Pointer to the destination block of memory.
//! \param[in] destsz Size of the destination buffer.
//! \param[in] src Pointer to the source block of memory.
//! \param[in] c Character to stop copying at.
//! \param[in] count Number of bytes to copy.
//! \return Zero on success, or an error code on failure.
//!
//! \note The following errors are detected at runtime and call the installed constraint handler:
//!
//! - \a dest is a null pointer
//!
//! - \a src is a null pointer
//!
//! - \a destsz or \a count is greater than \a RSIZE_MAX
//!
//! - \a count is greater than \a destsz (buffer overflow would occur)
//!
//! - \a the source and destination objects overlap
//!
//! The behavior is undefined if the size of the character array pointed to by \a dest < \a count <= \a destsz; in
//! other words, an erroneous value of \a destsz does not expose the impending buffer overflow.
#    define safe_memccpy(dest, destsz, src, c, count)                                                                  \
        safe_memccpy_impl(dest, destsz, src, c, count, __FILE__, __func__, __LINE__,                                   \
                          "safe_memccpy(" #dest ", " #destsz ", " #src ", " #c ", " #count ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_memcmove(void* M_RESTRICT dest, rsize_t destsz, const void* M_RESTRICT src, int c, rsize_t
    //! count)
    //! \brief Moves a block of memory until a specified character is found, with bounds checking.
    //!
    //! This function moves \a count bytes from the block of memory pointed to by \a src to the block of memory pointed
    //! to by \a dest, stopping when the character \a c is found, with bounds checking as specified in C11 annex K.
    //!
    //! \param[out] dest Pointer to the destination block of memory.
    //! \param[in] destsz Size of the destination buffer.
    //! \param[in] src Pointer to the source block of memory.
    //! \param[in] c Character to stop moving at.
    //! \param[in] count Number of bytes to move.
    //! \return Zero on success, or an error code on failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a dest is a null pointer
    //!
    //! - \a src is a null pointer
    //!
    //! - \a destsz or \a count is greater than \a RSIZE_MAX
    //!
    //! - \a count is greater than \a destsz (buffer overflow would occur)
    //!
    //! The behavior is undefined if the size of the character array pointed to by \a dest < \a count <= \a destsz; in
    //! other words, an erroneous value of \a destsz does not expose the impending buffer overflow.
    M_INLINE errno_t
    safe_memcmove(void* M_RESTRICT dest, rsize_t destsz, const void* M_RESTRICT src, int c, rsize_t count)
    {
        return safe_memcmove_impl(dest, destsz, src, c, count, __FILE__, __func__, __LINE__,
                                  "safe_memcmove(dest, destsz, src, c, count)");
    }
#else
//! \def safe_memcmove(dest, destsz, src, c, count)
//! \brief Moves a block of memory until a specified character is found, with bounds checking.
//!
//! This function moves \a count bytes from the block of memory pointed to by \a src to the block of memory pointed
//! to by \a dest, stopping when the character \a c is found, with bounds checking as specified in C11 annex K.
//!
//! \param[out] dest Pointer to the destination block of memory.
//! \param[in] destsz Size of the destination buffer.
//! \param[in] src Pointer to the source block of memory.
//! \param[in] c Character to stop moving at.
//! \param[in] count Number of bytes to move.
//! \return Zero on success, or an error code on failure.
//!
//! \note The following errors are detected at runtime and call the installed constraint handler:
//!
//! - \a dest is a null pointer
//!
//! - \a src is a null pointer
//!
//! - \a destsz or \a count is greater than \a RSIZE_MAX
//!
//! - \a count is greater than \a destsz (buffer overflow would occur)
//!
//! The behavior is undefined if the size of the character array pointed to by \a dest < \a count <= \a destsz; in
//! other words, an erroneous value of \a destsz does not expose the impending buffer overflow.
#    define safe_memcmove(dest, destsz, src, c, count)                                                                 \
        safe_memcmove_impl(dest, destsz, src, c, count, __FILE__, __func__, __LINE__,                                  \
                           "safe_memcmove(" #dest ", " #destsz ", " #src ", " #c ", " #count ")")
#endif

    //! \fn static M_INLINE size_t get_memalignment(const void* ptr)
    //! \brief calculates the maximum memory alignment of \a ptr
    //! \param[in] ptr pointer to memory to determine the alignment of
    //! \return returns the maximum byte alignment of the pointer. This may be greater than originally
    //! allocated with an aligned allocation. For example, specifying alignment of 8 at allocation
    //! may mean that the pointer is 8 byte aligned but also 16 byte aligned.
    //! if using this to determine if memory alignment is correct, do get_memalignment(ptr) >= alignment
    static M_INLINE size_t get_memalignment(const void* ptr)
    {
        return M_REINTERPRET_CAST(uintptr_t, ptr) & (~M_REINTERPRET_CAST(uintptr_t, ptr) + 1);
    }

//! \def SIZE_OF_STACK_ARRAY(array)
//! \brief Macro to assist with calculating the size of an array on the stack
//!
//! \warning Do not use on heap allocated memory!
//! \param[in] array stack array to determine the size of.
//! \return number of bytes allocated on the stack for the array
#define SIZE_OF_STACK_ARRAY(array) (sizeof(array) / sizeof((*array)))

#if defined(__cplusplus)
}

#    if !defined(safe_free)
template <typename T> M_INLINE void safe_free(T** mem)
{
    safe_free_core(reinterpret_cast<void**>(mem));
}
#    endif // safe_free

#    if !defined(safe_free_aligned)
template <typename T> M_INLINE void safe_free_aligned(T** mem)
{
    safe_free_aligned_core(reinterpret_cast<void**>(mem));
}
#    endif // safe_free_aligned

#    if !defined(safe_free_page_aligned)
template <typename T> M_INLINE void safe_free_page_aligned(T** mem)
{
    safe_free_aligned_core(reinterpret_cast<void**>(mem));
}
#    endif // safe_free_page_aligned

#endif

#if defined (SAFE_FREE_BACKWARDS_COMPATIBILITY)
#    define safe_Free(mem) safe_free_core(mem)
#    define safe_Free_aligned(mem) safe_free_aligned_core(mem)
#    define safe_Free_page_aligned(mem) safe_free_page_aligned(mem)
#endif
