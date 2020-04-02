#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include <stm32f10x.h>


void USART1_Initialise(u32 bound);
void USART2_Initialise(u32 bound);
void USART3_Initialise(u32 bound);
void USART4_Initialise(u32 bound);
void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
void USART3_IRQHandler(void);
void UART4_IRQHandler(void);
#endif
