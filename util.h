#ifndef _UTIL_H
#define _UTIL_H

#ifndef __CC65__
#include <endian.h>
#if __BYTE_ORDER == __LITTLE_ENDIAN
#include <byteswap.h>
#define htons bswap_16
#define ntohs bswap_16
#else
#define htons
#define ntohs
#endif
#endif

#if __CC65__
extern uint16_t bswap_16(uint16_t swap);
#define htons bswap_16
#define ntohs bswap_16
#endif

#endif
