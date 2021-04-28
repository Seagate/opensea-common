//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2018-2021 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
// 
// \file common_uefi.h
// \brief Definitions and function calls (and includes) specific to UEFI

#pragma once

//TODO: UEFI includes

#if defined (__cplusplus)
extern "C"
{
#endif

    #include <sys/syslimits.h>
    #define OPENSEA_PATH_MAX PATH_MAX //1024

    #define SYSTEM_PATH_SEPARATOR '/'
    #define SYSTEM_PATH_SEPARATOR_STR "/"

    void print_EFI_STATUS_To_Screen(EFI_STATUS efiStatus);

#if defined (__cplusplus)
}
#endif
