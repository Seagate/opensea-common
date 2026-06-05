// SPDX-License-Identifier: MPL-2.0

//! \file memory_safety.c
//! \brief Implements functions to assist with managing memory safely and mitigate common
//! errors.
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2026 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "memory_safety.h"
#include "bit_manip.h"
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
DISABLE_WARNING_4255
#    include <windows.h>
RESTORE_WARNING_4255
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
    return int_to_sizet(getpagesize());
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

//! \brief Store the original malloc pointer and compute the aligned pointer
//! \details For the generic fallback aligned allocation method, computes the properly
//!          aligned pointer offset and stores the original malloc pointer immediately
//!          before the aligned pointer to enable correct deallocation.
//! \param[in] malloc_ptr    The pointer returned from malloc() before alignment adjustment
//! \param[in] alignment     The required alignment (must be a power of 2)
//! \param[in] original_ptr  The original pointer from malloc() to store for deallocation
//! \return The aligned pointer to return to the caller, or NULL if malloc_ptr was NULL
M_PARAM_WO(1)
M_PARAM_RO(3) M_ATTR_UNUSED static M_INLINE
    void* store_aligned_original_ptr(void* M_NULLABLE malloc_ptr, size_t alignment, const void* M_NONNULL original_ptr)
{
    if (malloc_ptr == M_NULLPTR)
    {
        return M_NULLPTR;
    }

    // Offset to make room for storing the original pointer metadata
    const size_t requiredExtraBytes = sizeof(size_t);
    uintptr_t    alignedAddr        = C_CAST(uintptr_t, malloc_ptr) + requiredExtraBytes;

    // Calculate alignment offset to reach the next alignment boundary
    size_t alignmentOffset = alignment - (alignedAddr % alignment);
    alignedAddr += alignmentOffset;

    // Store the original pointer at the metadata location (just before the aligned pointer)
    void*   metaPtr           = C_CAST(void*, alignedAddr - requiredExtraBytes);
    size_t* savedLocationData = C_CAST(size_t*, metaPtr);
    *savedLocationData        = C_CAST(size_t, original_ptr);

    return C_CAST(void*, alignedAddr);
}

//! \brief Retrieve the original malloc pointer stored before an aligned pointer
//! \details For the generic fallback aligned allocation method, retrieves the original
//!          malloc pointer that was stored immediately before the aligned pointer.
//!          Handles NULL input safely, matching free() semantics: NULL input returns NULL.
//! \param[in] aligned_ptr The aligned pointer being freed
//! \return The original malloc pointer to free, or NULL if aligned_ptr was NULL
M_PARAM_RO(1) M_ATTR_UNUSED static M_INLINE void* retrieve_aligned_original_ptr(void* M_NULLABLE aligned_ptr)
{
    if (aligned_ptr == M_NULLPTR)
    {
        return M_NULLPTR;
    }

    // Find the starting address from the original allocation
    uintptr_t alignedAddr = C_CAST(uintptr_t, aligned_ptr);
    alignedAddr -= sizeof(size_t);
    void* metaPtr  = C_CAST(void*, alignedAddr);
    void* original = C_CAST(void*, *(C_CAST(size_t*, metaPtr)));
    return original;
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

// Platform-specific aligned memory allocation strategies
// These flags define which allocation/deallocation functions are available
// based on compiler, OS, and feature support.
// Note: Intel's compiler has _mm_malloc and _mm_free but these are just wrappers the same as our own. No need to keep
// those right now since we are otherwise detecting and using the proper APIs on the platforms we do support.
#if !defined(UEFI_C_SOURCE)

// C11 standard aligned_alloc + free support
#    if defined(USING_C11) && !defined(__MINGW32__) && !defined(_MSC_VER) &&                                           \
        (!defined(THIS_IS_GLIBC) || IS_GLIBC_VERSION(2, 16))
#        define USE_C11_ALIGNED
#    endif // C11 aligned_alloc support

// POSIX posix_memalign + free support
#    if (defined(POSIX_2001) && defined(_POSIX_ADVISORY_INFO)) || defined(VMK_CROSS_COMP)
#        define USE_POSIX_ALIGNED
#    endif // POSIX memalign support

// MinGW32 __mingw_aligned_malloc + __mingw_aligned_free support
#    if defined(__MINGW32__) && !defined(__MINGW64_VERSION_MAJOR)
#        define USE_MINGW_ALIGNED
#    endif // MinGW32 aligned support

// MSVC _aligned_malloc + _aligned_free support (also for MinGW64)
#    if IS_MSVC_VERSION(MSVC_2005) || defined(__MINGW64_VERSION_MAJOR)
#        define USE_MSVC_ALIGNED
#    endif // MSVC aligned support

// Linux/Sun memalign + free support
#    if defined(__linux__) || defined(_sun)
#        define USE_MEMALIGN
#    endif // Linux/Sun memalign support

#endif // !UEFI_C_SOURCE

// Free method abstraction based on allocation method
// These define which deallocation function to use
#if defined(USE_C11_ALIGNED) || defined(USE_POSIX_ALIGNED) || defined(USE_MEMALIGN)
#    define USE_STD_FREE
#endif // Standard free() functions

#if defined(USE_MINGW_ALIGNED)
#    define USE_MINGW_FREE
#endif // MinGW aligned free

#if defined(USE_MSVC_ALIGNED)
#    define USE_MSVC_FREE
#endif // MSVC aligned free

// Compile-time validation of mutually exclusive platform choices
// Mismatched allocation/deallocation functions cause runtime crashes

// Check: MinGW and MSVC cannot coexist
#if defined(USE_MINGW_ALIGNED) && defined(USE_MSVC_ALIGNED)
#    error "Configuration error: USE_MINGW_ALIGNED and USE_MSVC_ALIGNED are mutually exclusive. " \
            "Check platform detection: only one MinGW/MSVC variant should be defined."
#endif // MINGW/MSVC mutual exclusion

// Check: MinGW allocation must use MinGW free, not standard free
#if defined(USE_MINGW_ALIGNED)
#    if defined(USE_STD_FREE)
#        error "Configuration error: USE_MINGW_ALIGNED requires USE_MINGW_FREE, not USE_STD_FREE. " \
                "Mismatched __mingw_aligned_malloc + free() causes crashes."
#    endif
#    if !defined(USE_MINGW_FREE)
#        error "Configuration error: USE_MINGW_ALIGNED defined but USE_MINGW_FREE not defined. " \
                "MinGW aligned malloc requires __mingw_aligned_free for deallocation."
#    endif
#endif // MinGW allocation/free pairing

// Check: MSVC allocation must use MSVC free, not standard free
#if defined(USE_MSVC_ALIGNED)
#    if defined(USE_STD_FREE)
#        error "Configuration error: USE_MSVC_ALIGNED requires USE_MSVC_FREE, not USE_STD_FREE. " \
                "Mismatched _aligned_malloc + free() causes crashes."
#    endif
#    if !defined(USE_MSVC_FREE)
#        error "Configuration error: USE_MSVC_ALIGNED defined but USE_MSVC_FREE not defined. " \
                "MSVC aligned malloc requires _aligned_free for deallocation."
#    endif
#endif // MSVC allocation/free pairing

M_NODISCARD M_FUNC_ATTR_MALLOC M_ALLOC_DEALLOC(free_aligned, 1) M_ALLOC_ALIGN(2)
    M_MALLOC_SIZE(1) void* malloc_aligned(size_t size, size_t alignment)
{
#if defined(USE_C11_ALIGNED)
    // C11 added an aligned alloc function we can use
    // One additional requirement of this function is that the size must be a
    // multiple of alignment, so we will check and round up the size to make
    // this easy.
    if (size % alignment)
    {
        size = size + alignment - (size % alignment);
    }
    return aligned_alloc(alignment, size);
#elif defined(USE_POSIX_ALIGNED)
    // POSIX.1-2001 and higher define support for posix_memalign
    void* temp = M_NULLPTR;
    if (0 != posix_memalign(&temp, alignment, size))
    {
        temp = M_NULLPTR; // make sure the system we are running didn't change this.
    }
    return temp;
#elif defined(USE_MINGW_ALIGNED)
    // MinGW32 aligned malloc function
    return __mingw_aligned_malloc(size, alignment);
#elif defined(USE_MSVC_ALIGNED)
    // MSVC _aligned_malloc (also for MinGW64)
    return _aligned_malloc(size, alignment);
#elif defined(USE_MEMALIGN)
    // Linux/Sun memalign function
    return memalign(alignment, size);
#else
    // Generic fallback: manual alignment using overallocation
    // This allocates extra memory and computes an aligned offset within it,
    // storing the original pointer for proper deallocation.
    void* temp = M_NULLPTR;
    if (size && has_single_bit(alignment))
    {
        size_t       requiredExtraBytes = sizeof(size_t);
        const size_t totalAllocation    = size + alignment + requiredExtraBytes;
        temp                            = malloc(totalAllocation);
        if (temp != M_NULLPTR)
        {
            const void* originalLocation = temp;
            temp                         = store_aligned_original_ptr(temp, alignment, originalLocation);
        }
    }
    return temp;
#endif // Platform-specific aligned allocation
}

M_PARAM_WO(1) void free_aligned(void* ptr)
{
#if defined(USE_STD_FREE)
    // Standard free() for C11, POSIX, or Linux/Sun memalign
    free(ptr);
#elif defined(USE_MINGW_FREE)
    // MinGW32 aligned free function
    __mingw_aligned_free(ptr);
#elif defined(USE_MSVC_FREE)
    // MSVC _aligned_free (also for MinGW64)
    _aligned_free(ptr);
#else
    // Generic fallback: retrieve original pointer and free it
    // retrieve_aligned_original_ptr handles NULL input safely (returns NULL)
    void* original = retrieve_aligned_original_ptr(ptr);
    free(original); // free(NULL) is safe per C standard
#endif // Platform-specific aligned deallocation
}

M_NODISCARD M_FUNC_ATTR_MALLOC M_ALLOC_ALIGN(3) M_CALLOC_SIZE(1, 2)
    M_ALLOC_DEALLOC(free_aligned, 1) void* calloc_aligned(size_t num, size_t size, size_t alignment)
{
    // call malloc aligned and memset
    void*  zeroedMem = M_NULLPTR;
    size_t numSize   = num * size;
    if (numSize > SIZE_T_C(0))
    {
        zeroedMem = malloc_aligned(numSize, alignment);
        if (zeroedMem != M_NULLPTR)
        {
            safe_memset(zeroedMem, numSize, 0, numSize);
        }
    }
    return zeroedMem;
}
M_NODISCARD
M_PARAM_RW_SIZE(1, 2)
M_ALLOC_ALIGN(4)
M_MALLOC_SIZE(3)
void* realloc_aligned(void* alignedPtr, size_t originalSize, size_t size, size_t alignment)
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

M_NODISCARD M_FUNC_ATTR_MALLOC M_MALLOC_SIZE(1) void* malloc_page_aligned(size_t size)
{
    size_t pageSize = get_System_Pagesize();
    if (pageSize > SIZE_T_C(0))
    {
        return malloc_aligned(size, pageSize);
    }
    else
    {
        return M_NULLPTR;
    }
}

M_NODISCARD M_FUNC_ATTR_MALLOC M_CALLOC_SIZE(1, 2) void* calloc_page_aligned(size_t num, size_t size)
{
    size_t pageSize = get_System_Pagesize();
    if (pageSize > SIZE_T_C(0))
    {
        return calloc_aligned(num, size, get_System_Pagesize());
    }
    else
    {
        return M_NULLPTR;
    }
}
M_NODISCARD
M_PARAM_RW_SIZE(1, 2)
M_MALLOC_SIZE(3) void* realloc_page_aligned(void* alignedPtr, size_t originalSize, size_t size)
{
    size_t pageSize = get_System_Pagesize();
    if (pageSize > SIZE_T_C(0))
    {
        return realloc_aligned(alignedPtr, originalSize, size, get_System_Pagesize());
    }
    else
    {
        return M_NULLPTR;
    }
}

M_NONNULL_PARAM_LIST(1)
M_PARAM_RO_SIZE(1, 2) bool is_Buffer_All_ByteValue(const void* ptrData, size_t lengthBytes, uint8_t byteValue)
{
    DISABLE_NONNULL_COMPARE
    if (ptrData != M_NULLPTR && lengthBytes > SIZE_T_C(0))
    {
        const uint_fast8_t* byteptr = C_CAST(const uint_fast8_t*, ptrData);
        for (size_t iter = SIZE_T_C(0), iterEnd = lengthBytes - SIZE_T_C(1); iter < lengthBytes && iterEnd >= iter;
             ++iter, --iterEnd)
        {
            if (byteptr[iter] != byteValue || byteptr[iterEnd] != byteValue)
            {
                return false;
            }
        }
        return true;
    }
    RESTORE_NONNULL_COMPARE
    return false;
}

M_NONNULL_PARAM_LIST(1) M_PARAM_RO_SIZE(1, 2) bool is_Empty(const void* ptrData, size_t lengthBytes)
{
    return is_Buffer_All_ByteValue(ptrData, lengthBytes, 0);
}

M_PARAM_WO_SIZE(1, 2) void* explicit_zeroes(void* dest, size_t count)
{
    if (dest != M_NULLPTR && count > SIZE_T_C(0))
    {
#if defined(HAVE_MEMSET_EXPLICIT)
        return memset_explicit(dest, 0, count);
#elif defined(HAVE_C11_ANNEX_K)
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
#elif IS_FREEBSD_VERSION(11, 0, 0) || (defined(__OpenBSD__) && defined(OpenBSD5_5)) ||                                 \
    (defined(THIS_IS_GLIBC) && IS_GLIBC_VERSION(2, 25)) || defined(HAVE_EXPLICIT_BZERO)
        // https://elixir.bootlin.com/musl/latest/source/src/string/explicit_bzero.c
        // <- seems to appear first in 1.1.20
        // https://man.freebsd.org/cgi/man.cgi?query=explicit_bzero
        // https://www.gnu.org/software/gnulib/manual/html_node/explicit_005fbzero.html
        // https://man.dragonflybsd.org/?command=explicit_bzero&section=3
        // illumos has this too, but differs from oracle's solaris which has
        // explicit_memset https://illumos.org/man/3C/explicit_bzero
        explicit_bzero(dest, count);
        return dest;
#elif IS_NETBSD_VERSION(7, 0, 0) || defined(HAVE_EXPLICIT_MEMSET)
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

errno_t safe_memset_impl(void*       dest,
                         rsize_t     destsz,
                         int         ch,
                         rsize_t     count,
                         const char* file,
                         const char* function,
                         int         line,
                         const char* expression)
{
    errno_t           error = 0;
    constraintEnvInfo envInfo;
    if (dest == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memset: dest is NULL", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
        errno = error;
        return error;
    }
    else if (destsz > RSIZE_MAX)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_memset: destsz > RSIZE_MAX",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (count > RSIZE_MAX)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_memset: count > RSIZE_MAX",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (count > destsz)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_memset: count > destsz",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
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
#if defined(HAVE_MEMSET_EXPLICIT)
        memset_explicit(dest, ch, // NOLINT(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
                        count);   // NOLINT(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
#elif IS_NETBSD_VERSION(7, 0, 0) || defined(HAVE_EXPLICIT_MEMSET)
        // https://man.netbsd.org/NetBSD-8.0/explicit_memset.3
        // https://docs.oracle.com/cd/E88353_01/html/E37843/explicit-memset-3c.html
        // NOTE: Solaris 11.4.12 added this, but I cannot find it in illumos
        // based distributions
        //       Illumos does not list this, but lists explicit_bzero in their
        //       manual. Not sure what version to use, so letting meson detect
        //       and set the HAVE_...macros
        explicit_memset(dest, ch,
                        count); // NOLINT(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
#else
        // last attempts to prevent optimization as best we can
#    if IS_GCC_VERSION(3, 0) || IS_CLANG_VERSION(1, 0)
#        if defined(HAVE_BUILTIN_MEMSET)
        __builtin_memset(dest, ch, // NOLINT(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
                         count);   // NOLINT(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
#        else
        memset(dest, ch, count); // NOLINT(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
#        endif
        asm volatile("" ::: "memory");
#    elif IS_MSVC_VERSION(MSVC_2005)
#        if !defined(NO_HAVE_MSFT_SECURE_ZERO_MEMORY2) &&                                                              \
            (defined(HAVE_MSFT_SECURE_ZERO_MEMORY2) ||                                                                 \
             (defined(WIN_API_TARGET_VERSION) && WIN_API_TARGET_VERSION >= WIN_API_TARGET_WIN11_26100))
        // SecureZeroMemory2 calls FillVolatileMemory which we can use here to
        // do the same thing
        FillVolatileMemory(dest, count, // NOLINT(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
                           ch);         // NOLINT(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
#        elif defined(_M_AMD64) || (!defined(_M_CEE) && defined(_M_ARM) || defined(_M_ARM64) || defined(_M_ARM64EC))
        // NOTE: Using the securezeromemory implementation in this case
        volatile char* vptr = M_REINTERPRET_CAST(volatile char*, dest);
#            if defined(_M_AMD64) && !defined(_M_ARM64EC)
        __stosb(M_REINTERPRET_CAST(unsigned char*, M_STATIC_CAST(unsigned __int64, vptr)),
                M_STATIC_CAST(unsigned char, ch),
                count); // NOLINT(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
#            else
        while (count)
        {
#                if !defined(_M_CEE) && (defined(_M_ARM) || defined(_M_ARM64) || defined(_M_ARM64EC))
            __iso_volatile_store8(vptr, M_STATIC_CAST(unsigned char, ch));
#                else
            *vptr = M_STATIC_CAST(unsigned char, ch);
#                endif
            vptr++;
            count--;
        }
#            endif
#        else
        /*if you hit this case for some reason, you will need to add an include
         * for <intrin.h>. Not currently done as SecureZeroMemory is used
         * instead*/
        /* https://learn.microsoft.com/en-us/cpp/intrinsics/readwritebarrier?view=msvc-170
         */
        memset(dest, ch, count); // NOLINT(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
        _ReadWriteBarrier();
#        endif
#    else /* compiler does not support a method above as a barrier, so use                                             \
             this as a final way to try and prevent optimization */
        // one idea on the web is this ugly volatile function pointer to memset
        // to stop the compiler optimization
        void* (*const volatile no_optimize_memset)(void*, int, size_t) = memset;
        no_optimize_memset(dest, ch, count);
#    endif
#endif
        errno = error;
        return error;
    }
}

// malloc in standards leaves malloc'ing size 0 as a undefined behavior.
// this version will always return a null pointer if the size is zero
M_NODISCARD M_FUNC_ATTR_MALLOC M_MALLOC_SIZE(
    1) void* safe_malloc_impl(size_t size, const char* file, const char* function, int line, const char* expression)
{
    constraintEnvInfo envInfo;
    if (size == SIZE_T_C(0))
    {
        errno = EINVAL;
        invoke_Constraint_Handler("safe_malloc: size == 0", set_Env_Info(&envInfo, file, function, expression, line),
                                  errno);
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
M_NODISCARD M_FUNC_ATTR_MALLOC M_CALLOC_SIZE(1, 2) void* safe_calloc_impl(size_t      count,
                                                                          size_t      size,
                                                                          const char* file,
                                                                          const char* function,
                                                                          int         line,
                                                                          const char* expression)
{
    constraintEnvInfo envInfo;
    if (count == SIZE_T_C(0))
    {
        // do not alloc size zero
        errno = EINVAL;
        invoke_Constraint_Handler("safe_calloc: count == 0", set_Env_Info(&envInfo, file, function, expression, line),
                                  errno);
        return M_NULLPTR;
    }
    else if (size == SIZE_T_C(0))
    {
        // do not alloc size zero
        errno = EINVAL;
        invoke_Constraint_Handler("safe_calloc: size == 0", set_Env_Info(&envInfo, file, function, expression, line),
                                  errno);
        return M_NULLPTR;
    }
    else if (count > (SIZE_MAX / size))
    {
        // avoid overflow due to inputs
        errno = EINVAL;
        invoke_Constraint_Handler("safe_calloc: count * size = overflow",
                                  set_Env_Info(&envInfo, file, function, expression, line), errno);
        return M_NULLPTR;
    }
    else
    {
        return calloc(count, size);
    }
}

// if passed a null pointer, behaves as safe_Malloc
// if size is zero, will perform free and return NULL ptr
M_NODISCARD M_PARAM_RW(1) M_MALLOC_SIZE(2) void* safe_realloc(void* block, size_t size)
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
M_NODISCARD M_PARAM_RW(1) M_MALLOC_SIZE(2) void* safe_reallocf(void** block, size_t size)
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

// This rounds up to the nearest power of 2 to ensure memory is aligned as
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
    DISABLE_WARNING_CONVERSION_DATA_LOSS
    // Warning disabled in MSVC for excess noise
    return bit_ceil(alignment);
    RESTORE_WARNING_CONVERSION_DATA_LOSS
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
M_NODISCARD M_FUNC_ATTR_MALLOC M_MALLOC_SIZE(1) M_ALLOC_ALIGN(2) void* safe_malloc_aligned_impl(size_t      size,
                                                                                                size_t      alignment,
                                                                                                const char* file,
                                                                                                const char* function,
                                                                                                int         line,
                                                                                                const char* expression)
{
    constraintEnvInfo envInfo;
    if (size == SIZE_T_C(0))
    {
        // do not alloc size zero
        errno = EINVAL;
        invoke_Constraint_Handler("safe_malloc_aligned: count == 0",
                                  set_Env_Info(&envInfo, file, function, expression, line), errno);
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
M_NODISCARD M_FUNC_ATTR_MALLOC M_CALLOC_SIZE(1, 2)
    M_ALLOC_ALIGN(3) void* safe_calloc_aligned_impl(size_t      count,
                                                    size_t      size,
                                                    size_t      alignment,
                                                    const char* file,
                                                    const char* function,
                                                    int         line,
                                                    const char* expression)
{
    constraintEnvInfo envInfo;
    if (count == SIZE_T_C(0))
    {
        // do not alloc size zero
        errno = EINVAL;
        invoke_Constraint_Handler("safe_calloc_aligned: count == 0",
                                  set_Env_Info(&envInfo, file, function, expression, line), errno);
        return M_NULLPTR;
    }
    else if (size == SIZE_T_C(0))
    {
        // do not alloc size zero
        errno = EINVAL;
        invoke_Constraint_Handler("safe_calloc_aligned: size == 0",
                                  set_Env_Info(&envInfo, file, function, expression, line), errno);
        return M_NULLPTR;
    }
    else if (count > (SIZE_MAX / size))
    {
        // avoid overflow due to inputs
        errno = EINVAL;
        invoke_Constraint_Handler("safe_calloc_aligned: count * size = overflow",
                                  set_Env_Info(&envInfo, file, function, expression, line), errno);
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
M_NODISCARD
M_PARAM_RW_SIZE(1, 2)
M_ALLOC_ALIGN(4)
M_MALLOC_SIZE(3)
void* safe_realloc_aligned(void* block, size_t originalSize, size_t size, size_t alignment)
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
M_NODISCARD
M_PARAM_RW(1)
M_ALLOC_ALIGN(4)
M_MALLOC_SIZE(3)
void* safe_reallocf_aligned(void** block, size_t originalSize, size_t size, size_t alignment)
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
M_NODISCARD M_FUNC_ATTR_MALLOC M_MALLOC_SIZE(1) void* safe_malloc_page_aligned(size_t size)
{
    return safe_malloc_aligned(size, get_System_Pagesize());
}

// avoiding undefined behavior allocing zero size and avoiding alloc'ing less
// memory due to an overflow If alloc'ing zero or alloc would overflow size_t
// from count * size, then return a null pointer
M_NODISCARD M_FUNC_ATTR_MALLOC M_CALLOC_SIZE(1, 2) void* safe_calloc_page_aligned(size_t count, size_t size)
{
    return safe_calloc_aligned(count, size, get_System_Pagesize());
}

// if passed a null pointer, behaves as safe_Malloc
// if size is zero, will perform free and return NULL ptr
M_NODISCARD M_PARAM_RW_SIZE(1, 2)
    M_MALLOC_SIZE(3) void* safe_realloc_page_aligned(void* block, size_t originalSize, size_t size)
{
    return safe_realloc_aligned(block, originalSize, size, get_System_Pagesize());
}

// if pointer to block is NULL, returns NULL
// if pointer to block is passed as a null pointer, behaves as safe_Malloc
// if size is zero, will perform free and return NULL ptr
// if realloc fails, free's original block
// free's original block if realloc fails
M_NODISCARD
M_PARAM_RW(1)
M_MALLOC_SIZE(3) void* safe_reallocf_page_aligned(void** block, size_t originalSize, size_t size)
{
    return safe_reallocf_aligned(block, originalSize, size, get_System_Pagesize());
}

// Calls memmove_s if available, otherwise performs all checks that memmove_s
// does before calling memmove
errno_t safe_memmove_impl(void*       dest,
                          rsize_t     destsz,
                          const void* src,
                          rsize_t     count,
                          const char* file,
                          const char* function,
                          int         line,
                          const char* expression)
{
    errno_t           error = 0;
    constraintEnvInfo envInfo;
    errno = 0;
    if (dest == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memmove: dest is NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (src == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memmove: src is NULL", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
    }
    else if (destsz > RSIZE_MAX)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memmove: destsz > RSIZE_MAX",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
    }
    else if (count > RSIZE_MAX)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memmove: count > RSIZE_MAX",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
    }
    else if (count > destsz)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_memmove: count > destsz",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
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
#if defined(HAVE_MSFT_SECURE_LIB)
            // This is microsoft's version, but it does not do the same checks
            // as C11 standard. Even though we've already done all the checks we
            // need we are calling this because it prevents additional warning
            // from Microsoft's compiler.
            memmove_s(dest, destsz, src, count);
#elif defined(HAVE_BUILTIN_MEMMOVE)
            __builtin_memmove(dest, src, // NOLINT(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
                              count);    // NOLINT(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
#else
            memmove(dest, src, count); // NOLINT(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
#endif // HAVE_MSFT_SECURE_LIB
        }
        errno = 0;
        return error;
    }
}

// calls memcpy_s if available, otherwise performs all checks that memcpy_s does
// before calling memcpy
errno_t safe_memcpy_impl(void* M_RESTRICT       dest,
                         rsize_t                destsz,
                         const void* M_RESTRICT src,
                         rsize_t                count,
                         const char*            file,
                         const char*            function,
                         int                    line,
                         const char*            expression)
{
    errno_t           error = 0;
    constraintEnvInfo envInfo;
    errno = 0;
    if (dest == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memcpy: dest is NULL", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
        errno = error;
        return error;
    }
    else if (src == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memcpy: src is NULL", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
    }
    else if (destsz > RSIZE_MAX)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memcpy: destsz > RSIZE_MAX",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
    }
    else if (count > RSIZE_MAX)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memcpy: count > RSIZE_MAX",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
    }
    else if (count > destsz)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_memcpy: count > destsz",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
    }
    else if (memory_regions_overlap(dest, destsz, src, count))
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memcpy: dest and src pointer overlap. "
                                  "Use safe_memmove instead.",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
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
#if defined(HAVE_MSFT_SECURE_LIB)
            // This is microsoft's version, but it does not do the same checks
            // as C11 standard. Even though we've already done all the checks we
            // need we are calling this because it prevents additional warning
            // from Microsoft's compiler.
            memcpy_s(dest, destsz, src, count);
#elif defined(HAVE_BUILTIN_MEMCPY)
            __builtin_memcpy(dest, src, // NOLINT(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
                             count);    // NOLINT(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
#else
            memcpy(dest, src, count); // NOLINT(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
#endif // HAVE_MSFT_SECURE_LIB
        }
        errno = error;
        return error;
    }
}

errno_t safe_memccpy_impl(void* M_RESTRICT       dest,
                          rsize_t                destsz,
                          const void* M_RESTRICT src,
                          int                    c,
                          rsize_t                count,
                          const char*            file,
                          const char*            function,
                          int                    line,
                          const char*            expression)
{
    errno_t           error = 0;
    constraintEnvInfo envInfo;
    errno = 0;
    if (dest == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memccpy: dest is NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (src == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memccpy: src is NULL", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
    }
    else if (destsz > RSIZE_MAX)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_memccpy: destsz > RSIZE_MAX",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
    }
    else if (count > RSIZE_MAX)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_memccpy: count > RSIZE_MAX",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
    }
    else if (count > destsz)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_memccpy: count > destsz",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
    }
    else if ((src <= dest && C_CAST(void*, C_CAST(uintptr_t, src) + count) > dest) ||
             (dest <= src && C_CAST(void*, C_CAST(uintptr_t, dest) + count) > src))
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memccpy: dest and src pointer overlap. "
                                  "Use safe_memcmove instead.",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
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
errno_t safe_memcmove_impl(void* M_RESTRICT       dest,
                           rsize_t                destsz,
                           const void* M_RESTRICT src,
                           int                    c,
                           rsize_t                count,
                           const char*            file,
                           const char*            function,
                           int                    line,
                           const char*            expression)
{
    errno_t           error = 0;
    constraintEnvInfo envInfo;
    errno = 0;
    if (dest == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memcmove: dest is NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (src == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memcmove: src is NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
    }
    else if (destsz > RSIZE_MAX)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_memcmove: destsz > RSIZE_MAX",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
    }
    else if (count > RSIZE_MAX)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_memcmove: count > RSIZE_MAX",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
    }
    else if (count > destsz)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_memcmove: count > destsz",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
    }
    else if (dest == src)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_memcmove: dest and src are the same!",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    if (error != 0)
    {
        // on error when dest is not NULL and destsz is valid, then this memset
        // is done
        if (destsz < RSIZE_MAX)
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
                size_t      counter = count;
                const void* found   = memchr(src, c, count);
                if (found != M_NULLPTR)
                {
                    counter = M_REINTERPRET_CAST(uintptr_t, found) - M_REINTERPRET_CAST(uintptr_t, src) + SIZE_T_C(1);
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
