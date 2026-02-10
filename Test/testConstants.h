#ifndef TEST_CONSTANTS_H
#define TEST_CONSTANTS_H

#include <stdint.h>
#include <limits.h>
#include <float.h>

// int
#define INT_MIN_VAL        INT_MIN
#define INT_MAX_VAL        INT_MAX
#define INT_MIN_PLUS1      (INT_MIN + 1)
#define INT_MAX_MINUS1     (INT_MAX - 1)
#define INT_MID1           (INT_MIN / 2)
#define INT_MID2           (INT_MAX / 2)

// int8 
#define INT8_MIN_VAL       INT8_MIN
#define INT8_MAX_VAL       INT8_MAX
#define INT8_MIN_PLUS1     (INT8_MIN + 1)
#define INT8_MAX_MINUS1    (INT8_MAX - 1)
#define INT8_MID1          (INT8_MIN / 2)
#define INT8_MID2          (INT8_MAX / 2)

// int16
#define INT16_MIN_VAL      INT16_MIN
#define INT16_MAX_VAL      INT16_MAX
#define INT16_MIN_PLUS1    (INT16_MIN + 1)
#define INT16_MAX_MINUS1   (INT16_MAX - 1)
#define INT16_MID1         (INT16_MIN / 2)
#define INT16_MID2         (INT16_MAX / 2)

// int32
#define INT32_MIN_VAL      INT32_MIN
#define INT32_MAX_VAL      INT32_MAX
#define INT32_MIN_PLUS1    (INT32_MIN + 1)
#define INT32_MAX_MINUS1   (INT32_MAX - 1)
#define INT32_MID1         (INT32_MIN / 2)
#define INT32_MID2         (INT32_MAX / 2)

// int64
#define INT64_MIN_VAL      INT64_MIN
#define INT64_MAX_VAL      INT64_MAX
#define INT64_MIN_PLUS1    (INT64_MIN + 1)
#define INT64_MAX_MINUS1   (INT64_MAX - 1)
#define INT64_MID1         (INT64_MIN / 2)
#define INT64_MID2         (INT64_MAX / 2)

// uint 
#define UINT_MIN_VAL       0U
#define UINT_MAX_VAL       UINT_MAX
#define UINT_MIN_PLUS1     (0U + 1U)
#define UINT_MAX_MINUS1    (UINT_MAX - 1U)
#define UINT_MID1          (UINT_MAX / 2U)
#define UINT_MID2          (UINT_MAX / 2U + 1U)

// uint8 
#define UINT8_MIN_VAL      ((uint8_t)0)
#define UINT8_MAX_VAL      UINT8_MAX
#define UINT8_MIN_PLUS1    ((uint8_t)1)
#define UINT8_MAX_MINUS1   (UINT8_MAX - 1)
#define UINT8_MID1         (UINT8_MAX / 2)
#define UINT8_MID2         (UINT8_MAX / 2 + 1)

// uint16 
#define UINT16_MIN_VAL     ((uint16_t)0)
#define UINT16_MAX_VAL     UINT16_MAX
#define UINT16_MIN_PLUS1   ((uint16_t)1)
#define UINT16_MAX_MINUS1  (UINT16_MAX - 1)
#define UINT16_MID1        (UINT16_MAX / 2)
#define UINT16_MID2        (UINT16_MAX / 2 + 1)

// uint32 
#define UINT32_MIN_VAL     ((uint32_t)0)
#define UINT32_MAX_VAL     UINT32_MAX
#define UINT32_MIN_PLUS1   ((uint32_t)1)
#define UINT32_MAX_MINUS1  (UINT32_MAX - 1)
#define UINT32_MID1        (UINT32_MAX / 2)
#define UINT32_MID2        (UINT32_MAX / 2 + 1)

// uint64 
#define UINT64_MIN_VAL     ((uint64_t)0)
#define UINT64_MAX_VAL     UINT64_MAX
#define UINT64_MIN_PLUS1   ((uint64_t)1)
#define UINT64_MAX_MINUS1  (UINT64_MAX - 1)
#define UINT64_MID1        (UINT64_MAX / 2)
#define UINT64_MID2        (UINT64_MAX / 2 + 1)
#define UINT64_HIGH_BIT    ((uint64_t)1 << 63)

// uintptr 
#define UINTPTR_MIN_VAL      ((uintptr_t)0)
#define UINTPTR_MAX_VAL      UINTPTR_MAX
#define UINTPTR_MIN_PLUS1    ((uintptr_t)1)
#define UINTPTR_MAX_MINUS1   (UINTPTR_MAX - 1)
#define UINTPTR_MID1         (UINTPTR_MAX / 2)
#define UINTPTR_MID2         (UINTPTR_MAX / 2 + 1)

// float32 
#define FLOAT32_MIN_VAL     (-FLT_MAX)
#define FLOAT32_MAX_VAL     (FLT_MAX)
#define FLOAT32_MIN_PLUS1   (-FLT_MAX + 1.0f)
#define FLOAT32_MAX_MINUS1  (FLT_MAX - 1.0f)
#define FLOAT32_MID1        (0.0f)
#define FLOAT32_MID2        (FLT_MAX / 2.0f)

// float64 
#define FLOAT64_MIN_VAL     (-DBL_MAX)
#define FLOAT64_MAX_VAL     (DBL_MAX)
#define FLOAT64_MIN_PLUS1   (-DBL_MAX + 1.0)
#define FLOAT64_MAX_MINUS1  (DBL_MAX - 1.0)
#define FLOAT64_MID1        (0.0)
#define FLOAT64_MID2        (DBL_MAX / 2.0)

// custom values 
#define SERIAL_NUM      0x1234567890ABCDEFULL
#define HIGHER4BYTE     0x12345678U
#define LOWER4BYTE      0x90ABCDEFU
#define LOWER2BYTE      0xCDEFU
#define HIGHER2BYTE     0x1234U
#define MSB             0x12
#define BYTE6           0x34
#define BYTE5           0x56
#define BYTE4           0x78    
#define BYTE3           0x90
#define BYTE2           0xAB
#define BYTE1           0xCD
#define LSB             0xEF
#define NIBBLE0         0xF
#define NIBBLE1         0xE
#define MIN_VALUE       (-1)

#endif // TEST_CONSTANTS_H 