// SPDX-License-Identifier: MPL-2.0

//! \file impl_memory_safety.h
//! \brief This is an internal header to handle the internal definitions for memory_safety.h
//!        Do NOT include this and use these functions directly. Include memory_safety.h and use the calls from there
//!        The functions with "impl" in the name expect __FILE__, __func__, __LINE__, and an expression when called
//!        which get wrapped by memory_safety.h to make this easier for you to use.
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2026 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "code_attributes.h"
#include "common_types.h"
#include "constraint_handling.h"
#include "type_conversion.h"

#if defined(__cplusplus)
extern "C"
{
#endif

    //! \def M_MEMORY_REGIONS_OVERLAP_COMPILE_TIME(ptr1, size1, ptr2, size2)
    //! \brief Compile-time constant check for potentially overlapping memory regions.
    //!
    //! This macro can be used with M_DIAG_ERROR to detect overlapping memory regions using
    //! pointer arithmetic. The check determines if region 1 [ptr1, ptr1+size1) overlaps with
    //! region 2 [ptr2, ptr2+size2) by verifying that:
    //!   - ptr1 starts before region 2 ends (ptr1 < ptr2 + size2), AND
    //!   - ptr2 starts before region 1 ends (ptr2 < ptr1 + size1)
    //!
    //! This uses natural pointer subtraction (no explicit casts), which results in ptrdiff_t
    //! and is suitable for use in constant expressions with diagnose_if.
    //!
    //! \param[in] ptr1 First memory region pointer
    //! \param[in] size1 Size of first region
    //! \param[in] ptr2 Second memory region pointer
    //! \param[in] size2 Size of second region
    //! \return True (non-zero) if regions overlap, false (zero) otherwise
    //!
    //! \note This macro uses pointer arithmetic and will work for compile-time constant pointers.
    //!       For runtime cases with dynamic pointers, use the memory_regions_overlap() function
    //!       from memory_safety.h
    //!
    //! \example
    //! M_DIAG_ERROR(M_MEMORY_REGIONS_OVERLAP_COMPILE_TIME(dest, destsz, src, count),
    //!              "source and destination regions overlap")
#define M_MEMORY_REGIONS_OVERLAP_COMPILE_TIME(ptr1, size1, ptr2, size2)                                                \
    (((ptr1) < (ptr2) + (size2)) && ((ptr2) < (ptr1) + (size1)))

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
    M_PARAM_WO_SIZE(1, 2)
    CONSTRAINT_NO_DISCARD errno_t safe_memset_impl(void* M_NONNULL        dest,
                                                   rsize_t                destsz,
                                                   int                    ch,
                                                   rsize_t                count,
                                                   const char* M_NULLABLE file,
                                                   const char* M_NULLABLE function,
                                                   int                    line,
                                                   const char* M_NULLABLE expression)
        // clang-format off
        M_DIAG_ERROR(dest == M_NULLPTR, "dest is a null pointer")
        M_DIAG_ERROR(destsz > RSIZE_MAX, "destsz > RSIZE_MAX")
        M_DIAG_ERROR(count > RSIZE_MAX, "count > RSIZE_MAX")
        M_DIAG_ERROR(count > destsz, "count > destsz")
        // clang-format on
        ;

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
    M_PARAM_WO_SIZE(1, 2)
    M_PARAM_RO_SIZE(3, 4)
    CONSTRAINT_NO_DISCARD errno_t safe_memmove_impl(void* M_NONNULL        dest,
                                                    rsize_t                destsz,
                                                    const void* M_NONNULL  src,
                                                    rsize_t                count,
                                                    const char* M_NULLABLE file,
                                                    const char* M_NULLABLE function,
                                                    int                    line,
                                                    const char* M_NULLABLE expression)
        // clang-format off
        M_DIAG_ERROR(dest == M_NULLPTR, "dest is a null pointer")
        M_DIAG_ERROR(src == M_NULLPTR, "src is a null pointer")
        M_DIAG_ERROR(destsz > RSIZE_MAX, "destsz > RSIZE_MAX")
        M_DIAG_ERROR(count > RSIZE_MAX, "count > RSIZE_MAX")
        M_DIAG_ERROR(count > destsz, "count > destsz")
        // clang-format on
        ;

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
    M_PARAM_WO_SIZE(1, 2)
    M_PARAM_RO_SIZE(3, 4)
    CONSTRAINT_NO_DISCARD errno_t safe_memcpy_impl(void* M_RESTRICT M_NONNULL       dest,
                                                   rsize_t                          destsz,
                                                   const void* M_RESTRICT M_NONNULL src,
                                                   rsize_t                          count,
                                                   const char* M_NULLABLE           file,
                                                   const char* M_NULLABLE           function,
                                                   int                              line,
                                                   const char* M_NULLABLE           expression)
        // clang-format off
        M_DIAG_ERROR(dest == M_NULLPTR, "dest is a null pointer")
        M_DIAG_ERROR(src == M_NULLPTR, "src is a null pointer")
        M_DIAG_ERROR(destsz > RSIZE_MAX, "destsz > RSIZE_MAX")
        M_DIAG_ERROR(count > RSIZE_MAX, "count > RSIZE_MAX")
        M_DIAG_ERROR(count > destsz, "count > destsz")
        M_DIAG_ERROR(M_MEMORY_REGIONS_OVERLAP_COMPILE_TIME(dest, destsz, src, count), "source and destination regions overlap. Use safe_memmove instead.")
#if defined(ALLOW_NO_OVERLAP_SUGGESTIONS)
        M_DIAG_WARN(!M_MEMORY_REGIONS_OVERLAP_COMPILE_TIME(dest, destsz, src, count) && dest != src, "No overlap detected; consider safe_memcpy_no_overlap for better performance.")
#endif
        // clang-format on
        ;

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
    M_PARAM_WO_SIZE(1, 2)
    M_PARAM_RO_SIZE(3, 5)
    CONSTRAINT_NO_DISCARD errno_t safe_memccpy_impl(void* M_RESTRICT M_NONNULL       dest,
                                                    rsize_t                          destsz,
                                                    const void* M_RESTRICT M_NONNULL src,
                                                    int                              c,
                                                    rsize_t                          count,
                                                    const char* M_NULLABLE           file,
                                                    const char* M_NULLABLE           function,
                                                    int                              line,
                                                    const char* M_NULLABLE           expression)
        // clang-format off
        M_DIAG_ERROR(dest == M_NULLPTR, "dest is a null pointer")
        M_DIAG_ERROR(src == M_NULLPTR, "src is a null pointer")
        M_DIAG_ERROR(destsz > RSIZE_MAX, "destsz > RSIZE_MAX")
        M_DIAG_ERROR(count > RSIZE_MAX, "count > RSIZE_MAX")
        M_DIAG_ERROR(count > destsz, "count > destsz")
        M_DIAG_ERROR(M_MEMORY_REGIONS_OVERLAP_COMPILE_TIME(dest, destsz, src, count), "source and destination regions overlap. Use safe_memcmove instead.")
#if defined(ALLOW_NO_OVERLAP_SUGGESTIONS)
        M_DIAG_WARN(!M_MEMORY_REGIONS_OVERLAP_COMPILE_TIME(dest, destsz, src, count) && dest != src, "No overlap detected; consider safe_memccpy_no_overlap for better performance.")
#endif
        // clang-format on
        ;

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
    M_PARAM_WO_SIZE(1, 2)
    M_PARAM_RO_SIZE(3, 5)
    CONSTRAINT_NO_DISCARD errno_t safe_memcmove_impl(void* M_RESTRICT M_NONNULL       dest,
                                                     rsize_t                          destsz,
                                                     const void* M_RESTRICT M_NONNULL src,
                                                     int                              c,
                                                     rsize_t                          count,
                                                     const char* M_NULLABLE           file,
                                                     const char* M_NULLABLE           function,
                                                     int                              line,
                                                     const char* M_NULLABLE           expression)
        // clang-format off
        M_DIAG_ERROR(dest == M_NULLPTR, "dest is a null pointer")
        M_DIAG_ERROR(src == M_NULLPTR, "src is a null pointer")
        M_DIAG_ERROR(destsz > RSIZE_MAX, "destsz > RSIZE_MAX")
        M_DIAG_ERROR(count > RSIZE_MAX, "count > RSIZE_MAX")
        M_DIAG_ERROR(count > destsz, "count > destsz")
        // clang-format on
        ;

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
    M_NODISCARD M_FUNC_ATTR_MALLOC M_MALLOC_SIZE(1) void* M_NULLABLE safe_malloc_impl(size_t                 size,
                                                                                      const char* M_NULLABLE file,
                                                                                      const char* M_NULLABLE function,
                                                                                      int                    line,
                                                                                      const char* M_NULLABLE expression)
        // clang-format off
        M_DIAG_ERROR(size == 0, "safe_malloc of size zero is not allowed")
        M_DIAG_WARN(size > RSIZE_MAX, "allocating more than RSIZE_MAX bytes may fail")
        // clang-format on
        ;

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
    M_NODISCARD M_FUNC_ATTR_MALLOC M_CALLOC_SIZE(1, 2) void* M_NULLABLE
        safe_calloc_impl(size_t                 count,
                         size_t                 size,
                         const char* M_NULLABLE file,
                         const char* M_NULLABLE function,
                         int                    line,
                         const char* M_NULLABLE expression)
        // clang-format off
        M_DIAG_ERROR(size == 0, "safe_calloc with count of zero is not allowed")
        M_DIAG_ERROR(size == 0, "safe_calloc with size of zero is not allowed")
        M_DIAG_ERROR(count > (SIZE_MAX / size), "safe_calloc size * count overflows")
        M_DIAG_WARN((count * size) > RSIZE_MAX, "allocating more than RSIZE_MAX bytes may fail")
        // clang-format on
        ;

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
    M_NODISCARD M_FUNC_ATTR_MALLOC M_MALLOC_SIZE(1) M_ALLOC_ALIGN(2) void* M_NULLABLE
        safe_malloc_aligned_impl(size_t                 size,
                                 size_t                 alignment,
                                 const char* M_NULLABLE file,
                                 const char* M_NULLABLE function,
                                 int                    line,
                                 const char* M_NULLABLE expression)
        // clang-format off
        M_DIAG_ERROR(size == 0, "safe_malloc_aligned of size zero is not allowed")
        M_DIAG_WARN(size > RSIZE_MAX, "allocating more than RSIZE_MAX bytes may fail")
        M_DIAG_WARN(alignment == 0 || (alignment & (alignment - 1)) != 0, "alignment should be a non-zero power of two")
        // clang-format on
        ;

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
    M_NODISCARD M_FUNC_ATTR_MALLOC M_CALLOC_SIZE(1, 2) M_ALLOC_ALIGN(3) void* M_NULLABLE
        safe_calloc_aligned_impl(size_t                 count,
                                 size_t                 size,
                                 size_t                 alignment,
                                 const char* M_NULLABLE file,
                                 const char* M_NULLABLE function,
                                 int                    line,
                                 const char* M_NULLABLE expression)
        // clang-format off
        M_DIAG_ERROR(size == 0, "safe_calloc_aligned with count of zero is not allowed")
        M_DIAG_ERROR(size == 0, "safe_calloc_aligned with size of zero is not allowed")
        M_DIAG_ERROR(count > (SIZE_MAX / size), "safe_calloc_aligned size * count overflows")
        M_DIAG_WARN((count * size) > RSIZE_MAX, "allocating more than RSIZE_MAX bytes may fail")
        M_DIAG_WARN(alignment == 0 || (alignment & (alignment - 1)) != 0, "alignment should be a non-zero power of two")
        // clang-format on
        ;

#if defined(__cplusplus)
}
#endif
