// SPDX-License-Identifier: MPL-2.0
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2024-2024 Seagate Technology LLC and/or its Affiliates, All
// Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
//
// \file uefi_env_detect.c
// \brief UEFI environment detection implementation for env_detect.h

#include "bit_manip.h"
#include "common_types.h"
#include "env_detect.h"
#include "io_utils.h"
#include "string_utils.h"
#include "type_conversion.h"

// UEFI EDK includes
#include <Library/PrintLib.h> //to convert CHAR16 string to CHAR8. may also be able to use stdlib in someway, but this seems to work
#include <Library/UefiBootServicesTableLib.h> //to get global boot services pointer. This pointer should be checked before use, but any app using stdlib will have this set.
#include <Protocol/SimpleTextOut.h>           //for colors
#include <Uefi.h>
#include <Uefi/UefiBaseType.h> //for EFI_STATUS definitions

#define UEFI_FW_VENDOR_STR_LEN 80
eReturnValues get_Operating_System_Version_And_Name(ptrOSVersionNumber versionNumber, char* operatingSystemName)
{
    // get the UEFI version information and return it.
    // Also, set the operating system name to "UEFI" or "EFI" (If old enough
    // version)
    versionNumber->osVersioningIdentifier               = OS_UEFI;
    versionNumber->versionType.uefiVersion.majorVersion = M_Word1(gST->Hdr.Revision);
    versionNumber->versionType.uefiVersion.minorVersion = M_Word0(gST->Hdr.Revision);
    // gST->FirmwareVendor and gST->FirmwareRevision should be part of the
    // operating system name! The FirmwareVendor is a CHAR16 string, so it needs
    // conversion. The revision is a UINT32 and stores some vendor specific
    // version data (display as hex)
    DECLARE_ZERO_INIT_ARRAY(char, firmwareVendorASCII, UEFI_FW_VENDOR_STR_LEN);
    AsciiSPrintUnicodeFormat(firmwareVendorASCII, UEFI_FW_VENDOR_STR_LEN, L"%s", gST->FirmwareVendor);
    if (safe_strlen(firmwareVendorASCII) == 0)
    {
        snprintf_err_handle(firmwareVendorASCII, UEFI_FW_VENDOR_STR_LEN, "Unknown Firmware Vendor");
    }
    // NOLINTBEGIN(bugprone-branch-clone)
    // Turning off clang-tidy check for this branch. It's detected as the same,
    // but the string UEFI vs EFI is different which is helpful in this case-TJE
    if (versionNumber->versionType.uefiVersion.majorVersion >= 2)
    {
        snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "UEFI - %s - 0x%08" PRIX32, firmwareVendorASCII,
                            gST->FirmwareRevision);
    }
    else
    {
        snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "EFI - %s - 0x%08" PRIX32, firmwareVendorASCII,
                            gST->FirmwareRevision);
    }
    // NOLINTEND(bugprone-branch-clone)
    return SUCCESS;
}

bool is_Running_Elevated()
{
    return true; // In UEFI, there is no concept of admin or sudo. The getuid
                 // function always returns zero, and the geteuid function isn't
                 // present at all currently, so it's safe to just return true
                 // here.
}

#if defined(ENABLE_READ_USERNAME)
eReturnValues get_Current_User_Name(char** userName)
{
    eReturnValues ret = SUCCESS;
    if (userName)
    {
        // while unix functions are there, they are all stubs, so we're just
        // going to return "efi" as the username.
#define UEFI_USER_NAME_LENGTH 4
        *userName = C_CAST(char*, safe_calloc(UEFI_USER_NAME_LENGTH, sizeof(char)));
        if (*userName)
        {
            snprintf_err_handle(*userName, UEFI_USER_NAME_LENGTH, "efi");
        }
        else
        {
            ret = FAILURE;
        }
    }
    else
    {
        ret = BAD_PARAMETER;
    }
    return ret;
}
#endif // ENABLE_READ_USERNAME
