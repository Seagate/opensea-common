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
#include "type_conversion.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined(HAVE_C11_ANNEX_K)
// If C11 annex K's constraint handler functions are not available, declare them
// here.
typedef void (*constraint_handler_t)(const char* M_RESTRICT msg, void* M_RESTRICT ptr, errno_t error);

static constraint_handler_t* installedhandler = M_NULLPTR;

constraint_handler_t set_constraint_handler_s(constraint_handler_t handler);

M_NORETURN void abort_handler_s(const char* M_RESTRICT msg, void* M_RESTRICT ptr, errno_t error);

void ignore_handler_s(const char* M_RESTRICT msg, void* M_RESTRICT ptr, errno_t error);

M_NORETURN void abort_handler_s(const char* M_RESTRICT msg, M_ATTR_UNUSED void* M_RESTRICT ptr, errno_t error)
{
    fprintf(stderr, "abort_handler_s: %s\n", msg);
    fprintf(stderr, "Error code: %d\n", error);
    abort();
}

void ignore_handler_s(M_ATTR_UNUSED const char* M_RESTRICT msg,
                      M_ATTR_UNUSED void* M_RESTRICT       ptr,
                      M_ATTR_UNUSED errno_t                error)
{
    return;
}

constraint_handler_t set_constraint_handler_s(constraint_handler_t handler)
{
    constraint_handler_t* old = installedhandler;
    if (old == M_NULLPTR)
    {
        *old = abort_handler_s;
    }
    if (handler == M_NULLPTR)
    {
        // install default which is abort handler for us
        *installedhandler = abort_handler_s;
    }
    else
    {
        *installedhandler = handler;
    }
    return *old;
}

#else
static constraint_handler_t* installedhandler = M_NULLPTR;

#endif // HAVE_C11_ANNEX_K

void invoke_Constraint_Handler(const char* M_RESTRICT msg, void* M_RESTRICT ptr, errno_t error)
{
    constraint_handler_t handler = *installedhandler;
#if defined(HAVE_C11_ANNEX_K)
    // set the default handler with a NULL pointer
    handler = set_constraint_handler_s(M_NULLPTR); // get the current one
    if (handler == M_NULLPTR)
    {
        // this is not supposed to happen, but making sure we always have the
        // abort handler set as default
        handler = abort_handler_s;
    }
    set_constraint_handler_s(handler); // set it back, we just needed to figure it out to store our
                                       // copy for our own functions -TJE
    *installedhandler = handler;
#else
    if (handler == M_NULLPTR)
    {
        *installedhandler = abort_handler_s;
        handler           = *installedhandler;
    }
#endif
    handler(msg, ptr, error);
}

void warn_handler_s(const char* M_RESTRICT msg, M_ATTR_UNUSED void* M_RESTRICT ptr, errno_t error);

void warn_handler_s(const char* M_RESTRICT msg, M_ATTR_UNUSED void* M_RESTRICT ptr, errno_t error)
{
    fprintf(stderr, "warn_handler_s: %s\n", msg);
    fprintf(stderr, "Error code: %d\n", error);
}

eConstraintHandler set_Constraint_Handler(eConstraintHandler handler)
{
    eConstraintHandler    olde   = ERR_DEFAULT;
    constraint_handler_t* oldptr = M_NULLPTR;
    switch (handler)
    {
    case ERR_ABORT:
        *oldptr = set_constraint_handler_s(abort_handler_s);
        break;
    case ERR_WARN:
        *oldptr = set_constraint_handler_s(warn_handler_s);
        break;
    case ERR_IGNORE:
        *oldptr = set_constraint_handler_s(ignore_handler_s);
        break;
    }

    if (oldptr != M_NULLPTR && *oldptr != M_NULLPTR)
    {
        if (*oldptr == abort_handler_s)
        {
            olde = ERR_ABORT;
        }
        else if (*oldptr == warn_handler_s)
        {
            olde = ERR_WARN;
        }
        else if (*oldptr == ignore_handler_s)
        {
            olde = ERR_IGNORE;
        }
        else
        {
            // we did not get a match, so assume default
            olde = ERR_DEFAULT;
        }
    }
    else
    {
        // something is not right. We should never have this returned as a null
        // pointer. set the default as the output
        olde = ERR_DEFAULT;
    }
    return olde;
}
