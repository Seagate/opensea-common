#pragma once

#include "code_attributes.h"
#include "common_types.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#if !defined(COMPARE_FUNC_TYPES)
    typedef int (*comparefn)(const void*, const void*);

    typedef int (*ctxcomparefn)(const void*, const void*, void*);
#    define COMPARE_FUNC_TYPES
#endif // COMPARE_FUNC_TYPES

    // regular qsort without context, but added bounds checking
    errno_t safe_qsort_impl(void*       ptr,
                            rsize_t     count,
                            rsize_t     size,
                            comparefn   compare,
                            const char* file,
                            const char* function,
                            int         line,
                            const char* expression);

    // like qsort_s with bounds checking and optional context parameter
    errno_t safe_qsort_context_impl(void*        ptr,
                                    rsize_t      count,
                                    rsize_t      size,
                                    ctxcomparefn compare,
                                    void*        context,
                                    const char*  file,
                                    const char*  function,
                                    int          line,
                                    const char*  expression);

    // regular bsearch without context, but added bounds checking
    void* safe_bsearch_impl(const void* key,
                            const void* ptr,
                            rsize_t     count,
                            rsize_t     size,
                            comparefn   compare,
                            const char* file,
                            const char* function,
                            int         line,
                            const char* expression);

    // like bsearch_s with bounds checking and optional context parameter
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

    void* safe_lsearch_impl(const void* key,
                            void*       base,
                            size_t*     nelp,
                            size_t      width,
                            comparefn   compar,
                            const char* file,
                            const char* function,
                            int         line,
                            const char* expression);

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

    void* safe_lfind_impl(const void* key,
                          const void* base,
                          size_t*     nelp,
                          size_t      width,
                          comparefn   compar,
                          const char* file,
                          const char* function,
                          int         line,
                          const char* expression);

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
