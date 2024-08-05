// SPDX-License-Identifier: MPL-2.0
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2024 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
// 
// \file memory_safety.h
// \brief Implements functions to assist with managing memory as safely as possible

#pragma once

#include "code_attributes.h"
#include "common_types.h"
#include <stdlib.h>
#include <string.h> //to pull in memset for anything that includes this file

#if defined (_WIN32)
#include "windows_version_detect.h" //for determining if securezeromemory2 is available and setting the pragma to include the required library.
#endif

#if defined (__cplusplus)
extern "C"
{
#endif

    //malloc in standards leaves malloc'ing size 0 as a undefined behavior.
    //this version will always return a null pointer if the size is zero
    M_FUNC_ATTR_MALLOC void *safe_malloc(size_t size);

    //avoiding undefined behavior allocing zero size and avoiding alloc'ing less memory due to an overflow
    //If alloc'ing zero or alloc would overflow size_t from count * size, then return a null pointer
    M_FUNC_ATTR_MALLOC void * safe_calloc(size_t count, size_t size);

    //if passed a null pointer, behaves as safe_Malloc
    //if size is zero, will perform free and return NULL ptr
    M_FUNC_ATTR_MALLOC void * safe_realloc(void *block, size_t size);

    //if pointer to block is NULL, returns NULL
    //if pointer to block is passed as a null pointer, behaves as safe_Malloc
    //if size is zero, will perform free and return NULL ptr
    //if realloc fails, free's original block
    //free's original block if realloc fails
    M_FUNC_ATTR_MALLOC void * safe_reallocf(void **block, size_t size);

    //-----------------------------------------------------------------------------
    //
    //  safe_Free()
    //
    //! \brief   Description:  Safely free dynamically allocated memory. This checks for a valid pointer, then frees it and set's it to M_NULLPTR.
    //
    //  Entry:
    //!   \param[in] mem - pointer to heap memory you want to free. Uses double pointer so that upon completion it will automatically be set to M_NULLPTR
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
    //!   \return true = memory is filled with zeros. false = memory has nonzero values in it.
    //
    //-----------------------------------------------------------------------------
    bool is_Empty(const void *ptrData, size_t lengthBytes);

    //-----------------------------------------------------------------------------
    //
    //  void* explicit_zeroes(void* dest, size_t count)
    //
    //! \brief   Description:  Write zeroes to memory pointer that will not be optimized out by the compiler. 
    //
    //  Entry:
    //!   \param[in] dest = pointer to memory to write with zeroes. Must be non-M_NULLPTR
    //!   \param[in] count = number of bytes to write to zero
    //!
    //  Exit:
    //!   \return M_NULLPTR = error occurred otherwise returns pointer to dest
    //
    //-----------------------------------------------------------------------------
    void* explicit_zeroes(void* dest, size_t count);

#if defined (_MSC_VER) && !defined (NO_HAVE_MSFT_SECURE_ZERO_MEMORY2) && (defined (HAVE_MSFT_SECURE_ZERO_MEMORY2) || (defined (WIN_API_TARGET_VERSION) && WIN_API_TARGET_VERSION >= WIN_API_TARGET_WIN11_22621))
    #pragma comment(lib, "volatileaccessu.lib")//for VS to pick up this dependency when SecureZeroMemory2 is available
#endif

    //-----------------------------------------------------------------------------
    //
    //  malloc_aligned(size_t size, size_t alignment)
    //
    //! \brief   Description:  Allocates aligned memory based on the specified power of 2 alignement value
    //
    //  Entry:
    //!   \param[in] size = size of memory block in bytes to allocate
    //!   \param[in] alignment = alignment value required. This MUST be a power of 2.
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
    //! \brief   Description:  Deallocates memory that was allocated with one of malloc_aligned, calloc_aligned, or realloc_aligned
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
    //! \brief   Description:  Safely free dynamically aligned allocated memory. This checks for a valid pointer, then frees it and set's it to M_NULLPTR.
    //
    //  Entry:
    //!   \param[in] mem - pointer to heap memory you want to free. Uses double pointer so that upon completion it will automatically be set to M_NULLPTR
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

    //-----------------------------------------------------------------------------
    //
    //  calloc_aligned(size_t num, size_t size, size_t alignment)
    //
    //! \brief   Description:  Allocates aligned memory based on the specified power of 2 alignement value and zeroes it out.
    //
    //  Entry:
    //!   \param[in] num = # of elements to allocate
    //!   \param[in] size = size of each element
    //!   \param[in] alignment = alignment value required. This MUST be a power of 2.
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
    //! \brief   Description:  Reallocates aligned memory based on the specified power of 2 alignement value and zeroes it out.
    //
    //  Entry:
    //!   \param[in] alignedPtr = pointer to a memory block previously allocated with malloc_aligned, calloc_aligned, or realloc_aligned. If M_NULLPTR, this is the same as malloc_aligned
    //!   \param[in] originalSize = size in bytes of the alignedPtr being passed in. This is used so that previous data can be preserved. Can be set to 0 if there is no care about the original data.
    //!   \param[in] size = size of memory block in bytes to allocate
    //!   \param[in] alignment = alignment value required. This MUST be a power of 2.
    //!
    //  Exit:
    //!   \return ptrToAlignedMemory
    //
    //-----------------------------------------------------------------------------
    M_FUNC_ATTR_MALLOC void* realloc_aligned(void* alignedPtr, size_t originalSize, size_t size, size_t alignment);

    //malloc in standards leaves malloc'ing size 0 as a undefined behavior.
    //this version will always return a null pointer if the size is zero
    M_FUNC_ATTR_MALLOC void *safe_malloc_aligned(size_t size, size_t alignment);

    //avoiding undefined behavior allocing zero size and avoiding alloc'ing less memory due to an overflow
    //If alloc'ing zero or alloc would overflow size_t from count * size, then return a null pointer
    M_FUNC_ATTR_MALLOC void * safe_calloc_aligned(size_t count, size_t size, size_t alignment);

    //if passed a null pointer, behaves as safe_Malloc
    //if size is zero, will perform free and return NULL ptr
    M_FUNC_ATTR_MALLOC void * safe_realloc_aligned(void *block, size_t originalSize, size_t size, size_t alignment);

    //if pointer to block is NULL, returns NULL
    //if pointer to block is passed as a null pointer, behaves as safe_Malloc
    //if size is zero, will perform free and return NULL ptr
    //if realloc fails, free's original block
    //free's original block if realloc fails
    M_FUNC_ATTR_MALLOC void * safe_reallocf_aligned(void **block, size_t originalSize, size_t size, size_t alignment);

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
    //! \brief   Description:  Allocates aligned memory based on the system page size. Same as calling malloc_aligned(size, get_System_Pagesize())
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
    //! \brief   Description:  Deallocates memory that was allocated with one of malloc_page_aligned, calloc_page_aligned, or realloc_page_aligned
    //
    //  Entry:
    //!   \param[in] ptr = pointer to the aligned memory to free
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void free_page_aligned(void* ptr);

    //-----------------------------------------------------------------------------
    //
    //  safe_Free_page_aligned()
    //
    //! \brief   Description:  Safely free dynamically page alligned allocated memory. This checks for a valid pointer, then frees it and set's it to M_NULLPTR.
    //
    //  Entry:
    //!   \param[in] mem - pointer to heap memory you want to free. Uses double pointer so that upon completion it will automatically be set to M_NULLPTR
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

    //-----------------------------------------------------------------------------
    //
    //  calloc_page_aligned(size_t num, size_t size)
    //
    //! \brief   Description:  Allocates aligned memory based on the system page size. Same as calling calloc_aligned(num, size, get_System_Pagesize());
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
    //! \brief   Description:  Reallocates aligned memory based on the system page size. Same as calling realloc_aligned(alignedPtr, size, get_System_Pagesize());
    //
    //  Entry:
    //!   \param[in] alignedPtr = pointer to a memory block previously allocated with malloc_aligned, calloc_aligned, or realloc_aligned. If M_NULLPTR, this is the same as malloc_aligned
    //!   \param[in] originalSize = size in bytes of the alignedPtr being passed in. This is used so that previous data can be preserved.
    //!   \param[in] size = size of memory block in bytes to allocate
    //!
    //  Exit:
    //!   \return ptrToAlignedMemory
    //
    //-----------------------------------------------------------------------------
    M_FUNC_ATTR_MALLOC void* realloc_page_aligned(void* alignedPtr, size_t originalSize, size_t size);

    //malloc in standards leaves malloc'ing size 0 as a undefined behavior.
    //this version will always return a null pointer if the size is zero
    M_FUNC_ATTR_MALLOC void *safe_malloc_page_aligned(size_t size);

    //avoiding undefined behavior allocing zero size and avoiding alloc'ing less memory due to an overflow
    //If alloc'ing zero or alloc would overflow size_t from count * size, then return a null pointer
    M_FUNC_ATTR_MALLOC void * safe_calloc_page_aligned(size_t count, size_t size);

    //if passed a null pointer, behaves as safe_Malloc
    //if size is zero, will perform free and return NULL ptr
    M_FUNC_ATTR_MALLOC void * safe_realloc_page_aligned(void *block, size_t originalSize, size_t size);

    //if pointer to block is NULL, returns NULL
    //if pointer to block is passed as a null pointer, behaves as safe_Malloc
    //if size is zero, will perform free and return NULL ptr
    //if realloc fails, free's original block
    //free's original block if realloc fails
    M_FUNC_ATTR_MALLOC void * safe_reallocf_page_aligned(void **block, size_t originalSize, size_t size);

    //Calls memmove_s if available, otherwise performs all checks that memmove_s does before calling memmove
    errno_t safe_memmove(void *dest, rsize_t destsz, const void *src, rsize_t count);

    //calls memcpy_s if available, otherwise performs all checks that memcpy_s does before calling memcpy
    errno_t safe_memcpy(void *M_RESTRICT dest, rsize_t destsz, const void *M_RESTRICT src, rsize_t count);

#if defined (__cplusplus)
}
#endif
