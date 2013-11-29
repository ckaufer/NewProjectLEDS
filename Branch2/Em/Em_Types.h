#ifndef Em_Types_H_
#define Em_Types_H_

#ifndef Em_NOSTDBOOL
#include <stdbool.h>
#endif

#ifndef Em_NOSTDINT
#include <stdint.h>
#endif

#ifdef Em_16BIT
typedef signed char     int8_t;
typedef unsigned char   uint8_t;
#endif

#endif /*Em_Types_H_*/
