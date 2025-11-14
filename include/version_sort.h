// SPDX-License-Identifier: MPL-2.0

//! \file version_sort.h
//! \brief Provides GNU style version sort function. Slightly different name to
//! avoid conflict with other version sort implementations.
//!
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2025-2025 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "code_attributes.h"
#include "common_types.h"
#include "predef_env_detect.h"

#if !defined(_WIN32)
#    include <dirent.h>
#else
#    if !defined(NEED_OLD_SCANDIR_CMP_FUNC_TYPE)
#        define NEED_OLD_SCANDIR_CMP_FUNC_TYPE
#    endif // NEED_OLD_SCANDIR_CMP_FUNC_TYPE
#endif     //_WIN32

#if defined(__cplusplus)
extern "C"
{
#endif //__cplusplus

#if defined(NEED_OLD_SCANDIR_CMP_FUNC_TYPE)
    typedef int (*scandircmp)(const void*, const void*);
#else
typedef int (*scandircmp)(const struct dirent**, const struct dirent**);
#endif

//! \fn int version_sort(const void *ptr1, const void *ptr2)
//! \brief Works like GNU's versionsort comparison function.
//! Uses opensea-common's string_version_compare to compare two version strings.
//! \param[in] ptr1 Pointer to the first directory entry to compare
//! \param[in] ptr2 Pointer to the second directory entry to compare
//! \return An integer less than, equal to, or greater than zero if \a ptr1 is found, respectively, to be less than,
//! to match, or to be greater than \a ptr2.
//! \note On Windows, this function just returns 0 always since dirent is not supported.
#if defined(NEED_OLD_SCANDIR_CMP_FUNC_TYPE)
    int version_sort(const void* ptr1, const void* ptr2);
#else
int version_sort(const struct dirent** ptr1, const struct dirent** ptr2);
#endif

#if defined(__cplusplus)
}
#endif //__cplusplus
