// SPDX-License-Identifier: MPL-2.0
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2024 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
// 
// \file secured_env_vars.h
// \brief Implements best practices for handling environment variables as securely as possible

#pragma once

#include "code_attributes.h"
#include "common_types.h"

#if defined (__cplusplus)
extern "C"
{
#endif

    M_DECLARE_ENUM(eEnvVarResult,
        ENV_VAR_SUCCESS,
        ENV_VAR_NOT_FOUND,
        ENV_VAR_TAMPERED_ENV_DETECTED,
        ENV_VAR_FAILURE
        );

    //this function will return allocated memory for an environment variable that is requested
    //the returned memory must be freed by the caller with free() or safe_Free()
    //will return M_NULLPTR if not found
    //will return M_NULLPTR if the environment variable list is tampered and not secure to read and trust
    M_NODISCARD eEnvVarResult get_Environment_Variable(const char* environmentVariableName, char** envVar);

#if defined (__cplusplus)
}
#endif
