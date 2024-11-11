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
// \file memory_safety.c
// \brief Implements functions to assist with managing memory as safely as
// possible

#include "memory_safety.h"
#include "constraint_handling.h"
#include "env_detect.h"
#include "math_utils.h"
#include "type_conversion.h"

#include <stdlib.h>
#include <string.h>

#if defined(VMK_CROSS_COMP)
#    include <mm_malloc.h> //doing this to shut up warnings about posix_memalign not available despite stdlib include  TJE
#endif                     // VMK_CROSS_COMP

#if defined(_WIN32)
#    include "windows_version_detect.h"
#    include <WinBase.h>
#    include <windows.h>
#else
#    include <unistd.h>
#endif

size_t get_System_Pagesize(void)
{
#if defined(UEFI_C_SOURCE)
    return SIZE_T_C(4096); // This is not the processor page size, just the pagesize
                           // allocated by EDK2. It's in
                           // <MdePkg/Include/Uefi/UedfiBaseType.h
#elif defined(POSIX_2001)
    // use sysconf: http://man7.org/linux/man-pages/man3/sysconf.3.html
    return long_to_sizet(sysconf(_SC_PAGESIZE));
#elif defined(_POSIX_VERSION) || defined(BSD4_4) || defined(USING_SUS2)
    // use get page size:
    // http://man7.org/linux/man-pages/man2/getpagesize.2.html note marked
    // legacy in SUSv2, but checking it if posix is missing for some reason
    return int_to_sizet(getpagesize())
#elif defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
    SYSTEM_INFO winSysInfo;
    safe_memset(&winSysInfo, sizeof(SYSTEM_INFO), 0, sizeof(SYSTEM_INFO));
    GetSystemInfo(&winSysInfo);
    return ulong_to_sizet(winSysInfo.dwPageSize);
#else
    return SIZE_T_C(4096); // most CPUs use a 4KiB page size. We can detect specific
                           // architectures using something different if we need to
                           // here-TJE
#endif
}

// NOTE: C11 says supported alignments are implementation defined
//       We may want an if/else to call back to a generic method if it fails
//       some day. (unlikely, so not done right now) NOTE: There may also be
//       other functions to do this for other compilers or systems, but they are
//       not known today. Add them as necessary NOTE: some systems may have
//       memalign instead of the posix definition, but it is not clear how to
//       detect that implementation with feature test macros. NOTE: In UEFI,
//       using the EDK2, malloc will provide an 8-byte alignment, so it may be
//       possible to do some aligned allocations using it without extra work.
//       but we can revist that later.
M_FUNC_ATTR_MALLOC void* malloc_aligned(size_t size, size_t alignment)
{
#if !defined(__MINGW32__) && !defined(UEFI_C_SOURCE) && defined(USING_C11) && !defined(_MSC_VER) &&                    \
    (!defined(__GLIBC__) || (defined(__GLIBC__) && ((__GLIBC__ > 2) || (defined(__GLIBC_MINOR__) && __GLIBC__ >= 2 &&  \
                                                                        __GLIBC_MINOR__ >= 16))))
    // C11 added an aligned alloc function we can use
    // One additional requirement of this function is that the size must be a
    // multiple of alignment, so we will check and round up the size to make
    // this easy.
    if (size % alignment)
    {
        size = size + alignment - (size % alignment);
    }
    return aligned_alloc(alignment, size);
#elif !defined(UEFI_C_SOURCE) && (defined(POSIX_2001) || defined(VMK_CROSS_COMP))
    // POSIX.1-2001 and higher define support for posix_memalign
    void* temp = M_NULLPTR;
    if (0 != posix_memalign(&temp, alignment, size))
    {
        temp = M_NULLPTR; // make sure the system we are running didn't change
                          // this.
    }
    return temp;
#elif !defined(UEFI_C_SOURCE) && (defined(__INTEL_COMPILER) || defined(__ICC))
    //_mm_malloc
    return _mm_malloc(C_CAST(int, size), C_CAST(int, alignment));
#elif !defined(UEFI_C_SOURCE) && defined(__MINGW32__) && !defined(__MINGW64_VERSION_MAJOR)
    // mingw32 has an aligned malloc function that is not available in mingw64.
    return __mingw_aligned_malloc(size, alignment);
#elif !defined(UEFI_C_SOURCE) && (defined(_MSC_VER) || defined(__MINGW64_VERSION_MAJOR))
    // use the MS _aligned_malloc function. Mingw64 will support this as well
    // from what I can find - TJE
    return _aligned_malloc(size, alignment);
#elif !defined(UEFI_C_SOURCE) && (defined(__linux__) || defined(_sun))
    // most systems will support POSIX's memalign, but fallback to this if
    // necessary NOTE: May need to include malloc.h on some systems if this is
    // not a known function from stdlib.h where it is sometimes included from
    // too-TJE
    return memalign(alignment, size);
#else
    // need a generic way to do this with some math and overallocating...not
    // preferred but can make it work. This can waste a LOT of memory in some
    // cases depending on the required alignment. This way means overallocating
    // and adding to get to the required alignment...then knowing how much we
    // over aligned by. Will store the original starting pointer right before
    // the aligned pointer we return to the caller.
    void* temp = M_NULLPTR;
    // printf("\trequested allocation: size = %zu  alignment = %zu\n", size,
    // alignment);
    if (size && (alignment > SIZE_T_C(0)) &&
        ((alignment & (alignment - SIZE_T_C(1))) == SIZE_T_C(0))) // Check that we have a size to allocate and enforce that the
                                              // alignment value is a power of 2.
    {
        size_t requiredExtraBytes = sizeof(size_t); // We will store the original beginning address in
                                                    // front of the return data pointer
        const size_t totalAllocation = size + alignment + requiredExtraBytes;
        temp                         = malloc(totalAllocation);
        if (temp)
        {
            const void* const originalLocation = temp;
            temp += requiredExtraBytes; // allow enough room for storing the
                                        // original pointer location
            // now offset based on the required alignment
            temp += alignment - (C_CAST(size_t, temp) % alignment);
            // aligned.
            // now save the original pointer location
            size_t* savedLocationData = C_CAST(size_t*, temp - sizeof(size_t));
            *savedLocationData        = C_CAST(size_t, originalLocation);
        }
    }
    // else
    //{
    //     printf("\trequest did not meet requirements for generic allocation
    //     function\n");
    // }
    return temp;
#endif // UEFI vs compiler/OS specific capabilities check
}

void free_aligned(void* ptr)
{
    // NOTE: Can probably consolidate this a bit for the cases calling free,
    // however these macros match what is being done in
    //       the aligned malloc above.
#if !defined(__MINGW32__) && !defined(UEFI_C_SOURCE) && defined(USING_C11) && !defined(_MSC_VER)
    // just call free
    free(ptr);
    return;
#elif !defined(UEFI_C_SOURCE) && (defined(POSIX_2001) || defined(VMK_CROSS_COMP))
    // POSIX.1-2001 and higher define support for posix_memalign
    // Just call free
    free(ptr);
    return;
#elif !defined(UEFI_C_SOURCE) && (defined(__INTEL_COMPILER) || defined(__ICC))
    //_mm_free
    _mm_free(ptr);
    return;
#elif !defined(UEFI_C_SOURCE) && defined(__MINGW32__) && !defined(__MINGW64_VERSION_MAJOR)
    // mingw32 has an aligned malloc function that is not available in mingw64.
    __mingw_aligned_free(ptr);
    return;
#elif !defined(UEFI_C_SOURCE) && (defined(_MSC_VER) || defined(__MINGW64_VERSION_MAJOR))
    // use the MS _aligned_free function
    _aligned_free(ptr);
    return;
#elif !defined(UEFI_C_SOURCE) && (defined(__linux__) || defined(_sun))
    free(ptr);
    return;
#else
    // original pointer
    if (ptr)
    {
        // find the starting address from the original allocation.
        void* tempPtr = ptr - sizeof(size_t); // this gets us to where it was stored
        // this will set it back to what it's supposed to be
        tempPtr = C_CAST(void*, *(C_CAST(size_t*, tempPtr)));
        free(tempPtr);
    }
    return;
#endif // UEFI vs compiler/OS specific capabilities check
}

M_FUNC_ATTR_MALLOC void* calloc_aligned(size_t num, size_t size, size_t alignment)
{
    // call malloc aligned and memset
    void*  zeroedMem = M_NULLPTR;
    size_t numSize   = num * size;
    if (numSize)
    {
        zeroedMem = malloc_aligned(numSize, alignment);
        if (zeroedMem)
        {
            safe_memset(zeroedMem, numSize, 0, numSize);
        }
    }
    return zeroedMem;
}

M_FUNC_ATTR_MALLOC void* realloc_aligned(void* alignedPtr, size_t originalSize, size_t size, size_t alignment)
{
    void* temp = M_NULLPTR;
    if (size == SIZE_T_C(0))
    {
        free_aligned(alignedPtr);
        alignedPtr = M_NULLPTR;
    }
    else
    {
        temp = malloc_aligned(size, alignment);
        if (alignedPtr && originalSize > SIZE_T_C(0) && temp)
        {
            safe_memcpy(temp, size, alignedPtr, originalSize);
        }
        if (alignedPtr && temp)
        {
            // free the old pointer
            free_aligned(alignedPtr);
            alignedPtr = M_NULLPTR;
        }
    }
    return temp;
}

M_FUNC_ATTR_MALLOC void* malloc_page_aligned(size_t size)
{
    size_t pageSize = get_System_Pagesize();
    if (pageSize)
    {
        return malloc_aligned(size, pageSize);
    }
    else
    {
        return M_NULLPTR;
    }
}

M_FUNC_ATTR_MALLOC void* calloc_page_aligned(size_t num, size_t size)
{
    size_t pageSize = get_System_Pagesize();
    if (pageSize)
    {
        return calloc_aligned(num, size, get_System_Pagesize());
    }
    else
    {
        return M_NULLPTR;
    }
}

M_FUNC_ATTR_MALLOC void* realloc_page_aligned(void* alignedPtr, size_t originalSize, size_t size)
{
    size_t pageSize = get_System_Pagesize();
    if (pageSize)
    {
        return realloc_aligned(alignedPtr, originalSize, size, get_System_Pagesize());
    }
    else
    {
        return M_NULLPTR;
    }
}

bool is_Empty(const void* ptrData, size_t lengthBytes)
{
    if (ptrData || lengthBytes == SIZE_T_C(0))
    {
        bool byteByByte = true;
        // Allocate single memory page as zeroes to compare for the range
        // instead of byte by byte for better performance.-TJE
        size_t              sysPageSize = get_System_Pagesize();
        const uint_fast8_t* byteptr     = C_CAST(const uint_fast8_t*, ptrData);
        if (sysPageSize != SIZE_T_C(0))
        {
            uint8_t* zeroes = C_CAST(uint8_t*, calloc_page_aligned(sysPageSize, sizeof(uint8_t)));
            if (zeroes)
            {
                size_t increment = M_Min(sysPageSize, lengthBytes);
                byteByByte       = false;
                for (size_t iter = SIZE_T_C(0); iter < lengthBytes; iter += increment)
                {
                    if (memcmp(&byteptr[iter], zeroes, increment) != 0)
                    {
                        safe_Free_page_aligned(C_CAST(void**, &zeroes));
                        return false;
                    }
                    // update incement amount to ensure this does not access
                    // invalid memory
                }
                safe_Free_page_aligned(C_CAST(void**, &zeroes));
            }
        }
        if (byteByByte)
        {
            for (size_t iter = SIZE_T_C(0), iterEnd = lengthBytes - SIZE_T_C(1); iter < lengthBytes && iterEnd >= iter; ++iter, --iterEnd)
            {
                if (byteptr[iter] != UINT8_C(0) || byteptr[iterEnd] != UINT8_C(0))
                {
                    return false;
                }
            }
        }
        return true;
    }
    return false;
}

#if defined(__has_builtin)
#    if __has_builtin(__builtin___clear_cache)
#        define HAS_BUILT_IN_CLEAR_CACHE
#    endif
#endif

void* explicit_zeroes(void* dest, size_t count)
{
    if (dest && count > SIZE_T_C(0))
    {
#if defined(USING_C23) || defined(HAVE_MEMSET_EXPLICIT)
        return memset_explicit(dest, 0, count);
#elif defined(HAVE_C11_ANNEX_K) || defined(HAVE_MEMSET_S)
        // use memset_s since it cannot be optimized out
        if (0 == memset_s(dest, count, 0, count))
        {
            return dest;
        }
        else
        {
            return M_NULLPTR;
        }
#elif (defined(_WIN32) && defined(_MSC_VER)) || defined(HAVE_MSFT_SECURE_ZERO_MEMORY) ||                               \
    defined(HAVE_MSFT_SECURE_ZERO_MEMORY2)
#    if !defined(NO_HAVE_MSFT_SECURE_ZERO_MEMORY2) &&                                                                  \
        (defined(HAVE_MSFT_SECURE_ZERO_MEMORY2) ||                                                                     \
         (defined(WIN_API_TARGET_VERSION) && WIN_API_TARGET_VERSION >= WIN_API_TARGET_WIN11_26100))
        // use secure zero memory 2
        // Cast is to remove warning about different volatile qualifiers
        return M_CONST_CAST(void*, SecureZeroMemory2(dest, count));
#    else
        // use microsoft's SecureZeroMemory function
        return SecureZeroMemory(dest, count);
#    endif
#elif (defined(__FreeBSD__) && __FreeBSD__ >= 11) || (defined(__OpenBSD__) && defined(OpenBSD5_5)) ||                  \
    (defined(__GLIBC__) && defined(__GLIBC_MINOR__) && __GLIBC__ >= 2 && __GLIBC_MINOR__ >= 25) ||                     \
    defined(HAVE_EXPLICIT_BZERO)
        // https://elixir.bootlin.com/musl/latest/source/src/string/explicit_bzero.c
        // <- seems to appear first in 1.1.20
        // https://man.freebsd.org/cgi/man.cgi?query=explicit_bzero
        // https://www.gnu.org/software/gnulib/manual/html_node/explicit_005fbzero.html
        // https://man.dragonflybsd.org/?command=explicit_bzero&section=3
        // illumos has this too, but differs from oracle's solaris which has
        // explicit_memset https://illumos.org/man/3C/explicit_bzero
        explicit_bzero(dest, count);
        return dest;
#elif (defined(__NetBSD__) && defined(__NetBSD_Version__) &&                                                           \
       __NetBSD_Version >= 7000000000L /* net bsd version 7.0 and up*/) ||                                             \
    defined(HAVE_EXPLICIT_MEMSET)
        // https://man.netbsd.org/NetBSD-8.0/explicit_memset.3
        // https://docs.oracle.com/cd/E88353_01/html/E37843/explicit-memset-3c.html
        // NOTE: Solaris 11.4.12 added this, but I cannot find it in illumos
        // based distributions
        //       Illumos does not list this, but lists explicit_bzero in their
        //       manual. Not sure what version to use, so letting meson detect
        //       and set the HAVE_...macros
        return explicit_memset(dest, 0, count);
#else
        safe_memset(dest, count, 0, count);
        return dest;
#endif
    }
    else
    {
        return M_NULLPTR;
    }
}

errno_t safe_memset(void* dest, rsize_t destsz, int ch, rsize_t count)
{
    errno_t error = 0;
    if (dest == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memset: dest is NULL", M_NULLPTR, error);
        errno = error;
        return error;
    }
    else if (destsz > RSIZE_MAX)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_memset: destsz > RSIZE_MAX", M_NULLPTR, error);
        errno = error;
        return error;
    }
    else if (count > RSIZE_MAX)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_memset: count > RSIZE_MAX", M_NULLPTR, error);
        errno = error;
        return error;
    }
    else if (count > destsz)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_memset: count > destsz", M_NULLPTR, error);
        errno = error;
        return error;
    }
    else
    {
        if (destsz == RSIZE_T_C(0) || count == RSIZE_T_C(0))
        {
            errno = error;
            return error;
        }
#    if defined(USING_C23) || defined(HAVE_MEMSET_EXPLICIT)
        memset_explicit(dest, ch, count);//NOLINT(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
#    elif (defined(__NetBSD__) && defined(__NetBSD_Version__) &&                                                       \
           __NetBSD_Version >= 7000000000L /* net bsd version 7.0 and up*/) ||                                         \
        defined(HAVE_EXPLICIT_MEMSET)
        // https://man.netbsd.org/NetBSD-8.0/explicit_memset.3
        // https://docs.oracle.com/cd/E88353_01/html/E37843/explicit-memset-3c.html
        // NOTE: Solaris 11.4.12 added this, but I cannot find it in illumos
        // based distributions
        //       Illumos does not list this, but lists explicit_bzero in their
        //       manual. Not sure what version to use, so letting meson detect
        //       and set the HAVE_...macros
        explicit_memset(dest, ch, count);//NOLINT(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
#    else
        // last attempts to prevent optimization as best we can
#        if defined(__GNUC__) || defined(__clang__)
        memset(dest, ch, count);//NOLINT(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
        asm volatile("" ::: "memory");
#        elif defined(HAS_BUILT_IN_CLEAR_CACHE)
        memset(dest, ch, count);//NOLINT(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
        __builtin___clear_cache(dest, dest + count);
#        elif defined(_MSC_VER)
#            if !defined(NO_HAVE_MSFT_SECURE_ZERO_MEMORY2) &&                                                          \
                (defined(HAVE_MSFT_SECURE_ZERO_MEMORY2) ||                                                             \
                 (defined(WIN_API_TARGET_VERSION) && WIN_API_TARGET_VERSION >= WIN_API_TARGET_WIN11_26100))
        // SecureZeroMemory2 calls FillVolatileMemory which we can use here to
        // do the same thing
        FillVolatileMemory(dest, count, ch);//NOLINT(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
#            elif defined(_M_AMD64) || (!defined(_M_CEE) && defined(_M_ARM) || defined(_M_ARM64) || defined(_M_ARM64EC))
        // NOTE: Using the securezeromemory implementation in this case
        volatile char* vptr = M_REINTERPRET_CAST(volatile char*, dest);
#                if defined(_M_AMD64) && !defined(_M_ARM64EC)
        __stosb(M_REINTERPRET_CAST(unsigned char*, M_STATIC_CAST(unsigned __int64, vptr)), M_STATIC_CAST(unsigned char, ch), count);//NOLINT(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
#                else
        while (count)
        {
#                    if !defined(_M_CEE) && (defined(_M_ARM) || defined(_M_ARM64) || defined(_M_ARM64EC))
            __iso_volatile_store8(vptr, M_STATIC_CAST(unsigned char, ch));
#                    else
            *vptr = M_STATIC_CAST(unsigned char, ch);
#                    endif
            vptr++;
            count--;
        }
#                endif
#            else
        /*if you hit this case for some reason, you will need to add an include
         * for <intrin.h>. Not currently done as SecureZeroMemory is used
         * instead*/
        /* https://learn.microsoft.com/en-us/cpp/intrinsics/readwritebarrier?view=msvc-170
         */
        memset(dest, ch, count);//NOLINT(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
        _ReadWriteBarrier();
#            endif
#        else /* compiler does not support a method above as a barrier, so use                                         \
                 this as a final way to try and prevent optimization */
        // one idea on the web is this ugly volatile function pointer to memset
        // to stop the compiler optimization
        void* (*const volatile no_optimize_memset)(void*, int, size_t) = memset;
        no_optimize_memset(dest, ch, count);
#        endif
#    endif
        errno = error;
        return error;
    }
}

// malloc in standards leaves malloc'ing size 0 as a undefined behavior.
// this version will always return a null pointer if the size is zero
M_FUNC_ATTR_MALLOC void* safe_malloc(size_t size)
{
    if (size == SIZE_T_C(0))
    {
        errno = EINVAL;
        invoke_Constraint_Handler("safe_malloc: size == 0", M_NULLPTR, errno);
        return M_NULLPTR;
    }
    else
    {
        return malloc(size);
    }
}

// avoiding undefined behavior allocing zero size and avoiding alloc'ing less
// memory due to an overflow If alloc'ing zero or alloc would overflow size_t
// from count * size, then return a null pointer
M_FUNC_ATTR_MALLOC void* safe_calloc(size_t count, size_t size)
{
    if (count == SIZE_T_C(0))
    {
        // do not alloc size zero
        errno = EINVAL;
        invoke_Constraint_Handler("safe_calloc: count == 0", M_NULLPTR, errno);
        return M_NULLPTR;
    }
    else if (size == SIZE_T_C(0))
    {
        // do not alloc size zero
        errno = EINVAL;
        invoke_Constraint_Handler("safe_calloc: size == 0", M_NULLPTR, errno);
        return M_NULLPTR;
    }
    else if (count > (SIZE_MAX / size))
    {
        // avoid overflow due to inputs
        errno = EINVAL;
        invoke_Constraint_Handler("safe_calloc: count * size = overflow", M_NULLPTR, errno);
        return M_NULLPTR;
    }
    else
    {
        return calloc(count, size);
    }
}

// if passed a null pointer, behaves as safe_Malloc
// if size is zero, will perform free and return NULL ptr
M_FUNC_ATTR_MALLOC void* safe_realloc(void* block, size_t size)
{
    if (block == M_NULLPTR)
    {
        return safe_malloc(size);
    }
    else if (size == SIZE_T_C(0))
    {
        free(block);
        return M_NULLPTR;
    }
    else
    {
        // While using a temporary pointer here does not do anything different
        // than a simple return realloc, the purpose of this is to help reduce
        // false positives with SAST tools.
        void* newblock = realloc(block, size);
        if (newblock == M_NULLPTR)
        {
            return M_NULLPTR;
        }
        else
        {
            return newblock;
        }
    }
}

// if pointer to block is NULL, returns NULL
// if pointer to block is passed as a null pointer, behaves as safe_Malloc
// if size is zero, will perform free and return NULL ptr
// if realloc fails, free's original block
// free's original block if realloc fails
M_FUNC_ATTR_MALLOC void* safe_reallocf(void** block, size_t size)
{
    if (block == M_NULLPTR)
    {
        return M_NULLPTR;
    }
    else if (*block == M_NULLPTR)
    {
        return safe_malloc(size);
    }
    else if (size == SIZE_T_C(0))
    {
        free(*block);
        *block = M_NULLPTR;
        return M_NULLPTR;
    }
    else
    {
        void* newblock = realloc(*block, size);
        if (newblock == M_NULLPTR && size != SIZE_T_C(0))
        {
            free(*block);
            *block = M_NULLPTR;
        }
        return newblock;
    }
}

// This rounds up to the neareset power of 2 to ensure memory is aligned as
// expected and conforms with API requirements
static size_t alignment_Round_Up(size_t alignment)
{
    // first make sure the alignment is at least sizeof(void*), anything lower
    // than this doesn't make sense to align memory for as anything less will
    // also be compatible with void*
    if (alignment < sizeof(void*))
    {
        alignment = sizeof(void*);
    }
    // round up to next power of 2 if necessary
    if ((alignment & (alignment - SIZE_T_C(1))) != SIZE_T_C(0))
    {
        size_t rounded_alignment = sizeof(void*);
        while (rounded_alignment < alignment)
        {
            rounded_alignment <<= SIZE_T_C(1);
        }
        alignment = rounded_alignment;
    }
    return alignment;
}

// to be used after rounding up the alignment so that all allocations are
// multiples of the alignment which most, if not all, of the aligned allocation
// functions require
static size_t aligned_Size_Round_Up(size_t size, size_t alignment)
{
    // do not round up if it would cause an overflow of the size value
    if (size > (SIZE_MAX - alignment + SIZE_T_C(1)))
    {
        return size;
    }
    // round to nearest multiple of alignment
    size_t remainder = size % alignment;
    if (remainder != SIZE_T_C(0))
    {
        size += alignment - remainder;
    }
    return size;
}

// malloc in standards leaves malloc'ing size 0 as a undefined behavior.
// this version will always return a null pointer if the size is zero
M_FUNC_ATTR_MALLOC void* safe_malloc_aligned(size_t size, size_t alignment)
{
    if (size == SIZE_T_C(0))
    {
        // do not alloc size zero
        errno = EINVAL;
        invoke_Constraint_Handler("safe_malloc_aligned: count == 0", M_NULLPTR, errno);
        return M_NULLPTR;
    }
    else
    {
        alignment = alignment_Round_Up(alignment);
        size      = aligned_Size_Round_Up(size, alignment);
        return malloc_aligned(size, alignment);
    }
}

// avoiding undefined behavior allocing zero size and avoiding alloc'ing less
// memory due to an overflow If alloc'ing zero or alloc would overflow size_t
// from count * size, then return a null pointer
M_FUNC_ATTR_MALLOC void* safe_calloc_aligned(size_t count, size_t size, size_t alignment)
{
    if (count == SIZE_T_C(0))
    {
        // do not alloc size zero
        errno = EINVAL;
        invoke_Constraint_Handler("safe_calloc_aligned: count == 0", M_NULLPTR, errno);
        return M_NULLPTR;
    }
    else if (size == SIZE_T_C(0))
    {
        // do not alloc size zero
        errno = EINVAL;
        invoke_Constraint_Handler("safe_calloc_aligned: size == 0", M_NULLPTR, errno);
        return M_NULLPTR;
    }
    else if (count > (SIZE_MAX / size))
    {
        // avoid overflow due to inputs
        errno = EINVAL;
        invoke_Constraint_Handler("safe_calloc_aligned: count * size = overflow", M_NULLPTR, errno);
        return M_NULLPTR;
    }
    else
    {
        // instead of calling calloc_aligned, call safe_Malloc_aligned since it
        // will round alignment and size for us.
        void*  zeroedMem = M_NULLPTR;
        size_t numSize   = count * size;
        zeroedMem        = safe_malloc_aligned(numSize, alignment);
        if (zeroedMem != M_NULLPTR)
        {
            safe_memset(zeroedMem, numSize, 0, numSize);
        }
        return zeroedMem;
    }
}

// if passed a null pointer, behaves as safe_Malloc
// if size is zero, will perform free and return NULL ptr
M_FUNC_ATTR_MALLOC void* safe_realloc_aligned(void* block, size_t originalSize, size_t size, size_t alignment)
{
    if (block == M_NULLPTR)
    {
        return safe_malloc_aligned(size, alignment);
    }
    else if (size == SIZE_T_C(0))
    {
        free_aligned(block);
        return M_NULLPTR;
    }
    else
    {
        alignment = alignment_Round_Up(alignment);
        size      = aligned_Size_Round_Up(size, alignment);
        // While using a temporary pointer here does not do anything different
        // than a simple return realloc, the purpose of this is to help reduce
        // false positives with SAST tools.
        void* newblock = realloc_aligned(block, originalSize, size, alignment);
        if (newblock == M_NULLPTR)
        {
            return M_NULLPTR;
        }
        else
        {
            return newblock;
        }
    }
}

// if pointer to block is NULL, returns NULL
// if pointer to block is passed as a null pointer, behaves as safe_Malloc
// if size is zero, will perform free and return NULL ptr
// if realloc fails, free's original block
// free's original block if realloc fails
M_FUNC_ATTR_MALLOC void* safe_reallocf_aligned(void** block, size_t originalSize, size_t size, size_t alignment)
{
    if (block == M_NULLPTR)
    {
        return M_NULLPTR;
    }
    else if (*block == M_NULLPTR)
    {
        return safe_malloc_aligned(size, alignment);
    }
    else if (size == SIZE_T_C(0))
    {
        free_aligned(*block);
        *block = M_NULLPTR;
        return M_NULLPTR;
    }
    else
    {
        alignment      = alignment_Round_Up(alignment);
        size           = aligned_Size_Round_Up(size, alignment);
        void* newblock = realloc_aligned(*block, originalSize, size, alignment);
        if (newblock == M_NULLPTR && *block && size != SIZE_T_C(0))
        {
            free_aligned(*block);
            *block = M_NULLPTR;
        }
        return newblock;
    }
}

// malloc in standards leaves malloc'ing size 0 as a undefined behavior.
// this version will always return a null pointer if the size is zero
M_FUNC_ATTR_MALLOC void* safe_malloc_page_aligned(size_t size)
{
    return safe_malloc_aligned(size, get_System_Pagesize());
}

// avoiding undefined behavior allocing zero size and avoiding alloc'ing less
// memory due to an overflow If alloc'ing zero or alloc would overflow size_t
// from count * size, then return a null pointer
M_FUNC_ATTR_MALLOC void* safe_calloc_page_aligned(size_t count, size_t size)
{
    return safe_calloc_aligned(count, size, get_System_Pagesize());
}

// if passed a null pointer, behaves as safe_Malloc
// if size is zero, will perform free and return NULL ptr
M_FUNC_ATTR_MALLOC void* safe_realloc_page_aligned(void* block, size_t originalSize, size_t size)
{
    return safe_realloc_aligned(block, originalSize, size, get_System_Pagesize());
}

// if pointer to block is NULL, returns NULL
// if pointer to block is passed as a null pointer, behaves as safe_Malloc
// if size is zero, will perform free and return NULL ptr
// if realloc fails, free's original block
// free's original block if realloc fails
M_FUNC_ATTR_MALLOC void* safe_reallocf_page_aligned(void** block, size_t originalSize, size_t size)
{
    return safe_reallocf_aligned(block, originalSize, size, get_System_Pagesize());
}

// Calls memmove_s if available, otherwise performs all checks that memmove_s
// does before calling memmove
errno_t safe_memmove(void* dest, rsize_t destsz, const void* src, rsize_t count)
{
    errno_t error = 0;
    errno = 0;
    if (dest == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memmove: dest is NULL", M_NULLPTR, error);
        errno = error;
        return error;
    }
    else if (src == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memmove: src is NULL", M_NULLPTR, error);
    }
    else if (destsz > RSIZE_MAX)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memmove: destsz > RSIZE_MAX", M_NULLPTR, error);
    }
    else if (count > RSIZE_MAX)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memmove: count > RSIZE_MAX", M_NULLPTR, error);
    }
    else if (count > destsz)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_memmove: count > destsz", M_NULLPTR, error);
    }
    if (error != 0)
    {
        // on error when dest is not NULL and destsz is valid, then this memset
        // is done
        if (destsz <= RSIZE_MAX)
        {
            safe_memset(dest, destsz, 0, destsz);
        }
        errno = error;
        return error;
    }
    else
    {
        if (destsz > RSIZE_T_C(0) && count > RSIZE_T_C(0))
        {
#    if defined(HAVE_MSFT_SECURE_LIB)
            // This is microsoft's version, but it does not do the same checks
            // as C11 standard. Even though we've already done all the checks we
            // need we are calling this because it prevents additional warning
            // from Microsoft's compiler.
            memmove_s(dest, destsz, src, count);
#    else
            memmove(dest, src, count);// NOLINT(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
#    endif //HAVE_MSFT_SECURE_LIB
        }
        errno = 0;
        return error;
    }
}

// calls memcpy_s if available, otherwise performs all checks that memcpy_s does
// before calling memcpy
errno_t safe_memcpy(void* M_RESTRICT dest, rsize_t destsz, const void* M_RESTRICT src, rsize_t count)
{
    errno_t error = 0;
    errno = 0;
    if (dest == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memcpy: dest is NULL", M_NULLPTR, error);
        errno = error;
        return error;
    }
    else if (src == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memcpy: src is NULL", M_NULLPTR, error);
    }
    else if (destsz > RSIZE_MAX)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memcpy: destsz > RSIZE_MAX", M_NULLPTR, error);
    }
    else if (count > RSIZE_MAX)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memcpy: count > RSIZE_MAX", M_NULLPTR, error);
    }
    else if (count > destsz)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_memcpy: count > destsz", M_NULLPTR, error);
    }
    else if (memory_regions_overlap(dest, destsz, src, count))
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memcpy: dest and src pointer overlap. "
                                  "Use safe_memmove instead.",
                                  M_NULLPTR, error);
    }
    if (error != 0)
    {
        // on error when dest is not NULL and destsz is valid, then this memset
        // is done
        if (destsz <= RSIZE_MAX)
        {
            safe_memset(dest, destsz, 0, destsz);
        }
        errno = error;
        return error;
    }
    else
    {
        if (destsz > RSIZE_T_C(0) && count > RSIZE_T_C(0))
        {
#    if defined(HAVE_MSFT_SECURE_LIB)
            // This is microsoft's version, but it does not do the same checks
            // as C11 standard. Even though we've already done all the checks we
            // need we are calling this because it prevents additional warning
            // from Microsoft's compiler.
            memcpy_s(dest, destsz, src, count);
#    else
            memcpy(dest, src, count); // NOLINT(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
#    endif //HAVE_MSFT_SECURE_LIB
        }
        errno = error;
        return error;
    }
}

errno_t safe_memccpy(void* M_RESTRICT dest, rsize_t destsz, const void* M_RESTRICT src, int c, rsize_t count)
{
    errno_t error = 0;
    errno = 0;
    if (dest == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memccpy: dest is NULL", M_NULLPTR, error);
        errno = error;
        return error;
    }
    else if (src == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memccpy: src is NULL", M_NULLPTR, error);
    }
    else if (destsz > RSIZE_MAX)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_memccpy: destsz > RSIZE_MAX", M_NULLPTR, error);
    }
    else if (count > RSIZE_MAX)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_memccpy: count > RSIZE_MAX", M_NULLPTR, error);
    }
    else if (count > destsz)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_memccpy: count > destsz", M_NULLPTR, error);
    }
    else if ((src <= dest && C_CAST(void*, C_CAST(uintptr_t, src) + count) > dest) ||
             (dest <= src && C_CAST(void*, C_CAST(uintptr_t, dest) + count) > src))
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memccpy: dest and src pointer overlap. "
                                  "Use safe_memcmove instead.",
                                  M_NULLPTR, error);
    }
    if (error != 0)
    {
        // on error when dest is not NULL and destsz is valid, then this memset
        // is done
        if (destsz <= RSIZE_MAX)
        {
            safe_memset(dest, destsz, 0, destsz);
        }
        errno = error;
        return error;
    }
    else
    {
        if (destsz > RSIZE_T_C(0) && count > RSIZE_T_C(0))
        {
            size_t counter = SIZE_T_C(0);
            for (; counter < count; ++counter)
            {
                M_REINTERPRET_CAST(unsigned char*, dest)
                [counter] = M_REINTERPRET_CAST(unsigned const char*, src)[counter];
                if (M_STATIC_CAST(unsigned char, c) ==
                    M_STATIC_CAST(unsigned char, M_REINTERPRET_CAST(unsigned const char*, src)[counter]))
                {
                    break;
                }
            }
        }
        errno = error;
        return error;
    }
}

// allows overlapping ranges
errno_t safe_memcmove(void* M_RESTRICT dest, rsize_t destsz, const void* M_RESTRICT src, int c, rsize_t count)
{
    errno_t error = 0;
    errno = 0;
    if (dest == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memcmove: dest is NULL", M_NULLPTR, error);
        errno = error;
        return error;
    }
    else if (src == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memcmove: src is NULL", M_NULLPTR, error);
    }
    else if (destsz > RSIZE_MAX)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_memcmove: destsz > RSIZE_MAX", M_NULLPTR, error);
    }
    else if (count > RSIZE_MAX)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_memcmove: count > RSIZE_MAX", M_NULLPTR, error);
    }
    else if (count > destsz)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_memcmove: count > destsz", M_NULLPTR, error);
    }
    else if (dest == src)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memcmove: dest and src are the same!", M_NULLPTR, error);
        errno = error;
        return error;
    }
    if (error != 0)
    {
        // on error when dest is not NULL and destsz is valid, then this memset
        // is done
        if (destsz <= RSIZE_MAX)
        {
            safe_memset(dest, destsz, 0, destsz);
        }
        errno = error;
        return error;
    }
    else
    {
        if (destsz > RSIZE_T_C(0) && count > RSIZE_T_C(0))
        {
            if (dest < src)
            {
                // forward copy to avoid overlap
                size_t counter = SIZE_T_C(0);
                for (; counter < count; ++counter)
                {
                    M_REINTERPRET_CAST(unsigned char*, dest)
                    [counter] = M_REINTERPRET_CAST(unsigned const char*, src)[counter];
                    if (M_STATIC_CAST(unsigned char, c) ==
                        M_STATIC_CAST(unsigned char, M_REINTERPRET_CAST(unsigned const char*, src)[counter]))
                    {
                        break;
                    }
                }
            }
            else
            {
                // backward copy to avoid overlap
                size_t counter = count;
                void*  found   = memchr(src, c, count);
                if (found != M_NULLPTR)
                {
                    counter = M_STATIC_CAST(uintptr_t, found) - M_STATIC_CAST(uintptr_t, src) + SIZE_T_C(1);
                }
                for (; counter > SIZE_T_C(0); --counter)
                {
                    M_REINTERPRET_CAST(unsigned char*, dest)
                    [counter - SIZE_T_C(1)] = M_REINTERPRET_CAST(unsigned const char*, src)[counter - SIZE_T_C(1)];
                    // checking for C is not necessary since memchr already
                    // found it and we already adjusted the memory range we are
                    // working within.-TJE
                }
            }
        }
        errno = error;
        return error;
    }
}
