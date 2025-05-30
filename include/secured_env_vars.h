// SPDX-License-Identifier: MPL-2.0

//! \file secured_env_vars.h
//! \brief Implements best practices for handling environment variables as
//! securely as possible per Cert-C coding standard
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2025 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "code_attributes.h"
#include "common_types.h"

#if defined(__cplusplus)
extern "C"
{
#endif

    //! \enum eEnvVarResult
    //! \brief result codes to indicate if environment was read successfully or not
    M_DECLARE_ENUM(eEnvVarResult,
                   ENV_VAR_SUCCESS,               /**!< Successfully read environment variable */
                   ENV_VAR_NOT_FOUND,             /**!< environment variable was not found */
                   ENV_VAR_TAMPERED_ENV_DETECTED, /**!< tampered environment variables detected. Variable not read */
                   ENV_VAR_FAILURE                /**!< Failure occurred when attempting to read variable. */
    );

    //! \fn eEnvVarResult get_Environment_Variable(const char* environmentVariableName, char** envVar)
    //! \brief Securely accesses and returns the requested environment variable
    //!
    //! Follows the Cert-C coding standards recommendations to read an environment variable.
    //! This function will allocate the \a envVar when it is read and this must be free'd by the called
    //! with free().
    //! \param[in] environmentVariableName name of the environment variable to access and read the value of
    //! \param[out] envVar value of the environment variable if accessed without error
    //! \return
    //! - ENV_VAR_SUCCESS read successfully.
    //!
    //! - ENV_VAR_NOT_FOUND \a environmentVariableName not found.
    //!
    //! - ENV_VAR_TAMPERED_ENV_DETECTED tampered environment detected; variable not read.
    //!
    //! - ENV_VAR_FAILURE a failure occurred when trying to read the environment variable such as no more memory.
    M_NODISCARD M_NONNULL_PARAM_LIST(1, 2) M_PARAM_RO(1) M_NULL_TERM_STRING(1) M_PARAM_WO(2) eEnvVarResult
        get_Environment_Variable(const char* environmentVariableName, char** envVar);

#if defined(__cplusplus)
}
#endif
