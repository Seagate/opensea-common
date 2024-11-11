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
// \file error_translation.c
// \brief functions to translate error codes from errno, windows errors, or uefi
// errors to a string

#include "error_translation.h"
#include "memory_safety.h"
#include "type_conversion.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#if defined(UEFI_C_SOURCE)
#    include <Uefi.h>
#    include <Uefi/UefiBaseType.h> //for EFI_STATUS definitions
#elif defined(_WIN32)
#    include <tchar.h>
#    include <windows.h>
#endif

#define ERROR_STRING_BUFFER_LENGTH SIZE_T_C(1024)
void print_Errno_To_Screen(errno_t error)
{
#if defined(HAVE_C11_ANNEX_K)
    size_t errorStringLen = strerrorlen_s(error);
    if (errorStringLen > 0 && errorStringLen < RSIZE_MAX)
    {
        char* errorString = M_REINTERPRET_CAST(char*, safe_calloc(errorStringLen + SIZE_T_C(1), sizeof(char)));
        if (errorString)
        {
            errno_t truncated = strerror_s(errorString, errorStringLen + SIZE_T_C(1), error);
            if (truncated != 0)
            {
                printf("%d - %s (truncated)\n", error, errorString);
            }
            else
            {
                printf("%d - %s\n", error, errorString);
            }
        }
        else
        {
            printf("%d - <Unable to convert error to string>\n", error);
        }
        safe_free(&errorString);
    }
    else
    {
        printf("%d - <Unable to convert error to string>\n", error);
    }
#elif defined(HAVE_MSFT_SECURE_LIB)
    DECLARE_ZERO_INIT_ARRAY(char, errorString, ERROR_STRING_BUFFER_LENGTH);
    if (0 == strerror_s(errorString, ERROR_STRING_BUFFER_LENGTH, error))
    {
        printf("%d - %s\n", error, errorString);
    }
    else
    {
        printf("%d - <Unable to convert error to string>\n", error);
    }
#elif defined(HAVE_POSIX_STRERR_R) || defined(HAVE_GNU_STRERR_R) || defined(POSIX_2001) || defined(_GNU_SOURCE)
#    if defined(HAVE_POSIX_STRERR_R) && defined(HAVE_GNU_STRERR_R)
#        error                                                                                                         \
            "strerror_r is either POSIX or GNU compatible, but not both. Define only one of HAVE_POSIX_STRERR_R or HAVE_GNU_STRERR_R"
#    endif // HAVE_POSIX_STRERR_R && HAVE_GNU_STRERR_R
// GNU/POSIX strerror_r. Beware, GNU version works different-TJE
// POSIX version available since glibc 2.3.4, but not POSIX-compliant until
// glibc 2.13 GNU version available since glibc 2.0
// https://www.man7.org/linux/man-pages/man3/strerror.3.html
// https://pubs.opengroup.org/onlinepubs/9699919799/functions/strerror.html
// https://www.gnu.org/software/gnulib/manual/html_node/strerror_005fr.html
// There is brief mention of xstrerror, but I cannot find any documentation on
// it anywhere.-TJE
#    if (defined(POSIX_2001) && !defined(_GNU_SOURCE)) || defined(HAVE_POSIX_STRERR_R)
    // POSIX version
    DECLARE_ZERO_INIT_ARRAY(char, errorString, ERROR_STRING_BUFFER_LENGTH);
    if (0 == strerror_r(error, errorString, ERROR_STRING_BUFFER_LENGTH))
    {
        errorString[ERROR_STRING_BUFFER_LENGTH - SIZE_T_C(1)] = '\0'; // While it should be null terminated, there are known bugs
                                                            // on some systems where it is not!
        printf("%d - %s\n", error, errorString);
    }
    else
    {
        printf("%d - <Unable to convert error to string>\n", error);
    }
#    elif defined(HAVE_GNU_STRERR_R) || defined(_GNU_SOURCE)
    // GNU version
    DECLARE_ZERO_INIT_ARRAY(char, errorString, ERROR_STRING_BUFFER_LENGTH);
    char* errmsg = strerror_r(error, errorString, ERROR_STRING_BUFFER_LENGTH);
    if (errmsg != M_NULLPTR)
    {
        errorString[ERROR_STRING_BUFFER_LENGTH - SIZE_T_C(1)] = '\0'; // While it should be null terminated, there are known bugs
                                                            // on some systems where it is not!
        printf("%d - %s\n", error, errmsg);
    }
    else
    {
        printf("%d - <Unable to convert error to string>\n", error);
    }
#    else
#        error "Error detectiong POSIX strerror_r vs GNU strerror_r"
#    endif
#else
    char* temp = strerror(error);
    if (temp != M_NULLPTR)
    {
        printf("%d - %s\n", error, temp);
    }
    else
    {
        printf("%d - <Unable to convert error to string>\n", error);
    }
#endif
}

#if defined(UEFI_C_SOURCE)
// Use %d to print this out or the output look really strange
#    define PRI_UINTN "d"
void print_EFI_STATUS_To_Screen(EFI_STATUS efiStatus)
{
    switch (efiStatus)
    {
    case EFI_SUCCESS:
        printf("%" PRI_UINTN " - Success\n", efiStatus);
        break;
    case EFI_LOAD_ERROR:
        printf("%" PRI_UINTN " - Load Error\n", efiStatus);
        break;
    case EFI_INVALID_PARAMETER:
        printf("%" PRI_UINTN " - Invalid Parameter\n", efiStatus);
        break;
    case EFI_UNSUPPORTED:
        printf("%" PRI_UINTN " - Unsupported\n", efiStatus);
        break;
    case EFI_BAD_BUFFER_SIZE:
        printf("%" PRI_UINTN " - Bad Buffer Size\n", efiStatus);
        break;
    case EFI_BUFFER_TOO_SMALL:
        printf("%" PRI_UINTN " - Buffer Too Small\n", efiStatus);
        break;
    case EFI_NOT_READY:
        printf("%" PRI_UINTN " - Not Ready\n", efiStatus);
        break;
    case EFI_DEVICE_ERROR:
        printf("%" PRI_UINTN " - Device Error\n", efiStatus);
        break;
    case EFI_WRITE_PROTECTED:
        printf("%" PRI_UINTN " - Write Protected\n", efiStatus);
        break;
    case EFI_OUT_OF_RESOURCES:
        printf("%" PRI_UINTN " - Out Of Resources\n", efiStatus);
        break;
    case EFI_VOLUME_CORRUPTED:
        printf("%" PRI_UINTN " - Volume Corrupted\n", efiStatus);
        break;
    case EFI_VOLUME_FULL:
        printf("%" PRI_UINTN " - Volume Full\n", efiStatus);
        break;
    case EFI_NO_MEDIA:
        printf("%" PRI_UINTN " - No Media\n", efiStatus);
        break;
    case EFI_MEDIA_CHANGED:
        printf("%" PRI_UINTN " - Media Changed\n", efiStatus);
        break;
    case EFI_NOT_FOUND:
        printf("%" PRI_UINTN " - Not Found\n", efiStatus);
        break;
    case EFI_ACCESS_DENIED:
        printf("%" PRI_UINTN " - Access Denied\n", efiStatus);
        break;
    case EFI_NO_RESPONSE:
        printf("%" PRI_UINTN " - No Response\n", efiStatus);
        break;
    case EFI_NO_MAPPING:
        printf("%" PRI_UINTN " - No Mapping\n", efiStatus);
        break;
    case EFI_TIMEOUT:
        printf("%" PRI_UINTN " - Timeout\n", efiStatus);
        break;
    case EFI_NOT_STARTED:
        printf("%" PRI_UINTN " - Not Started\n", efiStatus);
        break;
    case EFI_ALREADY_STARTED:
        printf("%" PRI_UINTN " - Already Started\n", efiStatus);
        break;
    case EFI_ABORTED:
        printf("%" PRI_UINTN " - Aborted\n", efiStatus);
        break;
    case EFI_ICMP_ERROR:
        printf("%" PRI_UINTN " - ICMP Error\n", efiStatus);
        break;
    case EFI_TFTP_ERROR:
        printf("%" PRI_UINTN " - TFTP Error\n", efiStatus);
        break;
    case EFI_PROTOCOL_ERROR:
        printf("%" PRI_UINTN " - Protocol Error\n", efiStatus);
        break;
    case EFI_INCOMPATIBLE_VERSION:
        printf("%" PRI_UINTN " - Incompatible Version\n", efiStatus);
        break;
    case EFI_SECURITY_VIOLATION:
        printf("%" PRI_UINTN " - Security Violation\n", efiStatus);
        break;
    case EFI_CRC_ERROR:
        printf("%" PRI_UINTN " - CRC Error\n", efiStatus);
        break;
    case EFI_END_OF_MEDIA:
        printf("%" PRI_UINTN " - End Of Media\n", efiStatus);
        break;
    case EFI_END_OF_FILE:
        printf("%" PRI_UINTN " - End Of File\n", efiStatus);
        break;
    case EFI_INVALID_LANGUAGE:
        printf("%" PRI_UINTN " - Invalid Language\n", efiStatus);
        break;
    case EFI_COMPROMISED_DATA:
        printf("%" PRI_UINTN " - Compromised Data\n", efiStatus);
        break;
    case EFI_HTTP_ERROR:
        printf("%" PRI_UINTN " - HTTP Error\n", efiStatus);
        break;
    case EFI_WARN_UNKNOWN_GLYPH:
        printf("%" PRI_UINTN " - Warning Unknown Glyph\n", efiStatus);
        break;
    case EFI_WARN_DELETE_FAILURE:
        printf("%" PRI_UINTN " - Warning Delete Failure\n", efiStatus);
        break;
    case EFI_WARN_WRITE_FAILURE:
        printf("%" PRI_UINTN " - Warning Write Failure\n", efiStatus);
        break;
    case EFI_WARN_BUFFER_TOO_SMALL:
        printf("%" PRI_UINTN " - Warning Buffer Too Small\n", efiStatus);
        break;
    case EFI_WARN_STALE_DATA:
        printf("%" PRI_UINTN " - Warning Stale Data\n", efiStatus);
        break;
    case EFI_WARN_FILE_SYSTEM:
        printf("%" PRI_UINTN " - Warning File System\n", efiStatus);
        break;
    default:
        printf("%" PRI_UINTN " - Unknown EFI Status value\n", efiStatus);
        break;
    }
}

#elif defined(_WIN32)
void print_Windows_Error_To_Screen(winsyserror_t windowsError)
{
    LPTSTR windowsErrorString = M_NULLPTR;
    // Originally used: MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
    // switched to MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) to keep output
    // consistent with all other verbose output.-TJE I would love to remove the
    // cast, but I cannot figure out how to do it and keep out the warnings from
    // MSVC - TJE
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                  M_NULLPTR, windowsError, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
                  C_CAST(LPTSTR, &windowsErrorString), 0, M_NULLPTR);
    // This cast is necessary to tell the Windows API to allocate the string,
    // but it is necessary. Without it, this will not work.
    _tprintf_s(TEXT("%lu - %s\n"), windowsError, windowsErrorString);
    LocalFree(windowsErrorString);
}
#endif
