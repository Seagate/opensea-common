// SPDX-License-Identifier: MPL-2.0

//! \file error_translation.h
//! \brief functions to translate error codes from errno, windows errors, or uefi errors to a string
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2025 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "code_attributes.h"
#include "common_types.h"

#if defined(__cplusplus)
extern "C"
{
#endif

    //! \brief returns an allocated buffer with translation of the errno value
    //!
    //! \param[in] error The error number to translate
    //! \return M_NULLPTR if memory cannot be allocated or error cannot be translated. Pointer to string successful.
    M_FUNC_ATTR_MALLOC char* get_strerror(errno_t error);

    //! \brief Prints the error number and its meaning to the screen.
    //!
    //! This function prints the error number and its meaning to the screen, followed by a newline character.
    //!
    //! \param[in] error The error number to print.
    void print_Errno_To_Screen(errno_t error);

// Some platforms have their own specific error codes that can be translated
#if defined(UEFI_C_SOURCE)
    //! \brief Prints the EFI status code and its meaning to the screen.
    //!
    //! This function prints the EFI status code and its meaning to the screen, followed by a newline character.
    //!
    //! \param[in] efiStatus The EFI status code to print.
    void print_EFI_STATUS_To_Screen(EFI_STATUS efiStatus);
#elif defined(_WIN32)
//! \brief Prints the Windows error code and its meaning to the screen.
//!
//! This function prints the Windows error code and its meaning to the screen, followed by a newline character.
//!
//! \param[in] windowsError The Windows error code to print.
void print_Windows_Error_To_Screen(winsyserror_t windowsError);
#endif

#if defined(__cplusplus)
}
#endif
