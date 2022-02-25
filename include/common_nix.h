//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2012-2022 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
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

    #include <limits.h>
    #define OPENSEA_PATH_MAX PATH_MAX
    
    #define SYSTEM_PATH_SEPARATOR '/'
    #define SYSTEM_PATH_SEPARATOR_STR "/"

#if defined (__cplusplus)
}
#endif
