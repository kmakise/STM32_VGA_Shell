/**
  ******************************************************************************
  * @file    Terminalmain.c
  * @author  张东
  * @version V1.0.0			从这个版本开始应用将与底层分离
  * @date    2019-10-2 	工程变更C89标准为C99 
  * @brief   中断组主程序 由于之前程序时期过早且逻辑混乱故不采用（由于代码量较多故不重写）
  ******************************************************************************
  */
#include "Terminalmain.h"
#include "tmnio.h"



void TerminalMainSetup(void)
{
	tmnioSetup();
}


void TerminalMainLoop(void)
{
	tmnioLoop();
}


void TerminalMainInterrupt(void)
{
	tmnioInterrupt();
}




