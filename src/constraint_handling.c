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

M_NORETURN void safe_abort_handler(const char* M_RESTRICT msg, M_ATTR_UNUSED void* M_RESTRICT ptr, errno_t error)
{
    fprintf(stderr, "abort_handler_s: %s\n", msg);
    fprintf(stderr, "Error code: %d\n", error);
    if (ptr != M_NULLPTR)
    {
        constraintErrorInfo *errorInfo = M_REINTERPRET_CAST(constraintErrorInfo*, ptr);
        fprintf(stderr, "Error location: \n");
        if (errorInfo->file)
        {
            fprintf(stderr, "\tFile: %s", errorInfo->file);
            fprintf(stderr, "\tLine: %u", errorInfo->line);
        }
        if (errorInfo->function)
        {
            fprintf(stderr, "\tFunction: %s", errorInfo->function);
        }
        if (errorInfo->expression)
        {
            fprintf(stderr, "\tExpression: %s", errorInfo->expression);
        }
        fprintf(stderr, "\n");
    }
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

void safe_warn_handler(const char* M_RESTRICT msg, M_ATTR_UNUSED void* M_RESTRICT ptr, errno_t error);

void safe_warn_handler(const char* M_RESTRICT msg, M_ATTR_UNUSED void* M_RESTRICT ptr, errno_t error)
{
    fprintf(stderr, "warn_handler: %s\n", msg);
    fprintf(stderr, "Error code: %d\n", error);
    if (ptr != M_NULLPTR)
    {
        constraintErrorInfo *errorInfo = M_REINTERPRET_CAST(constraintErrorInfo*, ptr);
        fprintf(stderr, "Error location: \n");
        if (errorInfo->file)
        {
            fprintf(stderr, "\tFile: %s", errorInfo->file);
            fprintf(stderr, "\tLine: %u", errorInfo->line);
        }
        if (errorInfo->function)
        {
            fprintf(stderr, "\tFunction: %s", errorInfo->function);
        }
        if (errorInfo->expression)
        {
            fprintf(stderr, "\tExpression: %s", errorInfo->expression);
        }
        fprintf(stderr, "\n");
    }
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
