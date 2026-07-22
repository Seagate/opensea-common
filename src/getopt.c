/* SPDX-License-Identifier: 0BSD AND BSD-2-Clause */
/* Modifications Copyright 2022 Seagate Technology and/or its Affiliates */

/*	$OpenBSD: getopt_long.c,v 1.32 2020/05/27 22:25:09 schwarze Exp $	*/
/*	$NetBSD: getopt_long.c,v 1.15 2002/01/31 22:43:40 tv Exp $	*/

/*
 * Copyright (c) 2002 Todd C. Miller <millert@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Sponsored in part by the Defense Advanced Research Projects
 * Agency (DARPA) and Air Force Research Laboratory, Air Force
 * Materiel Command, USAF, under agreement number F39502-99-1-0512.
 */
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

#include "getopt.h"
#include "predef_env_detect.h"
#include "warning_utils.h"

#if defined(HAVE_STD_INT) || (defined __STDC__ && defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L)
#    include <stdint.h>
#elif !defined(_UINTPTR_T_DEFINED) && !defined(UINTPTR_MAX)
/* uintptr_t does not exist so need to define what type it is as best we can */
/* Checking known data model macros first and Windows predefined macros */
#    if defined(__LP64__) || defined(_LP64) || defined(__ILP64__) || defined(_ILP64)
typedef unsigned long uintptr_t;
#        define UINTPTR_MAX FFFFFFFFFFFFFFFFUL;
#    elif defined(_WIN32)
#        if defined(_WIN64)
typedef unsigned long long uintptr_t;
#            define UINTPTR_MAX FFFFFFFFFFFFFFFFULL;
#        else
typedef unsigned long uintptr_t;
#            define UINTPTR_MAX FFFFFFFFUL;
#        endif
#    elif defined(__ILP32__) || defined(_ILP32) || defined(__LP32__) || defined(_LP32)
typedef unsigned long uintptr_t;
#        define UINTPTR_MAX FFFFFFFFUL;
#    else /* done checking macros for various known data models. Fall back to CPU macros */
#        if defined(_WIN64) || defined(_M_IA64) || defined(_M_ALPHA) || defined(_M_X64) || defined(_M_AMD64) ||        \
            defined(__alpha__) || defined(__amd64__) || defined(__x86_64__) || defined(__aarch64__) ||                 \
            defined(__ia64__) || defined(__IA64__) || defined(__powerpc64__) || defined(__PPC64__) ||                  \
            defined(__ppc64__) || defined(_ARCH_PPC64) // 64bit
/* 64bit and Windows already handled, so assuming unsigned long*/
typedef unsigned long uintptr_t;
#            define UINTPTR_MAX FFFFFFFFFFFFFFFFUL
#        else
/* 32bit, use unsigned long*/
typedef unsigned long uintptr_t;
#            define UINTPTR_MAX FFFFFFFFUL;
#        endif
#    endif
#endif // checking for stdint
#if defined(_WIN32)
DISABLE_WARNING_4255
#    include <windows.h>
RESTORE_WARNING_4255
#else
#    include <libgen.h> /*for basename*/
#endif                  /*_WIN32*/

#define REPLACE_GETOPT /* use this getopt as the system getopt(3) */

#ifdef REPLACE_GETOPT
int opterr = 1;   /* if error message should be printed */
int optind = 1;   /* index into parent argv vector */
int optopt = '?'; /* character checked for validity */
#    if defined(__MINGW32__)
#        undef optreset /* see getopt.h */
#        define optreset __mingw_optreset
#    endif      /*__MINGW32__*/
int   optreset; /* reset getopt */
char* optarg;   /* argument associated with option */
#endif          /*REPLACE_GETOPT*/

#define PRINT_ERROR   ((opterr) && (*options != ':'))

#define FLAG_PERMUTE  0x01 /* permute non-options to the end of argv */
#define FLAG_ALLARGS  0x02 /* treat non-options as args to option "-1" */
#define FLAG_LONGONLY 0x04 /* operate as getopt_long_only */

/* return values */
#define BADCH   M_STATIC_CAST(int, '?')
#define BADARG  ((*options == ':') ? M_STATIC_CAST(int, ':') : M_STATIC_CAST(int, '?'))
#define INORDER M_STATIC_CAST(int, 1)

#ifdef __CYGWIN__
static char EMSG[] = "";
#else
#    define EMSG ""
#endif

static char* place = M_CONST_CAST(char*, EMSG); /* option letter processing */

/* XXX: set optreset to 1 rather than these two */
static int nonopt_start = -1; /* first non option argument (for permute) */
static int nonopt_end   = -1; /* first option after non options (for permute) */

/*
 * Compute the greatest common divisor of a and b.
 */
M_NODISCARD M_CONST_FUNC static int gcd(int a, int b)
{
    int c;

    c = a % b;
    while (c != 0)
    {
        a = b;
        b = c;
        c = a % b;
    }

    return (b);
}

M_NODISCARD M_PURE_FUNC M_NULL_TERM_STRING(1) M_PARAM_RO(1) static size_t getopt_strlen(const char* M_NULLABLE str)
{
    if (str)
    {
#if defined(RSIZE_MAX)
        const char* found = memchr(str, '\0', RSIZE_MAX);
#elif defined(SIZE_MAX)
        // Note dividing SIZE_MAX by 2 to prevent a warning about
        // stringop maximum object size.
        const char* found = memchr(str, '\0', SIZE_MAX >> 1);
#else
        // Note dividing SIZE_MAX by 2 to prevent a warning about
        // stringop maximum object size.
        const char* found = memchr(str, '\0', M_STATIC_CAST(size_t, -1) >> 1);
#endif
        if (found != M_NULLPTR)
        {
            return M_STATIC_CAST(size_t, M_REINTERPRET_CAST(uintptr_t, found) - M_REINTERPRET_CAST(uintptr_t, str));
        }
        else
        {
#if defined(RSIZE_MAX)
            return RSIZE_MAX;
#elif defined(SIZE_MAX)
            return SIZE_MAX >> 1;
#else
            return M_STATIC_CAST(size_t, -1) >> 1;
#endif
        }
    }
    else
    {
        return 0;
    }
}

/*
 * Exchange the block from nonopt_start to nonopt_end with the block
 * from nonopt_end to opt_end (keeping the same order of arguments
 * in each block).
 */
M_NONNULL_PARAM_LIST(4)
M_PARAM_RW(4) static void permute_args(int panonopt_start, int panonopt_end, int opt_end, char* const* M_NONNULL nargv)
{
    int   cstart, cyclelen, i, j, ncycle, nnonopts, nopts, pos;
    char* swap;

    /*
     * compute lengths of blocks and number and size of cycles
     */
    nnonopts = panonopt_end - panonopt_start;
    nopts    = opt_end - panonopt_end;
    ncycle   = gcd(nnonopts, nopts);
    cyclelen = (opt_end - panonopt_start) / ncycle;

    for (i = 0; i < ncycle; i++)
    {
        cstart = panonopt_end + i;
        pos    = cstart;
        for (j = 0; j < cyclelen; j++)
        {
            if (pos >= panonopt_end)
                pos -= nnonopts;
            else
                pos += nopts;
            swap = nargv[pos];
            /* LINTED const cast */
            M_CONST_CAST(char**, nargv)[pos] = nargv[cstart];
            /* LINTED const cast */
            M_CONST_CAST(char**, nargv)[cstart] = swap;
        }
    }
}

/*
 * parse_long_options --
 *	Parse long options in argc/argv argument vector.
 * Returns -1 if short_too is set and the option does not match long_options.
 */
M_NODISCARD M_NONNULL_PARAM_LIST(1, 3) M_PARAM_RO(1) M_PARAM_RO(3)
    M_PARAM_WO(4) static int parse_long_options(char* const* M_NONNULL               nargv,
                                                M_ATTR_UNUSED const char* M_NULLABLE options,
                                                const struct option* M_NONNULL       long_options,
                                                int* M_NULLABLE                      idx,
                                                int                                  short_too,
                                                int                                  flags)
{
    char * current_argv, *has_equal;
    size_t current_argv_len;
    int    i, match, exact_match, second_partial_match;

    current_argv         = place;
    match                = -1;
    exact_match          = 0;
    second_partial_match = 0;

    optind++;

    if ((has_equal = strchr(current_argv, '=')) != M_NULLPTR)
    {
        /* argument found (--option=arg) */
        current_argv_len = M_STATIC_CAST(size_t, M_REINTERPRET_CAST(uintptr_t, has_equal) -
                                                     M_REINTERPRET_CAST(uintptr_t, current_argv));
        has_equal++;
    }
    else
        current_argv_len = getopt_strlen(current_argv);

    for (i = 0; long_options[i].name; i++)
    {
        /* find matching long option */
        if (strncmp(current_argv, long_options[i].name, current_argv_len) != 0)
            continue;

        if (getopt_strlen(long_options[i].name) == current_argv_len)
        {
            /* exact match */
            match       = i;
            exact_match = 1;
            break;
        }
        /*
         * If this is a known short option, don't allow
         * a partial match of a single character.
         */
        if (short_too && current_argv_len == 1)
            continue;

        if (match == -1) /* first partial match */
            match = i;
        else if ((flags & FLAG_LONGONLY) || long_options[i].has_arg != long_options[match].has_arg ||
                 long_options[i].flag != long_options[match].flag || long_options[i].val != long_options[match].val)
            second_partial_match = 1;
    }
    if (!exact_match && second_partial_match)
    {
        /* ambiguous abbreviation */
        if (PRINT_ERROR)
            cmdline_warnx(CMDLINE_ERR_MSG_AMBIG, M_STATIC_CAST(int, current_argv_len), current_argv);
        optopt = 0;
        return (BADCH);
    }
    if (match != -1)
    { /* option found */
        if (long_options[match].has_arg == no_argument && has_equal)
        {
            if (PRINT_ERROR)
                cmdline_warnx(CMDLINE_ERR_MSG_NOARG, M_STATIC_CAST(int, current_argv_len), current_argv);
            /*
             * XXX: GNU sets optopt to val regardless of flag
             */
            if (long_options[match].flag == M_NULLPTR)
                optopt = long_options[match].val;
            else
                optopt = 0;
            return (BADARG);
        }
        if (long_options[match].has_arg == required_argument || long_options[match].has_arg == optional_argument)
        {
            if (has_equal)
                optarg = has_equal;
            else if (long_options[match].has_arg == required_argument)
            {
                /*
                 * optional argument doesn't use next nargv
                 */
                optarg = nargv[optind++];
            }
        }
        if ((long_options[match].has_arg == required_argument) && (optarg == M_NULLPTR))
        {
            /*
             * Missing argument; leading ':' indicates no error
             * should be generated.
             */
            if (PRINT_ERROR)
                cmdline_warnx(CMDLINE_ERR_MSG_RECARGSTRING, current_argv);
            /*
             * XXX: GNU sets optopt to val regardless of flag
             */
            if (long_options[match].flag == M_NULLPTR)
                optopt = long_options[match].val;
            else
                optopt = 0;
            --optind;
            return (BADARG);
        }
    }
    else
    { /* unknown option */
        if (short_too)
        {
            --optind;
            return (-1);
        }
        if (PRINT_ERROR)
            cmdline_warnx(CMDLINE_ERR_MSG_ILLOPTSTRING, current_argv);
        optopt = 0;
        return (BADCH);
    }
    if (idx)
        *idx = match;
    if (long_options[match].flag)
    {
        *long_options[match].flag = long_options[match].val;
        return (0);
    }
    else
        return (long_options[match].val);
}

static const char* posixlycorrectenv = "POSIXLY_CORRECT";

/*
 * getopt_internal --
 *	Parse argc/argv argument vector.  Called by user level routines.
 */
M_NODISCARD M_NONNULL_PARAM_LIST(2) M_PARAM_RW(2) M_NULL_TERM_STRING(3) M_PARAM_RO(3) M_PARAM_RO(4)
    M_PARAM_WO(5) static int getopt_internal(int                             nargc,
                                             char* const* M_NONNULL          nargv,
                                             const char* M_NULLABLE          options,
                                             const struct option* M_NULLABLE long_options,
                                             int* M_NULLABLE                 idx,
                                             int                             flags)
{
    const char* oli; /* option letter list index */
    int         optchar, short_too;
    static int  posixly_correct = -1;

#if defined(NEED_PROGNAME)
    /* store progam name before any other parsing is done */
    getopt_progname = nargv[0];
#endif // NEED_PROGNAME

    if (options == M_NULLPTR)
        return (-1);

    /*
     * XXX Some GNU programs (like cvs) set optind to 0 instead of
     * XXX using optreset.  Work around this braindamage.
     */
    if (optind == 0)
        optind = optreset = 1;

    /*
     * Disable GNU extensions if POSIXLY_CORRECT is set or options
     * string begins with a '+'.
     *
     * CV, 2009-12-14: Check POSIXLY_CORRECT anew if optind == 0 or
     *                 optreset != 0 for GNU compatibility.
     */
    if (posixly_correct == -1 || optreset != 0)
    {
#if defined(HAVE_GETENV_S) || (defined(_WIN32) && defined(_MSC_VER) && defined(__STDC_SECURE_LIB__)) ||                \
    (defined(__STDC_LIB_EXT1__) && defined(__STDC_WANT_LIB_EXT1__))
        /* MSFT/C11 annex K adds getenv_s, so use it when available to check if this exists */
        size_t size = 0;
        if (getenv_s(&size, M_NULLPTR, 0, posixlycorrectenv) == 0)
        {
            /*
             * You can allocate a buffer based off of size and call it again to read it,
             * however, this is not necessary. We just need to know if this exists or not
             * since that is how to getenv line below was set to work before this _s function was added.
             */
            posixly_correct = 1;
        }
        else
        {
            posixly_correct = 0;
        }
#elif defined(HAVE_SECURE_GETENV) && !defined(DISABLE_SECURE_GETENV)
        /*
         * Use secure_getenv, unless the DISABLE_SECURE_GETENV is defined
         * secure_getenv (when available) is used by default unless DISABLE_SECURE_GETENV is defined
         * by the person building this library.
         * See https://linux.die.net/man/3/secure_getenv for reasons to disable it.
         */
        posixly_correct = (secure_getenv(posixlycorrectenv) != M_NULLPTR);
#elif defined(HAVE___SECURE_GETENV) && !defined(DISABLE_SECURE_GETENV)
        /*
         * Use secure_getenv, unless the DISABLE_SECURE_GETENV is defined
         * secure_getenv (when available) is used by default unless DISABLE_SECURE_GETENV is defined
         * by the person building this library.
         * See https://linux.die.net/man/3/secure_getenv for reasons to disable it.
         */
        posixly_correct = (__secure_getenv(posixlycorrectenv) != M_NULLPTR);
#else
        posixly_correct = (getenv(posixlycorrectenv) != M_NULLPTR);
#endif
    }
    if (*options == '-')
        flags |= FLAG_ALLARGS;
    else if (posixly_correct || *options == '+')
        flags &= ~FLAG_PERMUTE;
    if (*options == '+' || *options == '-')
        options++;

    optarg = M_NULLPTR;
    if (optreset)
        nonopt_start = nonopt_end = -1;
start:
    if (optreset || !*place)
    { /* update scanning pointer */
        optreset = 0;
        if (optind >= nargc)
        { /* end of argument vector */
            place = M_CONST_CAST(char*, EMSG);
            if (nonopt_end != -1)
            {
                /* do permutation, if we have to */
                permute_args(nonopt_start, nonopt_end, optind, nargv);
                optind -= nonopt_end - nonopt_start;
            }
            else if (nonopt_start != -1)
            {
                /*
                 * If we skipped non-options, set optind
                 * to the first of them.
                 */
                optind = nonopt_start;
            }
            nonopt_start = nonopt_end = -1;
            return (-1);
        }
        if (*(place = nargv[optind]) != '-' || (place[1] == '\0' && strchr(options, '-') == M_NULLPTR))
        {
            place = M_CONST_CAST(char*, EMSG); /* found non-option */
            if (flags & FLAG_ALLARGS)
            {
                /*
                 * GNU extension:
                 * return non-option as argument to option 1
                 */
                optarg = nargv[optind++];
                return (INORDER);
            }
            if (!(flags & FLAG_PERMUTE))
            {
                /*
                 * If no permutation wanted, stop parsing
                 * at first non-option.
                 */
                return (-1);
            }
            /* do permutation */
            if (nonopt_start == -1)
                nonopt_start = optind;
            else if (nonopt_end != -1)
            {
                permute_args(nonopt_start, nonopt_end, optind, nargv);
                nonopt_start = optind - (nonopt_end - nonopt_start);
                nonopt_end   = -1;
            }
            optind++;
            /* process next argument */
            goto start;
        }
        if (nonopt_start != -1 && nonopt_end == -1)
            nonopt_end = optind;

        /*
         * If we have "-" do nothing, if "--" we are done.
         */
        if (place[1] != '\0' && *++place == '-' && place[1] == '\0')
        {
            optind++;
            place = M_CONST_CAST(char*, EMSG);
            /*
             * We found an option (--), so if we skipped
             * non-options, we have to permute.
             */
            if (nonopt_end != -1)
            {
                permute_args(nonopt_start, nonopt_end, optind, nargv);
                optind -= nonopt_end - nonopt_start;
            }
            nonopt_start = nonopt_end = -1;
            return (-1);
        }
    }

    /*
     * Check long options if:
     *  1) we were passed some
     *  2) the arg is not just "-"
     *  3) either the arg starts with -- we are getopt_long_only()
     */
    if (long_options != M_NULLPTR && place != nargv[optind] && (*place == '-' || (flags & FLAG_LONGONLY)))
    {
        short_too = 0;
        if (*place == '-')
            place++; /* --foo long option */
        else if (*place != ':' && strchr(options, *place) != M_NULLPTR)
            short_too = 1; /* could be short option too */

        optchar = parse_long_options(nargv, options, long_options, idx, short_too, flags);
        if (optchar != -1)
        {
            place = M_CONST_CAST(char*, EMSG);
            return (optchar);
        }
    }

    if ((optchar = M_STATIC_CAST(int, *place++)) == M_STATIC_CAST(int, ':') ||
        (optchar == M_STATIC_CAST(int, '-') && *place != '\0') || (oli = strchr(options, optchar)) == M_NULLPTR)
    {
        /*
         * If the user specified "-" and  '-' isn't listed in
         * options, return -1 (non-option) as per POSIX.
         * Otherwise, it is an unknown option character (or ':').
         */
        if (optchar == M_STATIC_CAST(int, '-') && *place == '\0')
            return (-1);
        if (!*place)
            ++optind;
        if (PRINT_ERROR)
            cmdline_warnx(CMDLINE_ERR_MSG_ILLOPTCHAR, optchar);
        optopt = optchar;
        return (BADCH);
    }
    if (long_options != M_NULLPTR && optchar == 'W' && oli[1] == ';')
    {
        /* -W long-option */
        if (*place) /* no space */
            /* NOTHING */;
        else if (++optind >= nargc)
        { /* no arg */
            place = M_CONST_CAST(char*, EMSG);
            if (PRINT_ERROR)
                cmdline_warnx(CMDLINE_ERR_MSG_RECARGCHAR, optchar);
            optopt = optchar;
            return (BADARG);
        }
        else /* white space */
            place = nargv[optind];
        optchar = parse_long_options(nargv, options, long_options, idx, 0, flags);
        place   = M_CONST_CAST(char*, EMSG);
        return (optchar);
    }
    if (*++oli != ':')
    { /* doesn't take argument */
        if (!*place)
            ++optind;
    }
    else
    { /* takes (optional) argument */
        optarg = M_NULLPTR;
        if (*place) /* no white space */
            optarg = place;
        else if (oli[1] != ':')
        { /* arg not optional */
            if (++optind >= nargc)
            { /* no arg */
                place = M_CONST_CAST(char*, EMSG);
                if (PRINT_ERROR)
                    cmdline_warnx(CMDLINE_ERR_MSG_RECARGCHAR, optchar);
                optopt = optchar;
                return (BADARG);
            }
            else
                optarg = nargv[optind];
        }
        place = M_CONST_CAST(char*, EMSG);
        ++optind;
    }
    /* dump back option letter */
    return (optchar);
}

#ifdef REPLACE_GETOPT
/*
 * getopt --
 *	Parse argc/argv argument vector.
 *
 * [eventually this will replace the BSD getopt]
 */
M_NODISCARD M_NULL_TERM_STRING(3) M_PARAM_RO(2)
    M_PARAM_RO(3) int getopt(int nargc, char* const* M_NONNULL nargv, const char* M_NULLABLE options)
{
    /*
     * We don't pass FLAG_PERMUTE to getopt_internal() since
     * the BSD getopt(3) (unlike GNU) has never done this.
     *
     * Furthermore, since many privileged programs call getopt()
     * before dropping privileges it makes sense to keep things
     * as simple (and bug-free) as possible.
     */
    return (getopt_internal(nargc, nargv, options, M_NULLPTR, M_NULLPTR, 0));
}
#endif /* REPLACE_GETOPT */

/*
 * getopt_long --
 *	Parse argc/argv argument vector.
 */
M_NODISCARD M_NULL_TERM_STRING(3) M_PARAM_RW(2) M_PARAM_RO(3) M_PARAM_RO(4)
    M_PARAM_WO(5) int getopt_long(int                             nargc,
                                  char* const* M_NONNULL          nargv,
                                  const char* M_NULLABLE          options,
                                  const struct option* M_NULLABLE long_options,
                                  int* M_NULLABLE                 idx)
{
    return (getopt_internal(nargc, nargv, options, long_options, idx, FLAG_PERMUTE));
}

/*
 * getopt_long_only --
 *	Parse argc/argv argument vector.
 */
M_NODISCARD M_NULL_TERM_STRING(3) M_PARAM_RW(2) M_PARAM_RO(3) M_PARAM_RO(4)
    M_PARAM_WO(5) int getopt_long_only(int                             nargc,
                                       char* const* M_NONNULL          nargv,
                                       const char* M_NULLABLE          options,
                                       const struct option* M_NULLABLE long_options,
                                       int* M_NULLABLE                 idx)
{
    return (getopt_internal(nargc, nargv, options, long_options, idx, FLAG_PERMUTE | FLAG_LONGONLY));
}
