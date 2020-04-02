#include "stm32f10x.h"
#include "beep.h"
#include "delay.h"
/**
  * @brief   系统Speaker_Beep初始化函数.
  * @param   void.
  * @retval  None.
  */
void beep_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOG,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOG,GPIO_Pin_6);
}

/**
  * @brief   系统Speaker_Beep.
  * @param   void.
  * @retval  None.
  */
void beep_speak(uint16_t ms,uint8_t times)
{
	while(times)
	{
		times--;
		GPIO_SetBits(GPIOG,GPIO_Pin_6);
		delay_ms(ms);
		GPIO_ResetBits(GPIOG,GPIO_Pin_6);
		delay_ms(ms);
	}
}


