#ifndef __PUBLIC_UTIL_H__
#define __PUBLIC_UTIL_H__

#include "string.h"

#ifndef TRUE
#define TRUE (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

#ifndef boolean_t
typedef unsigned char boolean_t;
#endif

#ifndef int8_t
typedef signed char int8_t;
#endif

#ifndef int16_t
typedef short int16_t;
#endif

#ifndef int32_t
typedef int int32_t;
#endif // !int32_t

#ifndef uchar_t
typedef unsigned char uchar_t;
#endif // !uchar_t

#ifndef uint8_t
typedef unsigned char uint8_t;
#endif // !uint8_t

#ifndef uint16_t
typedef unsigned short uint16_t;
#endif // !uint16_t

#ifndef uint32_t
typedef unsigned int uint32_t;
#endif // !uint32_t

#ifndef ushort
typedef unsigned short ushort;
#endif // !ushort

#ifndef ulong
typedef unsigned long ulong;
#endif // !ulong

#ifndef ulonglong
typedef unsigned long long ulonglong;
#endif // !ulonglong

#define filename(x) strrchr(x,'/')?strrchr(x,'/')+1:x

#endif

