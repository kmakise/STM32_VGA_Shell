#ifndef __TMNIO_H
#define __TMNIO_H

#include "components.h"

typedef enum
{
	Application,
	WaitCommand,
	
	
}TmlStateTypedef;


//终端交互显示刷新
void tmnUpdate(void);
//键盘输入使能
void KeyBoardEnable(void);

//系统显示输出 自动换行
void SysOutInfo(uint8_t * str);

void tmnioSetup(void);
void tmnioLoop(void);	
void tmnioInterrupt(void);

#endif /*__TMNIO_H*/

