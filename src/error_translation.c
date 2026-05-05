// SPDX-License-Identifier: MPL-2.0

//! \file error_translation.c
//! \brief functions to translate error codes from errno, windows errors, or uefi errors to a string
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2026 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "error_translation.h"
#include "constraint_handling.h"
#include "memory_safety.h"
#include "string_utils.h"
#include "type_conversion.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#if defined(UEFI_C_SOURCE)
#    include <Uefi.h>
#    include <Uefi/UefiBaseType.h> //for EFI_STATUS definitions
#elif defined(_WIN32)
#    include <tchar.h>
DISABLE_WARNING_4255
#    include <windows.h>
RESTORE_WARNING_4255
#endif

#define ERROR_STRING_BUFFER_LENGTH SIZE_T_C(1024)

M_FUNC_ATTR_MALLOC char* M_NULLABLE get_strerror(errno_t error)
{
#if defined(HAVE_C11_ANNEX_K)
    size_t errorStringLen = strerrorlen_s(error);
    if (errorStringLen > 0 && errorStringLen < RSIZE_MAX)
    {
        char* errorString = M_REINTERPRET_CAST(char*, safe_calloc(errorStringLen + SIZE_T_C(1), sizeof(char)));
        if (errorString != M_NULLPTR)
        {
            errno_t truncated = strerror_s(errorString, errorStringLen + SIZE_T_C(1), error);
            if (truncated != 0)
            {
                perror("Error string was truncated");
            }
            return errorString;
        }
        else
        {
            return M_NULLPTR;
        }
    }
    else
    {
        return M_NULLPTR;
    }
#elif defined(HAVE_MSFT_SECURE_LIB)
    char* errorString = M_REINTERPRET_CAST(char*, safe_calloc(ERROR_STRING_BUFFER_LENGTH, sizeof(char)));
    if (errorString != M_NULLPTR)
    {
        if (0 == strerror_s(errorString, ERROR_STRING_BUFFER_LENGTH, error))
        {
            return errorString;
        }
        else
        {
            safe_free(&errorString);
            return M_NULLPTR;
        }
    }
    else
    {
        return M_NULLPTR;
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
    char* errorString = M_REINTERPRET_CAST(char*, safe_calloc(ERROR_STRING_BUFFER_LENGTH, sizeof(char)));
    if (errorString != M_NULLPTR)
    {
        if (0 == strerror_r(error, errorString, ERROR_STRING_BUFFER_LENGTH))
        {
            errorString[ERROR_STRING_BUFFER_LENGTH - SIZE_T_C(1)] =
                '\0'; // While it should be null terminated, there are
                      // known bugs on some systems where it is not!
            return errorString;
        }
        else
        {
            safe_free(&errorString);
            return M_NULLPTR;
        }
    }
    else
    {
        return M_NULLPTR;
    }
#    elif defined(HAVE_GNU_STRERR_R) || defined(_GNU_SOURCE)
    // GNU version
    char* errorStringBuf = M_REINTERPRET_CAST(char*, safe_calloc(ERROR_STRING_BUFFER_LENGTH, sizeof(char)));
    if (errorStringBuf != M_NULLPTR)
    {
        char*              errorString    = M_NULLPTR;
        eConstraintHandler currentHandler = set_Constraint_Handler(ERR_IGNORE);
        errno_t            duperr         = 0;
        char*              errmsg         = strerror_r(error, errorStringBuf, ERROR_STRING_BUFFER_LENGTH);
        if (errmsg != M_NULLPTR)
        {
            errorStringBuf[ERROR_STRING_BUFFER_LENGTH - SIZE_T_C(1)] =
                '\0'; // While it should be null terminated, there are
                      // known bugs on some systems where it is not!
            duperr = safe_strdup(&errorString, errmsg);
            set_Constraint_Handler(currentHandler);
            safe_free(&errorStringBuf);
            if (duperr == 0)
            {
                return errorString;
            }
            else
            {
                return M_NULLPTR;
            }
        }
        else
        {
            safe_free(&errorStringBuf);
            return M_NULLPTR;
        }
    }
    else
    {
        return M_NULLPTR;
    }
#    else
#        error "Error detection POSIX strerror_r vs GNU strerror_r"
#    endif
#else
    // use safe_strdup after disabling abort due to null to make this simpler code.
    char*              errorString    = M_NULLPTR;
    eConstraintHandler currentHandler = set_Constraint_Handler(ERR_IGNORE);
    errno_t            duperr         = safe_strdup(&errorString, strerror(error));
    set_Constraint_Handler(currentHandler);
    if (0 == duperr)
    {
        return errorString;
    }
    else
    {
        return M_NULLPTR;
    }
#endif
}

void print_Errno_To_Screen(errno_t error)
{
    char* errormsg = get_strerror(error);
    if (errormsg != M_NULLPTR)
    {
        printf("%d - %s\n", error, errormsg);
        safe_free(&errormsg);
    }
    else
    {
        printf("%d - <Unable to convert error to string>\n", error);
    }
}

#if defined(UEFI_C_SOURCE)

M_FUNC_ATTR_MALLOC char* M_NULLABLE get_efistatus_str(EFI_STATUS efiStatus)
{
    errno_t errorCode = 0;
    char * efiStatusString = M_NULLPTR;
    switch (efiStatus)
    {
    case EFI_SUCCESS:
        errorCode = safe_strdup(&efiStatusString, "Success");
        break;
    case EFI_LOAD_ERROR:
        errorCode = safe_strdup(&efiStatusString, "Load Error");
        break;
    case EFI_INVALID_PARAMETER:
        errorCode = safe_strdup(&efiStatusString, "Invalid Parameter");
        break;
    case EFI_UNSUPPORTED:
        errorCode = safe_strdup(&efiStatusString, "Unsupported");
        break;
    case EFI_BAD_BUFFER_SIZE:
        errorCode = safe_strdup(&efiStatusString, "Bad Buffer Size");
        break;
    case EFI_BUFFER_TOO_SMALL:
        errorCode = safe_strdup(&efiStatusString, "Buffer Too Small");
        break;
    case EFI_NOT_READY:
        errorCode = safe_strdup(&efiStatusString, "Not Ready");
        break;
    case EFI_DEVICE_ERROR:
        errorCode = safe_strdup(&efiStatusString, "Device Error");
        break;
    case EFI_WRITE_PROTECTED:
        errorCode = safe_strdup(&efiStatusString, "Write Protected");
        break;
    case EFI_OUT_OF_RESOURCES:
        errorCode = safe_strdup(&efiStatusString, "Out Of Resources");
        break;
    case EFI_VOLUME_CORRUPTED:
        errorCode = safe_strdup(&efiStatusString, "Volume Corrupted");
        break;
    case EFI_VOLUME_FULL:
        errorCode = safe_strdup(&efiStatusString, "Volume Full");
        break;
    case EFI_NO_MEDIA:
        errorCode = safe_strdup(&efiStatusString, "No Media");
        break;
    case EFI_MEDIA_CHANGED:
        errorCode = safe_strdup(&efiStatusString, "Media Changed");
        break;
    case EFI_NOT_FOUND:
        errorCode = safe_strdup(&efiStatusString, "Not Found");
        break;
    case EFI_ACCESS_DENIED:
        errorCode = safe_strdup(&efiStatusString, "Access Denied");
        break;
    case EFI_NO_RESPONSE:
        errorCode = safe_strdup(&efiStatusString, "No Response");
        break;
    case EFI_NO_MAPPING:
        errorCode = safe_strdup(&efiStatusString, "No Mapping");
        break;
    case EFI_TIMEOUT:
        errorCode = safe_strdup(&efiStatusString, "Timeout");
        break;
    case EFI_NOT_STARTED:
        errorCode = safe_strdup(&efiStatusString, "Not Started");
        break;
    case EFI_ALREADY_STARTED:
        errorCode = safe_strdup(&efiStatusString, "Already Started");
        break;
    case EFI_ABORTED:
        errorCode = safe_strdup(&efiStatusString, "Aborted");
        break;
    case EFI_ICMP_ERROR:
        errorCode = safe_strdup(&efiStatusString, "ICMP Error");
        break;
    case EFI_TFTP_ERROR:
        errorCode = safe_strdup(&efiStatusString, "TFTP Error");
        break;
    case EFI_PROTOCOL_ERROR:
        errorCode = safe_strdup(&efiStatusString, "Protocol Error");
        break;
    case EFI_INCOMPATIBLE_VERSION:
        errorCode = safe_strdup(&efiStatusString, "Incompatible Version");
        break;
    case EFI_SECURITY_VIOLATION:
        errorCode = safe_strdup(&efiStatusString, "Security Violation");
        break;
    case EFI_CRC_ERROR:
        errorCode = safe_strdup(&efiStatusString, "CRC Error");
        break;
    case EFI_END_OF_MEDIA:
        errorCode = safe_strdup(&efiStatusString, "End Of Media");
        break;
    case EFI_END_OF_FILE:
        errorCode = safe_strdup(&efiStatusString, "End Of File");
        break;
    case EFI_INVALID_LANGUAGE:
        errorCode = safe_strdup(&efiStatusString, "Invalid Language");
        break;
    case EFI_COMPROMISED_DATA:
        errorCode = safe_strdup(&efiStatusString, "Compromised Data");
        break;
    case EFI_HTTP_ERROR:
        errorCode = safe_strdup(&efiStatusString, "HTTP Error");
        break;
    case EFI_WARN_UNKNOWN_GLYPH:
        errorCode = safe_strdup(&efiStatusString, "Warning Unknown Glyph");
        break;
    case EFI_WARN_DELETE_FAILURE:
        errorCode = safe_strdup(&efiStatusString, "Warning Delete Failure");
        break;
    case EFI_WARN_WRITE_FAILURE:
        errorCode = safe_strdup(&efiStatusString, "Warning Write Failure");
        break;
    case EFI_WARN_BUFFER_TOO_SMALL:
        errorCode = safe_strdup(&efiStatusString, "Warning Buffer Too Small");
        break;
    case EFI_WARN_STALE_DATA:
        errorCode = safe_strdup(&efiStatusString, "Warning Stale Data");
        break;
    case EFI_WARN_FILE_SYSTEM:
        errorCode = safe_strdup(&efiStatusString, "Warning File System");
        break;
    default:
        errorCode = safe_strdup(&efiStatusString, "Unknown EFI Status value");
        break;
    }
    if (errorCode == 0)
    {
        return M_NULLPTR;
    }
    else
    {
        return efiStatusString;
    }
}

// Use %d to print this out or the output look really strange
#    define PRI_UINTN "d"
void print_EFI_STATUS_To_Screen(EFI_STATUS efiStatus)
{
    char*efiStatusString = get_efistatus_str(efiStatus);
    if (efiStatusString != M_NULLPTR)
    {
        printf("%" PRI_UINTN " - %s\n", efiStatus, efiStatusString);
        safe_free(&efiStatusString);
    }
    else
    {
        printf("%" PRI_UINTN " - <Unable to convert EFI_STATUS to string>\n", efiStatus);
    }
}

#elif defined(_WIN32)

M_FUNC_ATTR_MALLOC char* M_NULLABLE get_windows_error_str(winsyserror_t windowsError)
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
    if (windowsErrorString != M_NULLPTR)
    {
        size_t errorStrLen = lstrlen(windowsErrorString) + SIZE_T_C(1);
        char* errorString = M_REINTERPRET_CAST(char*, safe_calloc(errorStrLen, sizeof(char)));
        if (errorString != M_NULLPTR)
        {
            // This call is to allow the Windows API to translate this from wide to char as needed.
            // I could not find a straight forward ifdef/tchar function for this so this is the best I have - TJE
            int result = 0;
            #if defined (UNICODE)
            // Uppercase S means a Wide character string in this case
            result = sprintf_s(errorString, errorStrLen, "%S", windowsErrorString);
            #else
            // Lowercase s means a "regular" single-byte or multibyte string in this case.
            result = sprintf_s(errorString, errorStrLen, "%s", windowsErrorString);
            #endif
            LocalFree(windowsErrorString);
            if (result > 0)
            {
                return errorString;
            }
            else
            {
                // Conversion failed, free the allocated string and return null
                safe_free(&errorString);
                return M_NULLPTR;
            }
        }
        else
        {
            LocalFree(windowsErrorString);
            return M_NULLPTR;
        }
    }
    else
    {
        return M_NULLPTR;
    }
}

void print_Windows_Error_To_Screen(winsyserror_t windowsError)
{
    char *windowsErrorString = get_windows_error_str(windowsError);
    if (windowsErrorString != M_NULLPTR)
    {
        printf("%lu - %s\n", windowsError, windowsErrorString);
        safe_free(&windowsErrorString);
    }
    else
    {
        printf("%lu - <Unable to convert Windows error code to string>\n", windowsError);
    }
}
#endif
