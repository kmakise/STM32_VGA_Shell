/**
  ******************************************************************************
  * @file    systool.c
  * @author  张东
  * @version V1.0.0			
  * @date    2019-10-2 	工程变更C89标准为C99 
  * @brief   系统基本工具
  ******************************************************************************
  */
/*include --------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "systool.h"
#include "graphical.h"
#include "tmnio.h"


/*tool ---------------------------------------------------*/

//屏幕清除
uint8_t ClearScreen(void * data)
{
	SysOutInfo((uint8_t *)"System Tools Clear Screen V1.0-191003");
	for(int i = 1;i < 730;i++)//横向扫描
	{
		ga_draw_line(i,16,i,339,GDI_ROP_DEL);
	}
	SysOutInfo((uint8_t *)"Clear screen complete without any error.");
	return 0;
}

//重新启动
uint8_t SysReboot(void * data)
{
	SysOutInfo((uint8_t *)"System Rebooting ...");
	NVIC_SystemReset();
	return 0;
}


uint8_t funcDemo(void * data)
{
	
	
	
}








