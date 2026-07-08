/* SPDX-License-Identifier: BSD-2-Clause  */
/* Modifications Copyright 2022 Seagate Technology and/or its Affiliates */

/*	$OpenBSD: getopt.h,v 1.3 2013/11/22 21:32:49 millert Exp $	*/
/*	$NetBSD: getopt.h,v 1.4 2000/07/07 10:43:54 ad Exp $	*/

/*-
 * Copyright (c) 2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Dieter Baron and Thomas Klausner.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __GETOPT_H__
/**
 * DISCLAIMER
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is a part of the w64 mingw-runtime package.
 *
 * The w64 mingw-runtime package and its code is distributed in the hope that it
 * will be useful but WITHOUT ANY WARRANTY.  ALL WARRANTIES, EXPRESSED OR
 * IMPLIED ARE HEREBY DISCLAIMED.  This includes but is not limited to
 * warranties of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#define __GETOPT_H__ // NOLINT

#if defined(_WIN32)
/* All the headers include this file. */
#    include <crtdefs.h>
#endif

#include "code_attributes.h"

#ifdef __cplusplus
extern "C"
{
#endif

    extern int optind; /* index of first non-option in argv      */
    extern int optopt; /* single option character, as parsed     */
    extern int opterr; /* flag to enable built-in diagnostics... */
    /* (user may set to zero, to suppress)    */

    extern char* optarg; /* pointer to argument of current option  */

    M_NULL_TERM_STRING(3)
    M_PARAM_RO(2)
    M_PARAM_RO(3)
    M_NODISCARD int getopt(int nargc, char* const* M_NONNULL nargv, const char* M_NULLABLE options);

#ifdef _BSD_SOURCE
/*
 * BSD adds the non-standard `optreset' feature, for reinitialisation
 * of `getopt' parsing.  We support this feature, for applications which
 * proclaim their BSD heritage, before including this header; however,
 * to maintain portability, developers are advised to avoid it.
 */
#    if defined(__MINGW32__)
#        define optreset __mingw_optreset
#    endif //__MINGW32__
    extern int optreset;
#endif

#ifdef __cplusplus
}
#endif
/*
 * POSIX requires the `getopt' API to be specified in `unistd.h';
 * thus, `unistd.h' includes this header.  However, we do not want
 * to expose the `getopt_long' or `getopt_long_only' APIs, when
 * included in this manner.  Thus, close the standard __GETOPT_H__
 * declarations block, and open an additional __GETOPT_LONG_H__
 * specific block, only when *not* __UNISTD_H_SOURCED__, in which
 * to declare the extended API.
 */
#endif /* !defined(__GETOPT_H__) */

#if !defined(__UNISTD_H_SOURCED__) && !defined(__GETOPT_LONG_H__)
#define __GETOPT_LONG_H__ // NOLINT

#ifdef __cplusplus
extern "C"
{
#endif

    struct option /* specification for a long form option...	*/
    {
        const char* name;    /* option name, without leading hyphens */
        int         has_arg; /* does it take an argument?		*/
        int*        flag;    /* where to save its status, or NULL	*/
        int         val;     /* its associated status value		*/
    };

    enum /* permitted values for its `has_arg' field...	*/
    {
        no_argument = 0,   /* option never takes an argument	*/
        required_argument, /* option always requires an argument	*/
        optional_argument  /* option may take an argument		*/
    };

    M_NULL_TERM_STRING(3)
    M_PARAM_RW(2)
    M_PARAM_RO(3)
    M_PARAM_RO(4)
    M_PARAM_WO(5)
    M_NODISCARD int getopt_long(int                             nargc,
                                char* const* M_NONNULL          nargv,
                                const char* M_NULLABLE          options,
                                const struct option* M_NULLABLE long_options,
                                int* M_NULLABLE                 idx);

    M_NULL_TERM_STRING(3)
    M_PARAM_RW(2)
    M_PARAM_RO(3)
    M_PARAM_RO(4)
    M_PARAM_WO(5)
    M_NODISCARD int getopt_long_only(int                             nargc,
                                     char* const* M_NONNULL          nargv,
                                     const char* M_NULLABLE          options,
                                     const struct option* M_NULLABLE long_options,
                                     int* M_NULLABLE                 idx);
/*
 * Previous MinGW implementation had...
 */
#ifndef HAVE_DECL_GETOPT
/*
 * ...for the long form API only; keep this for compatibility.
 */
#    define HAVE_DECL_GETOPT 1
#endif

#ifdef __cplusplus
}
#endif

#endif /* !defined(__UNISTD_H_SOURCED__) && !defined(__GETOPT_LONG_H__) */
