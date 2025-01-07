// SPDX-License-Identifier: MPL-2.0

//! \file constraint_handling.c
//! \brief Defines constraint handling functions and information for bounds checking functions
//! such as safe_memcpy, safe_strcpy, etc. Emulates behavior of C11 Annex K
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2024 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "constraint_handling.h"
#include "code_attributes.h"
#include "common_types.h"
#include "io_utils.h"
#include "type_conversion.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void (*constraint_handler_func)(const char* M_RESTRICT msg, void* M_RESTRICT ptr, errno_t error);

static constraint_handler_func installedhandler = M_NULLPTR;

M_NORETURN void safe_abort_handler(const char* M_RESTRICT msg, void* M_RESTRICT ptr, errno_t error);

void safe_ignore_handler(const char* M_RESTRICT msg, void* M_RESTRICT ptr, errno_t error);

static M_INLINE void print_Additional_Error_Info(constraintEnvInfo* envInfo, FILE* stream)
{
    if (envInfo)
    {
        if (envInfo->version == CONSTRAINT_HANDLER_ENV_INFO_VERSION && envInfo->size == sizeof(constraintEnvInfo))
        {
            fprintf(stream, "\tAdditional Error info:\n");
            if (envInfo->file)
            {
                const char* fileNameOnly = strrchr(envInfo->file, '/');
                if (fileNameOnly == M_NULLPTR)
                {
                    fileNameOnly = strrchr(envInfo->file, '\\');
                    if (fileNameOnly == M_NULLPTR)
                    {
                        fileNameOnly = envInfo->file;
                    }
                    else
                    {
                        fileNameOnly += 1;
                    }
                }
                else
                {
                    fileNameOnly += 1;
                }
                fprintf(stream, "\t\tFile:       %s\n", fileNameOnly);
                fprintf(stream, "\t\tLine:       %d\n", envInfo->line);
            }
            if (envInfo->function)
            {
                fprintf(stream, "\t\tFunction:   %s\n", envInfo->function);
            }
            if (envInfo->expression)
            {
                fprintf(stream, "\t\tExpression: %s\n", envInfo->expression);
            }
        }
    }
}

M_NORETURN void safe_abort_handler(const char* M_RESTRICT msg, void* M_RESTRICT ptr, errno_t error)
{
    fprintf(stderr, "abort_handler_s: %s\n", msg);
    fprintf(stderr, "Error code: %d\n", error);
    print_Additional_Error_Info(M_REINTERPRET_CAST(constraintEnvInfo*, ptr), stderr);
    flush_stderr();
    abort();
}

void safe_ignore_handler(M_ATTR_UNUSED const char* M_RESTRICT msg,
                         M_ATTR_UNUSED void* M_RESTRICT       ptr,
                         M_ATTR_UNUSED errno_t                error)
{
    return; // NOLINT(readability-redundant-control-flow)
}

constraint_handler_func int_set_constraint_handler(constraint_handler_func handler);

constraint_handler_func int_set_constraint_handler(constraint_handler_func handler)
{
    constraint_handler_func old = installedhandler;
    if (old == M_NULLPTR)
    {
        old = safe_abort_handler;
    }
    if (handler == M_NULLPTR)
    {
        // install default which is abort handler for us
        installedhandler = safe_abort_handler;
    }
    else
    {
        installedhandler = handler;
    }
    return old;
}

void invoke_Constraint_Handler(const char* M_RESTRICT msg, void* M_RESTRICT ptr, errno_t error)
{
    constraint_handler_func handler = installedhandler;
    if (handler == M_NULLPTR)
    {
        installedhandler = safe_abort_handler;
        handler          = installedhandler;
    }
    handler(msg, ptr, error);
}

void safe_warn_handler(const char* M_RESTRICT msg, void* M_RESTRICT ptr, errno_t error);

void safe_warn_handler(const char* M_RESTRICT msg, void* M_RESTRICT ptr, errno_t error)
{
    fprintf(stderr, "warn_handler: %s\n", msg);
    fprintf(stderr, "Error code: %d\n", error);
    print_Additional_Error_Info(M_REINTERPRET_CAST(constraintEnvInfo*, ptr), stderr);
    flush_stderr();
}

eConstraintHandler set_Constraint_Handler(eConstraintHandler handler)
{
    eConstraintHandler      olde   = ERR_DEFAULT;
    constraint_handler_func oldptr = safe_abort_handler;
    switch (handler)
    {
    case ERR_ABORT:
        oldptr = int_set_constraint_handler(safe_abort_handler);
        break;
    case ERR_WARN:
        oldptr = int_set_constraint_handler(safe_warn_handler);
        break;
    case ERR_IGNORE:
        oldptr = int_set_constraint_handler(safe_ignore_handler);
        break;
    }

    if (oldptr == safe_abort_handler)
    {
        olde = ERR_ABORT;
    }
    else if (oldptr == safe_warn_handler)
    {
        olde = ERR_WARN;
    }
    else if (oldptr == safe_ignore_handler)
    {
        olde = ERR_IGNORE;
    }
    else
    {
        // we did not get a match, so assume default
        olde = ERR_DEFAULT;
    }
    return olde;
}
