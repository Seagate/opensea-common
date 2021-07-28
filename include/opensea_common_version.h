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
// \file opensea_common_version.h
// \brief Defines the versioning information for opensea-transport API


#pragma once

#if defined (__cplusplus)
extern "C"
{
#endif

#define COMBINE_COMMON_VERSIONS_(x,y,z) #x "." #y "." #z
#define COMBINE_COMMON_VERSIONS(x,y,z) COMBINE_COMMON_VERSIONS_(x,y,z)

#define OPENSEA_COMMON_MAJOR_VERSION    1
#define OPENSEA_COMMON_MINOR_VERSION    20
#define OPENSEA_COMMON_PATCH_VERSION    0

#define OPENSEA_COMMON_VERSION COMBINE_COMMON_VERSIONS(OPENSEA_COMMON_MAJOR_VERSION,OPENSEA_COMMON_MINOR_VERSION,OPENSEA_COMMON_PATCH_VERSION)

#if defined (__cplusplus)
} //extern "C"
#endif

