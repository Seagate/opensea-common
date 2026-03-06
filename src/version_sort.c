// SPDX-License-Identifier: MPL-2.0

//! \file version_sort.c
//! \brief Provides GNU style version sort function. Slightly different name to
//! avoid conflict with other version sort implementations.
//!
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2025-2026 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "version_sort.h"
#include "string_utils.h"

#if defined(NEED_OLD_SCANDIR_CMP_FUNC_TYPE)
int version_sort(const void* ptr1, const void* ptr2)
{
#    if defined(_WIN32)
    M_USE_UNUSED(ptr1);
    M_USE_UNUSED(ptr2);
    return 0;
#    else
    const struct dirent* const* a = M_REINTERPRET_CAST(const struct dirent* const*, ptr1);
    const struct dirent* const* b = M_REINTERPRET_CAST(const struct dirent* const*, ptr2);
    return string_version_compare((*a)->d_name, (*b)->d_name);
#    endif // _WIN32
}
#else
int version_sort(const struct dirent** ptr1, const struct dirent** ptr2)
{
    return string_version_compare((*ptr1)->d_name, (*ptr2)->d_name);
}
#endif // NEED_OLD_SCANDIR_CMP_FUNC_TYPE
