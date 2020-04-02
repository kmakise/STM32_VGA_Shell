#include "stm32f10x.h"
#include "led.h"
/**
  * @brief   系统运行呼吸灯初始化函数.
  * @param   void.
  * @retval  None.
  */
void led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOE,GPIO_Pin_2);
}

/**
  * @brief   系统运行呼吸灯.
  * @param   void.
  * @retval  None.
  */
void led_running(void)
{
	static uint8_t ph,div;
	static _Bool flag;
	div++;
	GPIOE->BSRR = GPIO_Pin_2 << 16 * (ph<div);
	if(div==150)
	{
		div = RESET;
		ph += !flag;
		ph -= flag;
		flag += (ph==15);
		flag *= !(ph==0);
	}
}


