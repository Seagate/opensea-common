// SPDX-License-Identifier: MPL-2.0
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2024 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
// 
// \file error_translation.h
// \brief functions to translate error codes from errno, windows errors, or uefi errors to a string

#pragma once

#include "common_types.h"

#if defined (__cplusplus)
extern "C"
{
#endif

    //-----------------------------------------------------------------------------
    //
    //  print_Errno_To_Screen(errno_t error);
    //
    //! \brief   Description:  Prints the error number and it's meaning to the screen followed by a newline character
    //
    //  Entry:
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void print_Errno_To_Screen(errno_t error);

    //Some platforms have their own specific error codes that can be translated
#if defined (UEFI_C_SOURCE)
    //-----------------------------------------------------------------------------
    //
    //  print_EFI_STATUS_To_Screen(EFI_STATUS efiStatus)
    //
    //! \brief   Description:  Prints the error number and it's meaning to the screen followed by a newline character
    //
    //  Entry:
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void print_EFI_STATUS_To_Screen(EFI_STATUS efiStatus);
#elif defined (_WIN32)
    //-----------------------------------------------------------------------------
    //
    //  print_Windows_Error_To_Screen(unsigned int windowsError)
    //
    //! \brief   Description:  Prints the error number and it's meaning to the screen followed by a newline character
    //
    //  Entry:
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void print_Windows_Error_To_Screen(winsyserror_t windowsError);
#endif

#if defined (__cplusplus)
}
#endif
