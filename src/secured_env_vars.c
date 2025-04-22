// SPDX-License-Identifier: MPL-2.0

//! \file secured_env_vars.c
//! \brief Implements best practices for handling environment variables as
//! securely as possible per Cert-C coding standard
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2025 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "secured_env_vars.h"
#include "constraint_handling.h"
#include "env_detect.h"
#include "memory_safety.h"
#include "string_utils.h"
#include "type_conversion.h"

#include <string.h>

#if defined(_WIN32)
#    include <tchar.h>
// If multiple environment variables with the same name exist, consider this a
// tampered environment and insecure
static bool is_Environment_Variable_List_Tampered(void)
{
    static bool tampered     = false;
    static bool envEvaluated = false;

    if (envEvaluated == false)
    {
        LPTSTR envBlock = GetEnvironmentStrings();
        if (envBlock == M_NULLPTR)
        {
            return false;
        }

        size_t k     = SIZE_T_C(0);
        size_t l     = SIZE_T_C(0);
        size_t len_i = SIZE_T_C(0);
        size_t len_j = SIZE_T_C(0);

        for (LPTSTR envVar_i = envBlock; *envVar_i; envVar_i += _tcslen(envVar_i) + 1)
        {
            for (LPTSTR envVar_j = envVar_i; *envVar_j; envVar_j += _tcslen(envVar_j) + 1)
            {
                if (envVar_i != envVar_j)
                {
                    k = 0;
                    l = 0;
                    // originally used lstrlen, but switched to _tcslen to fix
                    // int to size_t warning
                    len_i = _tcslen(envVar_i);
                    len_j = _tcslen(envVar_j);

                    while (k < len_i && l < len_j)
                    {
                        if (envVar_i[k] != envVar_j[l])
                        {
                            break;
                        }

                        if (envVar_i[k] == '=')
                        {
                            envEvaluated = true;
                            tampered     = true;
                            FreeEnvironmentStrings(envBlock);
                            return tampered;
                        }

                        k++;
                        l++;
                    }
                }
            }
        }
        FreeEnvironmentStrings(envBlock);
        envBlock = M_NULLPTR;
    }
    return tampered;
}
#elif defined(POSIX_1993)

/*POSIX standard requires us to define this before using it*/
extern char** environ;

static bool is_Environment_Variable_List_Tampered(void)
{
    static bool tampered     = false;
    static bool envEvaluated = false;
    if (envEvaluated == false)
    {
        size_t k     = SIZE_T_C(0);
        size_t l     = SIZE_T_C(0);
        size_t len_i = SIZE_T_C(0);
        size_t len_j = SIZE_T_C(0);

        for (size_t i = SIZE_T_C(0); environ[i] != M_NULLPTR; i++)
        {
            for (size_t j = i; environ[j] != M_NULLPTR; j++)
            {
                if (i != j)
                {
                    k = 0;
                    l = 0;

                    len_i = safe_strlen(environ[i]);
                    len_j = safe_strlen(environ[j]);

                    while (k < len_i && l < len_j)
                    {
                        if (environ[i][k] != environ[j][l])
                        {
                            break;
                        }

                        if (environ[i][k] == '=')
                        {
                            envEvaluated = true;
                            tampered     = true;
                            return tampered;
                        }

                        k++;
                        l++;
                    }
                }
            }
        }
    }
    return tampered;
}
#else  // no way to detect tampered environment variables
// UEFI EDK2-libc seems to use shell API calls to look for environment
// variables. There may be one in there to read all of them to validate them
// ShellGetEnvironmentVariable()
// https://github.com/tianocore/edk2/blob/95a6892aacfef6c7862058fcdc495f6b35df9367/ShellPkg/Include/Library/ShellLib.h#L506
//  https://github.com/tianocore/edk2/blob/95a6892aacfef6c7862058fcdc495f6b35df9367/MdePkg/Include/Protocol/Shell.h#L514
//  or
//  https://github.com/tianocore/edk2/blob/95a6892aacfef6c7862058fcdc495f6b35df9367/MdePkg/Include/Protocol/Shell.h#L544
static bool is_Environment_Variable_List_Tampered(void)
{
    return false;
}
#endif // platform check for verifying environment is secure before reading
       // variables

#if !defined(DISABLE_SECURE_GETENV) && !defined(HAVE_SECURE_GETENV)
#    if defined(_GNU_SOURCE) && IS_GLIBC_VERSION(2, 17)
#        define HAVE_SECURE_GETENV
#    endif // lots of checks for secure_getenv function
#endif     //! DISABLE_SECURE_GETENV

// this function will return allocated memory for an environment variable that
// is requested the returned memory must be freed by the caller will return
// M_NULLPTR if not found
M_NODISCARD eEnvVarResult get_Environment_Variable(const char* environmentVariableName, char** envVar)
{
    DISABLE_NONNULL_COMPARE
    if (envVar == M_NULLPTR || environmentVariableName == M_NULLPTR)
    {
        return ENV_VAR_FAILURE;
    }
    RESTORE_NONNULL_COMPARE
    *envVar = M_NULLPTR;
    if (is_Environment_Variable_List_Tampered() == false)
    {
        errno_t error = 0;
#if defined(HAVE_GETENV_S) || defined(HAVE_MSFT_SECURE_LIB) ||                                                         \
    (defined(__STDC_LIB_EXT1__) && defined(__STDC_WANT_LIB_EXT1__))
        /* MSFT/C11 annex K adds getenv_s, so use it when available to check if
         * this exists */
        size_t size = SIZE_T_C(0);
        if (getenv_s(&size, M_NULLPTR, 0, environmentVariableName) == 0 && size > SIZE_T_C(0) && size < SIZE_MAX)
        {
            size += SIZE_T_C(1); // make sure there is room for M_NULLPTR
            *envVar = M_REINTERPRET_CAST(char*, safe_calloc(size, sizeof(char)));
            if (*envVar != M_NULLPTR)
            {
                size_t test = SIZE_T_C(0); // this is needed to make Windows happy.-TJE
                if (getenv_s(&test, *envVar, size, environmentVariableName) != 0)
                {
                    // error, so free this before moving on
                    safe_free(envVar);
                    if (errno != 0)
                    {
                        error = errno;
                    }
                    else
                    {
                        error = ENOMEM;
                    }
                }
            }
            else
            {
                if (errno != 0)
                {
                    error = errno;
                }
                else
                {
                    error = ENOMEM;
                }
            }
        }
#elif defined(HAVE_SECURE_GETENV) && !defined(DISABLE_SECURE_GETENV)
        /*
         * Use secure_getenv, unless the DISABLE_SECURE_GETENV is defined
         * secure_getenv (when available) is used by default unless
         * DISABLE_SECURE_GETENV is defined by the person building this library.
         * See https://linux.die.net/man/3/secure_getenv for reasons to disable
         * it.
         */
        // Turn off constraint handling since it is possible we will receive NULL and want to continue execution -TJE
        eConstraintHandler currentHandler = set_Constraint_Handler(ERR_IGNORE);
        error                             = safe_strdup(envVar, secure_getenv(environmentVariableName));
        // Restore previous constraint handler
        set_Constraint_Handler(currentHandler);
#elif defined(HAVE___SECURE_GETENV) && !defined(DISABLE_SECURE_GETENV)
        /*
         * Use __secure_getenv, unless the DISABLE_SECURE_GETENV is defined
         * __secure_getenv (when available) is used by default unless
         * DISABLE_SECURE_GETENV is defined by the person building this library.
         * See https://linux.die.net/man/3/secure_getenv for reasons to disable
         * it.
         */
        // Turn off constraint handling since it is possible we will receive NULL and want to continue execution -TJE
        eConstraintHandler currentHandler = set_Constraint_Handler(ERR_IGNORE);
        error                             = safe_strdup(envVar, __secure_getenv(environmentVariableName));
        // Restore previous constraint handler
        set_Constraint_Handler(currentHandler);
#else
        // Turn off constraint handling since it is possible we will receive NULL and want to continue execution -TJE
        eConstraintHandler currentHandler = set_Constraint_Handler(ERR_IGNORE);
        error                             = safe_strdup(envVar, getenv(environmentVariableName));
        // Restore previous constraint handler
        set_Constraint_Handler(currentHandler);
#endif // checking macros for which getenv function to use
        if (error == 0 && *envVar != M_NULLPTR)
        {
            return ENV_VAR_SUCCESS;
        }
        else
        {
            return ENV_VAR_NOT_FOUND;
        }
    }
    else
    {
        return ENV_VAR_TAMPERED_ENV_DETECTED;
    }
}
