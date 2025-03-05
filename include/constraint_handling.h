// SPDX-License-Identifier: MPL-2.0

//! \file constraint_handling.h
//! \brief Defines constraint handling functions and information for bounds checking functions
//! such as safe_memcpy, safe_strcpy, etc. Emulates behavior of C11 Annex K
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2024 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
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

    //! \enum eConstraintHandler
    //! \brief Defines constraint handler types.
    //!
    //! This enum defines different constraint handlers that can be set with
    //! the set_Constraint_Handler() function.
    //! \sa set_Constraint_Handler
    typedef enum eConstraintHandlerEnum
    {
        ERR_ABORT,               /*!< Outputs error information to stderr then calls abort() */
        ERR_DEFAULT = ERR_ABORT, /*!< Default constraint handler. Same as the abort handler*/
        ERR_WARN,                /*!< Outputs error information to stderr and allows execution to continue */
        ERR_IGNORE               /*!< Does not output any information or abort. Allows execution to continue */
    } eConstraintHandler;

    //! \def CONSTRAINT_HANDLER_ENV_INFO_VERSION
    //! \brief Defines a version number to be used in the constraintEnvInfo struct
#define CONSTRAINT_HANDLER_ENV_INFO_VERSION SIZE_T_C(1)

    //! \struct constraintEnvInfo
    //! \brief Structure to pass with additional error info to a constraint handler.
    //!
    //! This structure takes additional information for __FILE__, __func__, __LINE__,
    //! and an expression to pass to the constraint handlers to output additional
    //! error information to help track back to where an error occurred.
    //! \todo Other things to look into for the future: Compiler specific flags
    //! GCC: __FUNCTION__, __PRETTY_FUNCTION__
    //! MSVC: __FUNCDNAME__, __FUNCSIG__
    //! \sa CONSTRAINT_HANDLER_ENV_INFO_VERSION
    //! \sa set_Env_Info
    //! \sa invoke_Constraint_Handler
    typedef struct s_ConstraintEnvInfo
    {
        size_t      version;    //!< set to CONSTRAINT_HANDLER_ENV_INFO_VERSION
        size_t      size;       //!< set to sizeof(constraintEnvInfo)
        const char* file;       //!< set to __FILE__
        const char* function;   //!< set to __func__
        const char* expression; //!< set to expression that caused the violation. Example: safe_memset(params)
        int         line;       //!< set to __LINE__
    } constraintEnvInfo;

    //! \fn constraintEnvInfo* set_Env_Info(constraintEnvInfo* envInfo, const char* file, const char* function, const
    //! char* expression, const int line)
    //! \brief Inline helper function to quickly and easily fill in extra debug
    //! information
    //!
    //! This inline helper function is meant to help functions using a constraint handler easily
    //! create the additional error information structure with all the provided information quickly and easily.
    //! \param[in] envInfo pointer to the constraintEnvInfo to fill in data
    //! \param[in] file string describing the file the error came from
    //! \param[in] function string describing the function the error came from
    //! \param[in] expression string describing the expression the error came from in a source file. Ex:
    //! "safe_memset(pointer, destsz, 0, count)"
    //! \param[in] line integer representing the line number the error came
    //! from
    //! \return returns same pointer as \a envInfo to allow it to be quickly passed to a constraint handler's ptr
    //! parameter
    M_NONNULL_PARAM_LIST(1)
    M_PARAM_WO(1)
    static M_INLINE constraintEnvInfo* set_Env_Info(constraintEnvInfo* envInfo,
                                                    const char*        file,
                                                    const char*        function,
                                                    const char*        expression,
                                                    const int          line)
    {
        DISABLE_NONNULL_COMPARE
        if (envInfo != M_NULLPTR)
        {
            envInfo->version    = CONSTRAINT_HANDLER_ENV_INFO_VERSION;
            envInfo->size       = sizeof(constraintEnvInfo);
            envInfo->file       = file;
            envInfo->function   = function;
            envInfo->expression = expression;
            envInfo->line       = line;
        }
        RESTORE_NONNULL_COMPARE
        return envInfo;
    }

    //! \fn eConstraintHandler set_Constraint_Handler(eConstraintHandler handler)
    //! \brief Function to set the constraint handler to use during execution.
    //!
    //! Use this function to install a different contraint handler or restore a previous one.
    //! It will return the currently installed handler and install the new one requested.
    //! Save the output to allow restoring to the previous handler as needed.
    //! \param[in] handler enum value from eConstraintHandler to install
    //! \return returns enum value from eConstraintHandler for the previously installed handler.
    eConstraintHandler set_Constraint_Handler(eConstraintHandler handler);

    //! \fn void invoke_Constraint_Handler(const char* M_RESTRICT msg, void* M_RESTRICT ptr, errno_t error)
    //! \brief Used to call the currently installed constraint handler
    //!
    //! This is used in bounds-checking functions to call the currently installed constraint handler
    //! when an error is detected during execution. If no constraint handler is installed, this
    //! will automatically install the abort handler.
    //! \param[in] msg the message to print by the constraint handler when an error is encountered.
    //! \param[in] ptr additional error information to output in the constraint handler. Expects constraintEnvInfo*
    //! \param[in] error errno value describing the error that occurred during execution.
    M_NONNULL_PARAM_LIST(1)
    M_PARAM_RO(1)
    M_NULL_TERM_STRING(1)
    void invoke_Constraint_Handler(const char* M_RESTRICT msg, void* M_RESTRICT ptr, errno_t error);

#if defined(__cplusplus)
}
#endif
