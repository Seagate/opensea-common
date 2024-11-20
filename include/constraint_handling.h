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

    eConstraintHandler set_Constraint_Handler(eConstraintHandler handler);

    // calls the installed constraint handler. If one is not already set, it will
    // set the default error handler (abort) for you.
    void invoke_Constraint_Handler(const char* M_RESTRICT msg, void* M_RESTRICT ptr, errno_t error);

#if defined(__cplusplus)
}
#endif
