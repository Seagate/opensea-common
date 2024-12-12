#pragma once

#include "code_attributes.h"
#include "common_types.h"

#include <stdio.h>

#if defined(__cplusplus)
extern "C"
{
#endif

    errno_t safe_fopen_impl(FILE* M_RESTRICT* M_RESTRICT streamptr,
                            const char* M_RESTRICT       filename,
                            const char* M_RESTRICT       mode,
                            const char*                  file,
                            const char*                  function,
                            int                          line,
                            const char*                  expression);

    errno_t safe_freopen_impl(FILE* M_RESTRICT* M_RESTRICT newstreamptr,
                              const char* M_RESTRICT       filename,
                              const char* M_RESTRICT       mode,
                              FILE* M_RESTRICT             stream,
                              const char*                  file,
                              const char*                  function,
                              int                          line,
                              const char*                  expression);

#if defined(WANT_SAFE_TMPNAM)
    errno_t safe_tmpnam_impl(char*       filename_s,
                             rsize_t     maxsize,
                             const char* file,
                             const char* function,
                             int         line,
                             const char* expression);
#endif // WANT_SAFE_TMPNAM

    errno_t safe_tmpfile_impl(FILE* M_RESTRICT* M_RESTRICT streamptr,
                              const char*                  file,
                              const char*                  function,
                              int                          line,
                              const char*                  expression);

    char* safe_gets_impl(char*       str,
                         rsize_t     n,
                         const char* file,
                         const char* function,
                         int         line,
                         const char* expression);

    errno_t safe_strtol_impl(long*                  value,
                             const char* M_RESTRICT str,
                             char** M_RESTRICT      endp,
                             int                    base,
                             const char*            file,
                             const char*            function,
                             int                    line,
                             const char*            expression);

    errno_t safe_strtoll_impl(long long*             value,
                              const char* M_RESTRICT str,
                              char** M_RESTRICT      endp,
                              int                    base,
                              const char*            file,
                              const char*            function,
                              int                    line,
                              const char*            expression);

    errno_t safe_strtoul_impl(unsigned long*         value,
                              const char* M_RESTRICT str,
                              char** M_RESTRICT      endp,
                              int                    base,
                              const char*            file,
                              const char*            function,
                              int                    line,
                              const char*            expression);

    errno_t safe_strtoull_impl(unsigned long long*    value,
                               const char* M_RESTRICT str,
                               char** M_RESTRICT      endp,
                               int                    base,
                               const char*            file,
                               const char*            function,
                               int                    line,
                               const char*            expression);

    errno_t safe_strtoimax_impl(intmax_t*              value,
                                const char* M_RESTRICT str,
                                char** M_RESTRICT      endp,
                                int                    base,
                                const char*            file,
                                const char*            function,
                                int                    line,
                                const char*            expression);

    errno_t safe_strtoumax_impl(uintmax_t*             value,
                                const char* M_RESTRICT str,
                                char** M_RESTRICT      endp,
                                int                    base,
                                const char*            file,
                                const char*            function,
                                int                    line,
                                const char*            expression);

    errno_t safe_strtof_impl(float*                 value,
                             const char* M_RESTRICT str,
                             char** M_RESTRICT      endp,
                             const char*            file,
                             const char*            function,
                             int                    line,
                             const char*            expression);

    errno_t safe_strtod_impl(double*                value,
                             const char* M_RESTRICT str,
                             char** M_RESTRICT      endp,
                             const char*            file,
                             const char*            function,
                             int                    line,
                             const char*            expression);

    errno_t safe_strtold_impl(long double*           value,
                              const char* M_RESTRICT str,
                              char** M_RESTRICT      endp,
                              const char*            file,
                              const char*            function,
                              int                    line,
                              const char*            expression);

#if defined(__cplusplus)
}
#endif
