/**
 * @file
 * various utility macros
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef LWIP_HDR_DEF_H
#define LWIP_HDR_DEF_H

/* arch.h might define NULL already */
#include "lwip/arch.h"
#include "lwip/opt.h"
#if LWIP_PERF
#include "arch/perf.h"
#else /* LWIP_PERF */
#define PERF_START    /* null definition */
#define PERF_STOP(x)  /* null definition */
#endif /* LWIP_PERF */

LWIP_EXTERN_C_BEGIN

#define LWIP_MAX(x , y)  (((x) > (y)) ? (x) : (y))
#define LWIP_MIN(x , y)  (((x) < (y)) ? (x) : (y))

/* Get the number of entries in an array ('x' must NOT be a pointer!) */
#define LWIP_ARRAYSIZE(x) (sizeof(x)/sizeof((x)[0]))

/* Endianess-optimized shifting of two u8_t to create one u16_t */
#if LWIP_BYTE_ORDER == LWIP_LITTLE_ENDIAN
#define LWIP_MAKE_U16(a, b) ((u16_t)(((u16_t)(u8_t)(a) << 8) | (u8_t)(b)))
#else
#define LWIP_MAKE_U16(a, b) ((u16_t)(((u16_t)(u8_t)(b) << 8) | (u8_t)(a)))
#endif

#ifndef LWIP_PLATFORM_BYTESWAP
#define LWIP_PLATFORM_BYTESWAP 0
#endif

/* These swap bytes, regardless of the byte order. */
#if LWIP_PLATFORM_BYTESWAP
#define lwip_bswap16(x) ((u16_t)LWIP_PLATFORM_BSWAP16((u16_t)(x)))
#define lwip_bswap32(x) ((u32_t)LWIP_PLATFORM_BSWAP32((u32_t)(x)))
#else
u16_t lwip_bswap16(u16_t n);
u32_t lwip_bswap32(u32_t n);
#endif

#if LWIP_BYTE_ORDER == LWIP_BIG_ENDIAN

#define lwip_htons(x) ((u16_t)(x))
#define lwip_ntohs(x) ((u16_t)(x))
#define lwip_htonl(x) ((u32_t)(x))
#define lwip_ntohl(x) ((u32_t)(x))
#define PP_HTONS(x) ((u16_t)(x))
#define PP_NTOHS(x) ((u16_t)(x))
#define PP_HTONL(x) ((u32_t)(x))
#define PP_NTOHL(x) ((u32_t)(x))

#else

#define lwip_htons(x) lwip_bswap16(x)
#define lwip_ntohs(x) lwip_bswap16(x)
#define lwip_htonl(x) lwip_bswap32(x)
#define lwip_ntohl(x) lwip_bswap32(x)

/* These macros should be calculated by the preprocessor and are used
   with compile-time constants only (so that there is no little-endian
   overhead at runtime). */
#define PP_HTONS(x) ((u16_t)((((u16_t)(x) & 0xff) << 8) | (((u16_t)(x) & 0xff00) >> 8)))
#define PP_NTOHS(x) PP_HTONS(x)
#define PP_HTONL(x) ((u32_t)((((u32_t)(x) & 0xff) << 24) | \
                             (((u32_t)(x) & 0xff00) << 8) | \
                             (((u32_t)(x) & 0xff0000UL) >> 8) | \
                             (((u32_t)(x) & 0xff000000UL) >> 24)))
#define PP_NTOHL(x) PP_HTONL(x)

#endif

LWIP_EXTERN_C_END

#endif

