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

//! \file impl_sort_and_search.h
//! \brief This is an internal header to handle the internal definitions for sort_and_search.h
//!
//!       Do NOT include this and use these functions directly. Include sort_and_search.h and use the calls from there
//!       The functions with "impl" in the name expect __FILE__, __func__, __LINE__, and an expression when called which
//!       get wrapped to make this easier for you to use.

#pragma once

#include "code_attributes.h"
#include "common_types.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#if !defined(COMPARE_FUNC_TYPES)
    //! \typedef comparefn
    //! \brief Function pointer type for comparison functions.
    //!
    //! This typedef defines a function pointer type for comparison functions used in sorting and searching algorithms.
    //!
    //! \param[in] a Pointer to the first element to compare.
    //! \param[in] b Pointer to the second element to compare.
    //! \return An integer less than, equal to, or greater than zero if \a a is found, respectively, to be less than, to
    //! match, or to be greater than \a b.
    typedef int (*comparefn)(const void*, const void*);

    //! \typedef ctxcomparefn
    //! \brief Function pointer type for comparison functions with context.
    //!
    //! This typedef defines a function pointer type for comparison functions used in sorting and searching algorithms
    //! that also take an additional context parameter.
    //!
    //! \param[in] a Pointer to the first element to compare.
    //! \param[in] b Pointer to the second element to compare.
    //! \param[in] context Pointer to the context for the comparison.
    //! \return An integer less than, equal to, or greater than zero if \a a is found, respectively, to be less than, to
    //! match, or to be greater than \a b.
    typedef int (*ctxcomparefn)(const void*, const void*, void*);

    //! \def COMPARE_FUNC_TYPES
    //! \brief defined when the typedefs for comparfn and ctxcomparefn are available
#    define COMPARE_FUNC_TYPES
#endif // COMPARE_FUNC_TYPES

    //! \fn errno_t safe_qsort_impl(void* ptr, rsize_t count, rsize_t size, comparefn compare, const char* file, const
    //! char* function, int line, const char* expression)
    //!
    //! \brief Sorts an array with bounds checking.
    //!
    //! This function sorts an array of \a count elements, each of \a size bytes, using the comparison function \a
    //! compare, with bounds checking as specified in C11 annex K.
    //!
    //! \param[in,out] ptr Pointer to the array to be sorted.
    //! \param[in] count Number of elements in the array.
    //! \param[in] size Size of each element in the array.
    //! \param[in] compare Comparison function to determine the order of the elements.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return Zero on success, or an error code on failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a ptr is a null pointer
    //!
    //! - \a count or \a size is zero
    //!
    //! - \a compare is a null pointer
    errno_t safe_qsort_impl(void*       ptr,
                            rsize_t     count,
                            rsize_t     size,
                            comparefn   compare,
                            const char* file,
                            const char* function,
                            int         line,
                            const char* expression);

    //! \fn errno_t safe_qsort_context_impl(void* ptr, rsize_t count, rsize_t size, ctxcomparefn compare, void* context,
    //! const char* file, const char* function, int line, const char* expression)
    //!
    //! \brief Sorts an array with bounds checking and an optional context parameter.
    //!
    //! This function sorts an array of \a count elements, each of \a size bytes, using the comparison function \a
    //! compare and an optional context parameter \a context, with bounds checking as specified in C11 annex K.
    //!
    //! \param[in,out] ptr Pointer to the array to be sorted.
    //! \param[in] count Number of elements in the array.
    //! \param[in] size Size of each element in the array.
    //! \param[in] compare Comparison function to determine the order of the elements.
    //! \param[in] context Optional context parameter for the comparison function.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return Zero on success, or an error code on failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a ptr is a null pointer
    //!
    //! - \a count or \a size is zero
    //!
    //! - \a compare is a null pointer
    errno_t safe_qsort_context_impl(void*        ptr,
                                    rsize_t      count,
                                    rsize_t      size,
                                    ctxcomparefn compare,
                                    void*        context,
                                    const char*  file,
                                    const char*  function,
                                    int          line,
                                    const char*  expression);

    //! \fn void* safe_bsearch_impl(const void* key, const void* ptr, rsize_t count, rsize_t size, comparefn compare,
    //! const char* file, const char* function, int line, const char* expression)
    //!
    //! \brief Searches a sorted array with bounds checking.
    //!
    //! This function searches a sorted array of \a count elements, each of \a size bytes, for an element that matches
    //! \a key, using the comparison function \a compare, with bounds checking as specified in C11 annex K.
    //!
    //! \param[in] key Pointer to the key to search for.
    //! \param[in] ptr Pointer to the array to be searched.
    //! \param[in] count Number of elements in the array.
    //! \param[in] size Size of each element in the array.
    //! \param[in] compare Comparison function to determine the order of the elements.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return Pointer to the matching element, or NULL if no match is found.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a key is a null pointer
    //!
    //! - \a ptr is a null pointer
    //!
    //! - \a count or \a size is zero
    //!
    //! - \a compare is a null pointer
    void* safe_bsearch_impl(const void* key,
                            const void* ptr,
                            rsize_t     count,
                            rsize_t     size,
                            comparefn   compare,
                            const char* file,
                            const char* function,
                            int         line,
                            const char* expression);

    //! \fn void* safe_bsearch_context_impl(const void* key, void* ptr, rsize_t count, rsize_t size, ctxcomparefn
    //! compare, void* context, const char* file, const char* function, int line, const char* expression)
    //!
    //! \brief Searches a sorted array with bounds checking and an optional context parameter.
    //!
    //! This function searches a sorted array of \a count elements, each of \a size bytes, for an element that matches
    //! \a key, using the comparison function \a compare and an optional context parameter \a context, with bounds
    //! checking as specified in C11 annex K.
    //!
    //! \param[in] key Pointer to the key to search for.
    //! \param[in,out] ptr Pointer to the array to be searched.
    //! \param[in] count Number of elements in the array.
    //! \param[in] size Size of each element in the array.
    //! \param[in] compare Comparison function to determine the order of the elements.
    //! \param[in] context Optional context parameter for the comparison function.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return Pointer to the matching element, or NULL if no match is found.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a key is a null pointer
    //!
    //! - \a ptr is a null pointer
    //!
    //! - \a count or \a size is zero
    //!
    //! - \a compare is a null pointer
    void* safe_bsearch_context_impl(const void*  key,
                                    void*        ptr,
                                    rsize_t      count,
                                    rsize_t      size,
                                    ctxcomparefn compare,
                                    void*        context,
                                    const char*  file,
                                    const char*  function,
                                    int          line,
                                    const char*  expression);

    //! \fn void* safe_lsearch_impl(const void* key, void* base, size_t* nelp, size_t width, comparefn compar, const
    //! char* file, const char* function, int line, const char* expression)
    //!
    //! \brief Searches an array linearly with bounds checking.
    //!
    //! This function searches an array of \a *nelp elements, each of \a width bytes, for an element that matches \a
    //! key, using the comparison function \a compar, with bounds checking as specified in C11 annex K.
    //!
    //! \param[in] key Pointer to the key to search for.
    //! \param[in,out] base Pointer to the array to be searched.
    //! \param[in,out] nelp Pointer to the number of elements in the array.
    //! \param[in] width Size of each element in the array.
    //! \param[in] compar Comparison function to determine the order of the elements.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return Pointer to the matching element, or NULL if no match is found.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a key is a null pointer
    //!
    //! - \a base is a null pointer
    //!
    //! - \a nelp is a null pointer
    //!
    //! - \a width is zero
    //!
    //! - \a compar is a null pointer
    void* safe_lsearch_impl(const void* key,
                            void*       base,
                            size_t*     nelp,
                            size_t      width,
                            comparefn   compar,
                            const char* file,
                            const char* function,
                            int         line,
                            const char* expression);

    //! \fn void* safe_lsearch_context_impl(const void* key, void* base, size_t* nelp, size_t width, ctxcomparefn
    //! compar, void* context, const char* file, const char* function, int line, const char* expression)
    //!
    //! \brief Searches an array linearly with bounds checking and an optional context parameter.
    //!
    //! This function searches an array of \a *nelp elements, each of \a width bytes, for an element that matches \a
    //! key, using the comparison function \a compar and an optional context parameter \a context, with bounds checking
    //! as specified in C11 annex K.
    //!
    //! \param[in] key Pointer to the key to search for.
    //! \param[in,out] base Pointer to the array to be searched.
    //! \param[in,out] nelp Pointer to the number of elements in the array.
    //! \param[in] width Size of each element in the array.
    //! \param[in] compar Comparison function to determine the order of the elements.
    //! \param[in] context Optional context parameter for the comparison function.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return Pointer to the matching element, or NULL if no match is found.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a key is a null pointer
    //!
    //! - \a base is a null pointer
    //!
    //! - \a nelp is a null pointer
    //!
    //! - \a width is zero
    //!
    //! - \a compar is a null pointer
    void* safe_lsearch_context_impl(const void*  key,
                                    void*        base,
                                    size_t*      nelp,
                                    size_t       width,
                                    ctxcomparefn compar,
                                    void*        context,
                                    const char*  file,
                                    const char*  function,
                                    int          line,
                                    const char*  expression);

    //! \fn void* safe_lfind_impl(const void* key, const void* base, size_t* nelp, size_t width, comparefn compar, const
    //! char* file, const char* function, int line, const char* expression)
    //!
    //! \brief Searches an array linearly for an element with bounds checking.
    //!
    //! This function searches an array of \a *nelp elements, each of \a width bytes, for an element that matches \a
    //! key, using the comparison function \a compar, with bounds checking as specified in C11 annex K.
    //!
    //! \param[in] key Pointer to the key to search for.
    //! \param[in] base Pointer to the array to be searched.
    //! \param[in,out] nelp Pointer to the number of elements in the array.
    //! \param[in] width Size of each element in the array.
    //! \param[in] compar Comparison function to determine the order of the elements.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return Pointer to the matching element, or NULL if no match is found.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a key is a null pointer
    //!
    //! - \a base is a null pointer
    //!
    //! - \a nelp is a null pointer
    //!
    //! - \a width is zero
    //!
    //! - \a compar is a null pointer
    void* safe_lfind_impl(const void* key,
                          const void* base,
                          size_t*     nelp,
                          size_t      width,
                          comparefn   compar,
                          const char* file,
                          const char* function,
                          int         line,
                          const char* expression);

    //! \fn void* safe_lfind_context_impl(const void* key, const void* base, size_t* nelp, size_t width, ctxcomparefn
    //! compar, void* context, const char* file, const char* function, int line, const char* expression)
    //!
    //! \brief Searches an array linearly for an element with bounds checking and an optional context parameter.
    //!
    //! This function searches an array of \a *nelp elements, each of \a width bytes, for an element that matches \a
    //! key, using the comparison function \a compar and an optional context parameter \a context, with bounds checking
    //! as specified in C11 annex K.
    //!
    //! \param[in] key Pointer to the key to search for.
    //! \param[in] base Pointer to the array to be searched.
    //! \param[in,out] nelp Pointer to the number of elements in the array.
    //! \param[in] width Size of each element in the array.
    //! \param[in] compar Comparison function to determine the order of the elements.
    //! \param[in] context Optional context parameter for the comparison function.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return Pointer to the matching element, or NULL if no match is found.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a key is a null pointer
    //!
    //! - \a base is a null pointer
    //!
    //! - \a nelp is a null pointer
    //!
    //! - \a width is zero
    //!
    //! - \a compar is a null pointer
    void* safe_lfind_context_impl(const void*  key,
                                  const void*  base,
                                  size_t*      nelp,
                                  size_t       width,
                                  ctxcomparefn compar,
                                  void*        context,
                                  const char*  file,
                                  const char*  function,
                                  int          line,
                                  const char*  expression);

#if defined(__cplusplus)
}
#endif
