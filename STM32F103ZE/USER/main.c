#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "stdio.h"
#include "led.h"
#include "vga.h"
#include "usart.h"
#include "beep.h"
#include "login.h"
#include "math.h"
#include "esp8266.h"
#include "graphical.h"
#include "terminal.h"

#define DEBUGDEF		//调试模式直接进入终端


/*Amadeus_sys_pll 8 x 16 = 128Mhz*/
void Amadeus_loop(void);
void Amadeus_setup(void);

int main(void)
{	
	Amadeus_setup();
	while(1)
	{
		Amadeus_loop();
	}
}

void Amadeus_setup(void)
{
	/*初始化阶段*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	
	delay_init();
	
	beep_init();
	
	USART1_Initialise(115200);
	USART2_Initialise(115200);
	USART3_Initialise(115200);
	USART4_Initialise(115200);
	
	vga_device_init();
	
//程序调试模式 调过自检和其他直接进入加载终端
#ifndef DEBUGDEF

	//termainal_load();
  //termainal_init();
	interactive_editbox();
  
	beep_speak(1500,1);
	
	Sys_inf();
	


	/*等待显示器适配*/
	delay_ms(1000);
	delay_ms(1000);
	
  ESP_SevericeInit();
	
	/*启动阶段*/
	Sys_load();
	
	led_init();
	
	beep_speak(100,2);
	



	Sys_login();
	
#endif
	
	/*终端阶段*/
	
	termainal_load();
	
	termainal_init();
}

void Amadeus_loop(void)
{
	interactive_editbox();
}











