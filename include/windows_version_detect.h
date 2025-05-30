// SPDX-License-Identifier: MPL-2.0

//! \file windows_version_detect.h
//! \brief Definitions and Windows specific functions regarding detection of the Windows version
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2025 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "env_detect.h"

#if defined(__cplusplus)
extern "C"
{
#endif

    //! \fn eReturnValues read_Win_Version(ptrOSVersionNumber versionNumber)
    //! \brief Reads the Windows operating system version number
    //! \param[in,out] versionNumber pointer to OS version number structure that will hold the result
    //! of reading the version number from the OS.
    //! \return SUCCESS means successfully read the version number. Other values indicate a failure.
    M_NONNULL_PARAM_LIST(1) M_PARAM_WO(1) eReturnValues read_Win_Version(ptrOSVersionNumber versionNumber);

    //! \fn bool is_Windows_Vista_Or_Higher(void)
    //! \brief checks if the current execution environment is Windows vista or higher
    //! \return true means Windows Vista or later, false means OS earlier than Vista
    bool is_Windows_Vista_Or_Higher(void);

    //! \fn bool is_Windows_7_Or_Higher(void)
    //! \brief checks if the current execution environment is Windows 7 or higher
    //! \return true means Windows 7 or later, false means OS earlier than 7
    bool is_Windows_7_Or_Higher(void);

    //! \fn bool is_Windows_8_Or_Higher(void)
    //! \brief checks if the current execution environment is Windows 8 or higher
    //! \return true means Windows 8 or later, false means OS earlier than 8
    bool is_Windows_8_Or_Higher(void);

    //! \fn bool is_Windows_8_One_Or_Higher(void)
    //! \brief Checks if the current OS is Windows 8.1 or higher.
    //! \return true means Windows 8.1 or later, false means OS earlier than 8.1
    bool is_Windows_8_One_Or_Higher(void);

    //! \fn bool is_Windows_10_Or_Higher(void)
    //! \brief Checks if the current OS is Windows 10 or higher.
    //! \return true means Windows 10 or later, false means OS earlier than 10
    bool is_Windows_10_Or_Higher(void);

    //! \fn bool is_Windows_10_Version_1507_Or_Higher(void)
    //! \brief Checks if the current OS is Windows 10 Version 1507 or higher.
    //! \return true means Windows 10 Version 1507 or later, false means OS earlier than 1507
    bool is_Windows_10_Version_1507_Or_Higher(void);

    //! \fn bool is_Windows_10_Version_1511_Or_Higher(void)
    //! \brief Checks if the current OS is Windows 10 Version 1511 or higher.
    //! \return true means Windows 10 Version 1511 or later, false means OS earlier than 1511
    bool is_Windows_10_Version_1511_Or_Higher(void);

    //! \fn bool is_Windows_10_Version_1607_Or_Higher(void)
    //! \brief Checks if the current OS is Windows 10 Version 1607 or higher.
    //! \return true means Windows 10 Version 1607 or later, false means OS earlier than 1607
    bool is_Windows_10_Version_1607_Or_Higher(void);

    //! \fn bool is_Windows_10_Version_1703_Or_Higher(void)
    //! \brief Checks if the current OS is Windows 10 Version 1703 or higher.
    //! \return true means Windows 10 Version 1703 or later, false means OS earlier than 1703
    bool is_Windows_10_Version_1703_Or_Higher(void);

    //! \fn bool is_Windows_10_Version_1709_Or_Higher(void)
    //! \brief Checks if the current OS is Windows 10 Version 1709 or higher.
    //! \return true means Windows 10 Version 1709 or later, false means OS earlier than 1709
    bool is_Windows_10_Version_1709_Or_Higher(void);

    //! \fn bool is_Windows_10_Version_1803_Or_Higher(void)
    //! \brief Checks if the current OS is Windows 10 Version 1803 or higher.
    //! \return true means Windows 10 Version 1803 or later, false means OS earlier than 1803
    bool is_Windows_10_Version_1803_Or_Higher(void);

    //! \fn bool is_Windows_10_Version_1809_Or_Higher(void)
    //! \brief Checks if the current OS is Windows 10 Version 1809 or higher.
    //! \return true means Windows 10 Version 1809 or later, false means OS earlier than 1809
    bool is_Windows_10_Version_1809_Or_Higher(void);

    //! \fn bool is_Windows_10_Version_1903_Or_Higher(void)
    //! \brief Checks if the current OS is Windows 10 Version 1903 or higher.
    //! \return true means Windows 10 Version 1903 or later, false means OS earlier than 1903
    bool is_Windows_10_Version_1903_Or_Higher(void);

    //! \fn bool is_Windows_10_Version_1909_Or_Higher(void)
    //! \brief Checks if the current OS is Windows 10 Version 1909 or higher.
    //! \return true means Windows 10 Version 1909 or later, false means OS earlier than 1909
    bool is_Windows_10_Version_1909_Or_Higher(void);

    //! \fn bool is_Windows_10_Version_2004_Or_Higher(void)
    //! \brief Checks if the current OS is Windows 10 Version 2004 or higher.
    //! \return true means Windows 10 Version 2004 or later, false means OS earlier than 2004
    bool is_Windows_10_Version_2004_Or_Higher(void);

    //! \fn bool is_Windows_10_Version_20H2_Or_Higher(void)
    //! \brief Checks if the current OS is Windows 10 Version 20H2 or higher.
    //! \return true means Windows 10 Version 20H2 or later, false means OS earlier than 20H2
    bool is_Windows_10_Version_20H2_Or_Higher(void);

    //! \fn bool is_Windows_10_Version_21H1_Or_Higher(void)
    //! \brief Checks if the current OS is Windows 10 Version 21H1 or higher.
    //! \return true means Windows 10 Version 21H1 or later, false means OS earlier than 21H1
    bool is_Windows_10_Version_21H1_Or_Higher(void);

    //! \fn bool is_Windows_10_Version_21H2_Or_Higher(void)
    //! \brief Checks if the current OS is Windows 10 Version 21H2 or higher.
    //! \return true means Windows 10 Version 21H2 or later, false means OS earlier than 21H2
    bool is_Windows_10_Version_21H2_Or_Higher(void);

    //! \fn bool is_Windows_10_Version_22H2_Or_Higher(void)
    //! \brief Checks if the current OS is Windows 10 Version 22H2 or higher.
    //! \return true means Windows 10 Version 22H2 or later, false means OS earlier than 22H2
    bool is_Windows_10_Version_22H2_Or_Higher(void);

    //! \fn bool is_Windows_11_Version_21H2_Or_Higher(void)
    //! \brief Checks if the current OS is Windows 11 Version 21H2 or higher.
    //! \return true means Windows 11 Version 21H2 or later, false means OS earlier than 21H2
    bool is_Windows_11_Version_21H2_Or_Higher(void);

    //! \fn bool is_Windows_11_Version_22H2_Or_Higher(void)
    //! \brief Checks if the current OS is Windows 11 Version 22H2 or higher.
    //! \return true means Windows 11 Version 22H2 or later, false means OS earlier than 22H2
    bool is_Windows_11_Version_22H2_Or_Higher(void);

    //! \fn bool is_Windows_11_Version_23H2_Or_Higher(void)
    //! \brief Checks if the current OS is Windows 11 Version 23H2 or higher.
    //! \return true means Windows 11 Version 23H2 or later, false means OS earlier than 23H2
    bool is_Windows_11_Version_23H2_Or_Higher(void);

    //! \fn bool is_Windows_PE(void)
    //! \brief Checks if the application is currently running in the Windows Pre-Installation Environment (PE).
    //! \return true means running in PE, false means running in standard Windows
    bool is_Windows_PE(void);

    //! \fn bool is_Windows_Server_OS(void)
    //! \brief Checks if the current OS is a Windows Server operating system.
    //! \return true means Windows Server OS, false means non-server OS
    bool is_Windows_Server_OS(void);

#if defined(__cplusplus)
}
#endif
