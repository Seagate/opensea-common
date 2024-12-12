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
// \file constraint_handling.h
// \brief Defines how to call a constraint handler in C11 annex k -like
// functions
//

#pragma once

#include "code_attributes.h"
#include "common_types.h"

#if defined(__cplusplus)
extern "C"
{
#endif

    typedef enum eConstraintHandlerEnum
    {
        ERR_ABORT,
        ERR_DEFAULT = ERR_ABORT,
        ERR_WARN,  // emulated abort handler and outputs a warning message but does
                   // not abort
        ERR_IGNORE // does not output any information
    } eConstraintHandler;

#define CONSTRAINT_HANDLER_ENV_INFO_VERSION SIZE_T_C(1)
    typedef struct s_ConstraintEnvInfo
    {
        size_t      version; // CONSTRAINT_HANDLER_ENV_INFO_VERSION
        size_t      size;    // sizeof(constraintEnvInfo)
        const char* file;
        const char* function;
        const char* expression;
        int         line;
        // Other things to look into for the future: Compiler specific flags
        // GCC: __FUNCTION__, __PRETTY_FUNCTION__
        // MSVC: __FUNCDNAME__, __FUNCSIG__
    } constraintEnvInfo;

    static M_INLINE constraintEnvInfo* set_Env_Info(constraintEnvInfo* envInfo,
                                                    const char*        file,
                                                    const char*        function,
                                                    const char*        expression,
                                                    const int          line)
    {
        if (envInfo)
        {
            envInfo->version    = CONSTRAINT_HANDLER_ENV_INFO_VERSION;
            envInfo->size       = sizeof(constraintEnvInfo);
            envInfo->file       = file;
            envInfo->function   = function;
            envInfo->expression = expression;
            envInfo->line       = line;
        }
        return envInfo;
    }

    eConstraintHandler set_Constraint_Handler(eConstraintHandler handler);

    // calls the installed constraint handler. If one is not already set, it will
    // set the default error handler (abort) for you.
    void invoke_Constraint_Handler(const char* M_RESTRICT msg, void* M_RESTRICT ptr, errno_t error);

#if defined(__cplusplus)
}
#endif
