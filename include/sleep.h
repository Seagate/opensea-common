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
// \file sleep.h
// \brief Defines functions to sleep/delay in cross-platform mannor
//

#pragma once

#include "common_types.h"

#if defined(__cplusplus)
extern "C"
{
#endif

    //-----------------------------------------------------------------------------
    //
    //  delay_Milliseconds()
    //
    //! \brief   Description:  delay for a specified number of milliseconds
    //
    //  Entry:
    //!   \param[in] milliseconds = number of milliseconds to delay for
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void delay_Milliseconds(uint32_t milliseconds);

    //-----------------------------------------------------------------------------
    //
    //  delay_Seconds()
    //
    //! \brief   Description:  delay for a specified number of seconds
    //
    //  Entry:
    //!   \param[in] seconds = number of seconds to delay for
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void delay_Seconds(uint32_t seconds);

#if defined(__cplusplus)
}
#endif
