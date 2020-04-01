#ifndef __LIBMASTER_H
#define __LIBMASTER_H
#include "stm32f10x.h"

typedef struct
{
	uint8_t (* run)(void * data);		//方法指针
	uint8_t   name[10];							//方法名
	uint8_t   id;										//方法id
	
}ApplibTypedef;


//指令执行器
uint8_t libMaster(uint8_t * cmd);

#endif /*__LIBMASTER_H*/

