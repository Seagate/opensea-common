//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2012-2021 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
// 
// \file common_nix.h
// \brief Definitions and function calls (and includes) specific to different Unix or Linux OSs

#pragma once

#if defined (__cplusplus)
extern "C"
{
#endif

    #if defined(__linux__)
    /*
    Please note that GCC doesn't really have a PATH_MAX & recommends path buffers be allocated dynamically.
    Also, this #if #else might need to expand to cater to nauances of BSD/HP UX etc.
    */
    #include <linux/limits.h>
    #define OPENSEA_PATH_MAX PATH_MAX
    #else 

    #include <limits.h>
    #define OPENSEA_PATH_MAX PATH_MAX
    #endif

    #define SYSTEM_PATH_SEPARATOR '/'
    #define SYSTEM_PATH_SEPARATOR_STR "/"

#if defined (__cplusplus)
}
#endif
