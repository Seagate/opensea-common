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

//! \file impl_memory_safety.h
//! \brief This is an internal header to handle the internal definitions for memory_safety.h
//!        Do NOT include this and use these functions directly. Include memory_safety.h and use the calls from there
//!        The functions with "impl" in the name expect __FILE__, __func__, __LINE__, and an expression when called
//!        which get wrapped by memory_safety.h to make this easier for you to use.

#pragma once

#include "code_attributes.h"
#include "common_types.h"

#if defined(__cplusplus)
extern "C"
{
#endif

    //! \fn errno_t safe_memset_impl(void* dest, rsize_t destsz, int ch, rsize_t count, const char* file, const char*
    //! function, int line, const char* expression)
    //! \brief Sets a block of memory to a specified value with bounds checking.
    //!
    //! This function sets the first \a count bytes of the block of memory pointed to by \a dest to the specified value
    //! \a ch, with bounds checking as specified in C11 annex K.
    //!
    //! \param[out] dest Pointer to the block of memory to fill.
    //! \param[in] destsz Size of the destination buffer.
    //! \param[in] ch Value to be set.
    //! \param[in] count Number of bytes to be set to the value.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return Zero on success, or an error code on failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a dest is a null pointer
    //!
    //! - \a destsz or \a count is greater than \a RSIZE_MAX
    //!
    //! - \a count is greater than \a destsz (buffer overflow would occur)
    //!
    //! The behavior is undefined if the size of the character array pointed to by \a dest < \a count <= \a destsz; in
    //! other words, an erroneous value of \a destsz does not expose the impending buffer overflow.
    errno_t safe_memset_impl(void*       dest,
                             rsize_t     destsz,
                             int         ch,
                             rsize_t     count,
                             const char* file,
                             const char* function,
                             int         line,
                             const char* expression);

    //! \fn errno_t safe_memmove_impl(void* dest, rsize_t destsz, const void* src, rsize_t count, const char* file,
    //! const char* function, int line, const char* expression)
    //! \brief Moves a block of memory with bounds checking.
    //!
    //! This function moves \a count bytes from the block of memory pointed to by \a src to the block of memory pointed
    //! to by \a dest, with bounds checking as specified in C11 annex K.
    //!
    //! \param[out] dest Pointer to the destination block of memory.
    //! \param[in] destsz Size of the destination buffer.
    //! \param[in] src Pointer to the source block of memory.
    //! \param[in] count Number of bytes to move.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
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
    errno_t safe_memmove_impl(void*       dest,
                              rsize_t     destsz,
                              const void* src,
                              rsize_t     count,
                              const char* file,
                              const char* function,
                              int         line,
                              const char* expression);

    //! \fn errno_t safe_memcpy_impl(void* M_RESTRICT dest, rsize_t destsz, const void* M_RESTRICT src, rsize_t count,
    //! const char* file, const char* function, int line, const char* expression)
    //! \brief Copies a block of memory with bounds checking.
    //!
    //! This function copies \a count bytes from the block of memory pointed to by \a src to the block of memory pointed
    //! to by \a dest, with bounds checking as specified in C11 annex K.
    //!
    //! \param[out] dest Pointer to the destination block of memory.
    //! \param[in] destsz Size of the destination buffer.
    //! \param[in] src Pointer to the source block of memory.
    //! \param[in] count Number of bytes to copy.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
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
    errno_t safe_memcpy_impl(void* M_RESTRICT       dest,
                             rsize_t                destsz,
                             const void* M_RESTRICT src,
                             rsize_t                count,
                             const char*            file,
                             const char*            function,
                             int                    line,
                             const char*            expression);

    //! \fn errno_t safe_memccpy_impl(void* M_RESTRICT dest, rsize_t destsz, const void* M_RESTRICT src, int c, rsize_t
    //! count, const char* file, const char* function, int line, const char* expression)
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
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
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
    errno_t safe_memccpy_impl(void* M_RESTRICT       dest,
                              rsize_t                destsz,
                              const void* M_RESTRICT src,
                              int                    c,
                              rsize_t                count,
                              const char*            file,
                              const char*            function,
                              int                    line,
                              const char*            expression);

    //! \fn errno_t safe_memcmove_impl(void* M_RESTRICT dest, rsize_t destsz, const void* M_RESTRICT src, int c, rsize_t
    //! count, const char* file, const char* function, int line, const char* expression)
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
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
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
    errno_t safe_memcmove_impl(void* M_RESTRICT       dest,
                               rsize_t                destsz,
                               const void* M_RESTRICT src,
                               int                    c,
                               rsize_t                count,
                               const char*            file,
                               const char*            function,
                               int                    line,
                               const char*            expression);

    //! \fn M_FUNC_ATTR_MALLOC void* safe_malloc_impl(size_t size, const char* file, const char* function, int line,
    //! const char* expression)
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
    M_FUNC_ATTR_MALLOC void* safe_malloc_impl(size_t      size,
                                              const char* file,
                                              const char* function,
                                              int         line,
                                              const char* expression);

    //! \fn M_FUNC_ATTR_MALLOC void* safe_calloc_impl(size_t count, size_t size, const char* file, const char* function,
    //! int line, const char* expression)
    //! \brief Allocates memory for an array with bounds checking.
    //!
    //! This function allocates memory for an array of \a count elements of \a size bytes each, with bounds checking to
    //! guard against an overflow in the \a count and \a size arguments.
    //!
    //! \param[in] count The number of elements to allocate.
    //! \param[in] size The size of each element.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return A pointer to the allocated memory block, or NULL on failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a count or \a size is zero
    //!
    //! - \a count * \a size results in an overflow
    M_FUNC_ATTR_MALLOC void* safe_calloc_impl(size_t      count,
                                              size_t      size,
                                              const char* file,
                                              const char* function,
                                              int         line,
                                              const char* expression);

    //! \fn M_FUNC_ATTR_MALLOC void* safe_malloc_aligned_impl(size_t size, size_t alignment, const char* file, const
    //! char* function, int line, const char* expression)
    //! \brief Allocates aligned memory with bounds checking.
    //!
    //! This function allocates \a size bytes of memory aligned to \a alignment, with bounds checking to guard against
    //! allocating memory with a size of zero.
    //!
    //! \param[in] size The size of the memory block to allocate.
    //! \param[in] alignment The alignment of the memory block.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return A pointer to the allocated memory block, or NULL on failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a size is zero
    M_FUNC_ATTR_MALLOC void* safe_malloc_aligned_impl(size_t      size,
                                                      size_t      alignment,
                                                      const char* file,
                                                      const char* function,
                                                      int         line,
                                                      const char* expression);

    //! \fn M_FUNC_ATTR_MALLOC void* safe_calloc_aligned_impl(size_t count, size_t size, size_t alignment, const char*
    //! file, const char* function, int line, const char* expression)
    //! \brief Allocates aligned memory for an array with bounds checking.
    //!
    //! This function allocates memory for an array of \a count elements of \a size bytes each, aligned to \a alignment,
    //! with bounds checking to guard against an overflow in the \a count and \a size arguments.
    //!
    //! \param[in] count The number of elements to allocate.
    //! \param[in] size The size of each element.
    //! \param[in] alignment The alignment of the memory block.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return A pointer to the allocated memory block, or NULL on failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a count or \a size is zero
    //!
    //! - \a count * \a size results in an overflow
    M_FUNC_ATTR_MALLOC void* safe_calloc_aligned_impl(size_t      count,
                                                      size_t      size,
                                                      size_t      alignment,
                                                      const char* file,
                                                      const char* function,
                                                      int         line,
                                                      const char* expression);

#if defined(__cplusplus)
}
#endif
