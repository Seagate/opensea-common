// SPDX-License-Identifier: MPL-2.0

//! \file opensea_common_version.h
//! \brief Defines the version information for opensea-common API
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2024 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#if defined(__cplusplus)
extern "C"
{
#endif

//! \def COMBINE_COMMON_VERSIONS_(major, minor, patch)
//! \brief Combines major, minor, and patch version numbers into a string.
//! \param major Major version number.
//! \param minor Minor version number.
//! \param patch Patch version number.
//! \return A string in the format "major.minor.patch".
#define COMBINE_COMMON_VERSIONS_(major, minor, patch) #major "." #minor "." #patch

//! \def COMBINE_COMMON_VERSIONS(major, minor, patch)
//! \brief Macro to combine major, minor, and patch version numbers into a string using COMBINE_COMMON_VERSIONS_.
//! \param major Major version number.
//! \param minor Minor version number.
//! \param patch Patch version number.
//! \return A string in the format "major.minor.patch".
#define COMBINE_COMMON_VERSIONS(major, minor, patch) COMBINE_COMMON_VERSIONS_(major, minor, patch)

//! \def OPENSEA_COMMON_MAJOR_VERSION
//! \brief Defines the major version number for the opensea-common library.
#define OPENSEA_COMMON_MAJOR_VERSION 6

//! \def OPENSEA_COMMON_MINOR_VERSION
//! \brief Defines the minor version number for the opensea-common library.
#define OPENSEA_COMMON_MINOR_VERSION 0

//! \def OPENSEA_COMMON_PATCH_VERSION
//! \brief Defines the patch version number for the opensea-common library.
#define OPENSEA_COMMON_PATCH_VERSION 0

//! \def OPENSEA_COMMON_VERSION
//! \brief Combines the major, minor, and patch version numbers into a single version string for the opensea-common
//! library. \return A string in the format "major.minor.patch".
#define OPENSEA_COMMON_VERSION                                                                                         \
    COMBINE_COMMON_VERSIONS(OPENSEA_COMMON_MAJOR_VERSION, OPENSEA_COMMON_MINOR_VERSION, OPENSEA_COMMON_PATCH_VERSION)

#if defined(__cplusplus)
} // extern "C"
#endif
