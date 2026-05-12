// SPDX-License-Identifier: MPL-2.0

//! \file sort_and_search.c
//! \brief Provides safe sort and search function definitions.
//!
//! These work similar to C11's Annex K checking bounds, checking for valid pointers, etc.
//!
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2026 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "sort_and_search.h"
#include "code_attributes.h"
#include "common_types.h"
#include "constraint_handling.h"
#include "math_utils.h"
#include "type_conversion.h"
#include <stdlib.h>

// regular qsort without context, but added checks for qsort_s
M_PARAM_RW(1)
CONSTRAINT_NO_DISCARD
errno_t safe_qsort_impl(void* M_NONNULL        ptr,
                        rsize_t                count,
                        rsize_t                size,
                        comparefn M_NONNULL    compare,
                        const char* M_NULLABLE file,
                        const char* M_NULLABLE function,
                        int                    line,
                        const char* M_NULLABLE expression)
{
    errno_t           error = 0;
    constraintEnvInfo envInfo;
    if (count > RSIZE_T_C(0) && ptr == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_qsort: count > 0 && ptr == NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (count > RSIZE_T_C(0) && compare == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_qsort: count > 0 && compare == NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (count > RSIZE_MAX)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_qsort: count > RSIZE_MAX",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (size > RSIZE_MAX)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_qsort: size > RSIZE_MAX",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else
    {
        errno = 0;
        if (count > RSIZE_T_C(0))
        {
            qsort(ptr, count, size, compare);
        }
        return 0;
    }
}

// regular bsearch without context, but added checks for bsearch_s
M_PARAM_RO(1)
M_PARAM_RO(2)
CONSTRAINT_NO_DISCARD
void* M_NULLABLE safe_bsearch_impl(const void* M_NONNULL  key,
                                   const void* M_NONNULL  ptr,
                                   rsize_t                count,
                                   rsize_t                size,
                                   comparefn M_NONNULL    compare,
                                   const char* M_NULLABLE file,
                                   const char* M_NULLABLE function,
                                   int                    line,
                                   const char* M_NULLABLE expression)
{
    errno_t           error = 0;
    constraintEnvInfo envInfo;
    if (count > RSIZE_T_C(0) && ptr == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_bsearch: count > 0 && ptr == NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return M_NULLPTR;
    }
    else if (count > RSIZE_T_C(0) && compare == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_bsearch: count > 0 && compare == NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return M_NULLPTR;
    }
    else if (count > RSIZE_T_C(0) && key == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_bsearch: count > 0 && key == NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return M_NULLPTR;
    }
    else if (count > RSIZE_MAX)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_bsearch: count > RSIZE_MAX",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return M_NULLPTR;
    }
    else if (size > RSIZE_MAX)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_bsearch: size > RSIZE_MAX",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return M_NULLPTR;
    }
    else
    {
        errno = 0;
        if (count > RSIZE_T_C(0))
        {
            // Const cast to deal with changes to bseach for const and non-const searches in C23
            return M_CONST_CAST(void*, bsearch(key, ptr, count, size, compare));
        }
        else
        {
            return M_NULLPTR;
        }
    }
}
