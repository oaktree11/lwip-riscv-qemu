//This is a basic header that describes your compiler and processor to lwIP

#ifndef __ARCH_CC_H__
#define __ARCH_CC_H__

#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>

#include "reip.h"

//#define BYTE_ORDER LITTLE_ENDIAN

typedef uint8_t     u8_t;
typedef int8_t      s8_t;
typedef uint16_t    u16_t;
typedef int16_t     s16_t;
typedef uint32_t    u32_t;
typedef int32_t     s32_t;
typedef uintptr_t   mem_ptr_t;

#define LWIP_ERR_T  int

/* Define (sn)printf formatters for these lwIP types */
#define U16_F "hu"
#define S16_F "hd"
#define X16_F "hx"
#define U32_F "u"
#define S32_F "d"
#define X32_F "x"

/* repulsion: htons/htonl and byteswap */
#define LWIP_PLATFORM_BYTESWAP 1
#define LWIP_PLATFORM_HTONS(x) ( (((u16_t)(x))>>8) | (((x)&0xFF)<<8) )
#define LWIP_PLATFORM_HTONL(x) ( (((u32_t)(x))>>24) | (((x)&0xFF0000)>>8) | (((x)&0xFF00)<<8) | (((x)&0xFF)<<24) )

/* Compiler hints for packing structures */
#define PACK_STRUCT_FIELD(x)    x
#define PACK_STRUCT_STRUCT  __attribute__((packed))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END

#define reip_abort()

/* Plaform specific diagnostic output */
#define LWIP_PLATFORM_DIAG(x)   do {                \
        printf x;                   \
    } while (0)

#define LWIP_PLATFORM_ASSERT(x) do {                \
        printf("Assert \"%s\" failed at line %d in %s\n",   \
                x, __LINE__, __FILE__);             \
        reip_abort();                        \
    } while (0)

#endif /* __ARCH_CC_H__ */
