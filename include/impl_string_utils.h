#pragma once

#include "code_attributes.h"
#include "common_types.h"

#if defined(__cplusplus)
extern "C"
{
#endif

    errno_t safe_strcpy_impl(char* M_RESTRICT       dest,
                             rsize_t                destsz,
                             const char* M_RESTRICT src,
                             const char*            file,
                             const char*            function,
                             int                    line,
                             const char*            expression);

    errno_t safe_strmove_impl(char*       dest,
                              rsize_t     destsz,
                              const char* src,
                              const char* file,
                              const char* function,
                              int         line,
                              const char* expression);

    errno_t safe_strncpy_impl(char* M_RESTRICT       dest,
                              rsize_t                destsz,
                              const char* M_RESTRICT src,
                              rsize_t                count,
                              const char*            file,
                              const char*            function,
                              int                    line,
                              const char*            expression);

    errno_t safe_strnmove_impl(char*       dest,
                               rsize_t     destsz,
                               const char* src,
                               rsize_t     count,
                               const char* file,
                               const char* function,
                               int         line,
                               const char* expression);

    errno_t safe_strcat_impl(char* M_RESTRICT       dest,
                             rsize_t                destsz,
                             const char* M_RESTRICT src,
                             const char*            file,
                             const char*            function,
                             int                    line,
                             const char*            expression);

    errno_t safe_strncat_impl(char* M_RESTRICT       dest,
                              rsize_t                destsz,
                              const char* M_RESTRICT src,
                              rsize_t                count,
                              const char*            file,
                              const char*            function,
                              int                    line,
                              const char*            expression);

    char* safe_String_Token_impl(char* M_RESTRICT       str,
                                 rsize_t* M_RESTRICT    strmax,
                                 const char* M_RESTRICT delim,
                                 char** M_RESTRICT      saveptr,
                                 const char*            file,
                                 const char*            function,
                                 int                    line,
                                 const char*            expression);

    errno_t safe_strdup_impl(char**      dup,
                             const char* src,
                             const char* file,
                             const char* function,
                             int         line,
                             const char* expression);

    errno_t safe_strndup_impl(char**      dup,
                              const char* src,
                              rsize_t     size,
                              const char* file,
                              const char* function,
                              int         line,
                              const char* expression);

#if defined(__cplusplus)
}
#endif
