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

//! \file sort_and_search.h
//! \brief Provides safe sort and search function definitions.
//!
//! These work similar to C11's Annex K checking bounds, checking for valid pointers, etc.
//!

#pragma once

#include "code_attributes.h"
#include "common_types.h"
#include "impl_sort_and_search.h"
#include "predef_env_detect.h"

#if defined(__cplusplus)
extern "C"
{
#endif //__cplusplus

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

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_qsort(void* ptr, rsize_t count, rsize_t size, comparefn compare))
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
    //! \return Zero on success, or an error code on failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a ptr is a null pointer
    //!
    //! - \a count or \a size is zero
    //!
    //! - \a compare is a null pointer
    M_INLINE errno_t safe_qsort(void* ptr, rsize_t count, rsize_t size, comparefn compare)
    {
        return safe_qsort_impl(ptr, count, size, compare, __FILE__, __func__, __LINE__,
                               "safe_qsort(ptr, count, size, compare)");
    }
#else
//! \def safe_qsort
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
//! \return Zero on success, or an error code on failure.
//!
//! \note The following errors are detected at runtime and call the installed constraint handler:
//!
//! - \a ptr is a null pointer
//!
//! - \a count or \a size is zero
//!
//! - \a compare is a null pointer
#    define safe_qsort(ptr, count, size, compare)                                                                      \
        safe_qsort_impl(ptr, count, size, compare, __FILE__, __func__, __LINE__,                                       \
                        "safe_qsort(" #ptr ", " #count ", " #size ", " #compare ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_qsort_context(void* ptr, rsize_t count, rsize_t size, ctxcomparefn compare)
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
    //! \return Zero on success, or an error code on failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a ptr is a null pointer
    //!
    //! - \a count or \a size is zero
    //!
    //! - \a compare is a null pointer
    M_INLINE errno_t safe_qsort_context(void* ptr, rsize_t count, rsize_t size, ctxcomparefn compare, void* context)
    {
        return safe_qsort_context_impl(ptr, count, size, compare, context, __FILE__, __func__, __LINE__,
                                       "safe_qsort_context(ptr, count, size, compare, context)");
    }
#else
//! \def safe_qsort_context
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
//! \return Zero on success, or an error code on failure.
//!
//! \note The following errors are detected at runtime and call the installed constraint handler:
//!
//! - \a ptr is a null pointer
//!
//! - \a count or \a size is zero
//!
//! - \a compare is a null pointer
#    define safe_qsort_context(ptr, count, size, compare, context)                                                     \
        safe_qsort_context_impl(ptr, count, size, compare, context, __FILE__, __func__, __LINE__,                      \
                                "safe_qsort_context(" #ptr ", " #count ", " #size ", " #compare ", " #context ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn void* safe_bsearch(const void* key, const void* ptr, rsize_t count, rsize_t size, comparefn compare)
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
    M_INLINE void* safe_bsearch(const void* key, const void* ptr, rsize_t count, rsize_t size, comparefn compare)
    {
        return safe_bsearch_impl(key, ptr, count, size, compare, __FILE__, __func__, __LINE__,
                                 "safe_bsearch(key, ptr, count, size, compare)");
    }
#else
//! \def safe_bsearch
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
#    define safe_bsearch(key, ptr, count, size, compare)                                                               \
        safe_bsearch_impl(key, ptr, count, size, compare, __FILE__, __func__, __LINE__,                                \
                          "safe_bsearch(" #key ", " #ptr ", " #count ", " #size ", " #compare ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn void* safe_bsearch_context(const void* key, void* ptr, rsize_t count, rsize_t size, ctxcomparefn
    //! compare)
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
    M_INLINE void* safe_bsearch_context(const void*  key,
                                        const void*  ptr,
                                        rsize_t      count,
                                        rsize_t      size,
                                        ctxcomparefn compare,
                                        void*        context)
    {
        return safe_bsearch_context_impl(key, ptr, count, size, compare, context, __FILE__, __func__, __LINE__,
                                         "safe_bsearch_context(key, ptr, count, size, compare, context)");
    }
#else
//! \def safe_bsearch_context
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
#    define safe_bsearch_context(key, ptr, count, size, compare, context)                                              \
        safe_bsearch_context_impl(key, ptr, count, size, compare, context, __FILE__, __func__, __LINE__,               \
                                  "safe_bsearch_context(" #key ", " #ptr ", " #count ", " #size ", " #compare          \
                                  ", " #context ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn void* safe_lsearch(const void* key, void* base, size_t* nelp, size_t width, comparefn compar)
    //!
    //! \brief Searches an array linearly with bounds checking.
    //!
    //! This function searches an array of \a *nelp elements, each of \a width bytes, for an element that matches \a
    //! key, using the comparison function \a compar, with bounds checking as specified in C11 annex K.
    //! If safe_lsearch does not find a matching element, then the key object is inserted at the end of the
    //! table, and *nmemb is incremented. In particular, one should know that a matching element exists, or that more
    //! room is available.
    //!
    //! \param[in] key Pointer to the key to search for.
    //! \param[in,out] base Pointer to the array to be searched.
    //! \param[in,out] nelp Pointer to the number of elements in the array.
    //! \param[in] width Size of each element in the array.
    //! \param[in] compar Comparison function to determine the order of the elements.
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
    M_INLINE void* safe_lsearch(const void* key, void* base, size_t* nelp, size_t width, comparefn compar)
    {
        return safe_lsearch_impl(key, base, nelp, width, compar, __FILE__, __func__, __LINE__,
                                 "safe_lsearch(key, base, nelp, width, compar)");
    }
#else
//! \def safe_lsearch
//!
//! \brief Searches an array linearly with bounds checking.
//!
//! This function searches an array of \a *nelp elements, each of \a width bytes, for an element that matches \a
//! key, using the comparison function \a compar, with bounds checking as specified in C11 annex K.
//! If safe_lsearch does not find a matching element, then the key object is inserted at the end of the
//! table, and *nmemb is incremented. In particular, one should know that a matching element exists, or that more
//! room is available.
//!
//! \param[in] key Pointer to the key to search for.
//! \param[in,out] base Pointer to the array to be searched.
//! \param[in,out] nelp Pointer to the number of elements in the array.
//! \param[in] width Size of each element in the array.
//! \param[in] compar Comparison function to determine the order of the elements.
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
#    define safe_lsearch(key, base, nelp, width, compar)                                                               \
        safe_lsearch_impl(key, base, nelp, width, compar, __FILE__, __func__, __LINE__,                                \
                          "safe_lsearch(" #key ", " #base ", " #nelp ", " #width ", " #compar ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn void* safe_lsearch_context(const void* key, void* base, size_t* nelp, size_t width, ctxcomparefn
    //! compar)
    //!
    //! \brief Searches an array linearly with bounds checking and an optional context parameter.
    //!
    //! This function searches an array of \a *nelp elements, each of \a width bytes, for an element that matches \a
    //! key, using the comparison function \a compar and an optional context parameter \a context, with bounds checking
    //! as specified in C11 annex K.
    //! If safe_lsearch_context does not find a matching element, then the key object is inserted at the end of the
    //! table, and *nmemb is incremented. In particular, one should know that a matching element exists, or that more
    //! room is available.
    //!
    //! \param[in] key Pointer to the key to search for.
    //! \param[in,out] base Pointer to the array to be searched.
    //! \param[in,out] nelp Pointer to the number of elements in the array.
    //! \param[in] width Size of each element in the array.
    //! \param[in] compar Comparison function to determine the order of the elements.
    //! \param[in] context Optional context parameter for the comparison function.
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
    M_INLINE void* safe_lsearch_context(const void*  key,
                                        void*        base,
                                        size_t*      nelp,
                                        size_t       width,
                                        ctxcomparefn compar,
                                        void*        context)
    {
        return safe_lsearch_context_impl(key, base, nelp, width, compar, context, __FILE__, __func__, __LINE__,
                                         "safe_lsearch_context(key, base, nelp, width, compar, context)");
    }
#else
//! \def safe_lsearch_context
//!
//! \brief Searches an array linearly with bounds checking and an optional context parameter.
//!
//! This function searches an array of \a *nelp elements, each of \a width bytes, for an element that matches \a
//! key, using the comparison function \a compar and an optional context parameter \a context, with bounds checking
//! as specified in C11 annex K.
//! If safe_lsearch_context does not find a matching element, then the key object is inserted at the end of the
//! table, and *nmemb is incremented. In particular, one should know that a matching element exists, or that more
//! room is available.
//!
//! \param[in] key Pointer to the key to search for.
//! \param[in,out] base Pointer to the array to be searched.
//! \param[in,out] nelp Pointer to the number of elements in the array.
//! \param[in] width Size of each element in the array.
//! \param[in] compar Comparison function to determine the order of the elements.
//! \param[in] context Optional context parameter for the comparison function.
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
#    define safe_lsearch_context(key, base, nelp, width, compar, context)                                              \
        safe_lsearch_context_impl(key, base, nelp, width, compar, context, __FILE__, __func__, __LINE__,               \
                                  "safe_lsearch_context(" #key ", " #base ", " #nelp ", " #width ", " #compar          \
                                  ", " #context ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn void* safe_lfind(const void* key, const void* base, size_t* nelp, size_t width, comparefn compar)
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
    M_INLINE void* safe_lfind(const void* key, void* base, size_t* nelp, size_t width, comparefn compar)
    {
        return safe_lfind_impl(key, base, nelp, width, compar, __FILE__, __func__, __LINE__,
                               "safe_lfind(key, base, nelp, width, compar)");
    }
#else
//! \def safe_lfind
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
#    define safe_lfind(key, base, nelp, width, compar)                                                                 \
        safe_lfind_impl(key, base, nelp, width, compar, __FILE__, __func__, __LINE__,                                  \
                        "safe_lfind(" #key ", " #base ", " #nelp ", " #width ", " #compar ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn void* safe_lfind_context(const void* key, const void* base, size_t* nelp, size_t width, ctxcomparefn
    //! compar)
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
    M_INLINE void* safe_lfind_context(const void*  key,
                                      void*        base,
                                      size_t*      nelp,
                                      size_t       width,
                                      ctxcomparefn compar,
                                      void*        context)
    {
        return safe_lfind_context_impl(key, base, nelp, width, compar, context, __FILE__, __func__, __LINE__,
                                       "safe_lfind_context(key, base, nelp, width, compar, context)");
    }
#else
//! \def safe_lfind_context
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
#    define safe_lfind_context(key, base, nelp, width, compar, context)                                                \
        safe_lfind_context_impl(key, base, nelp, width, compar, context, __FILE__, __func__, __LINE__,                 \
                                "safe_lfind_context(" #key ", " #base ", " #nelp ", " #width ", " #compar              \
                                ", " #context ")")
#endif

#if defined(__cplusplus)
}
#endif //__cplusplus
