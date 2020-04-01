#ifndef __STR_DEALING
#define __STR_DEALING

#include "stm32f10x.h"

typedef	uint8_t             *pu8;

uint16_t strLen(pu8 str);
_Bool str_compare(uint8_t * str1,uint8_t * str2);
void strtoarr(uint8_t * str,uint8_t * arr);

#endif  /* __STR_DEALING */

