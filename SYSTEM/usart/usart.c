#include <usart.h>	
#include <sys.h>
#include <cmnct.h>
#include <keyboard.h>

#pragma import(__use_no_semihosting)             
																																										//标准库需要的支持函数                 
struct __FILE { 
	int handle; 
}; 

FILE __stdout;       
																																										//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) { 
	x = x; 
} 
																																										//重定义fputc函数 
int fputc(int ch, FILE *f){      
	while((USART1->SR&0X40)==0)																												//循环发送,直到发送完毕 
		
		if(ch==0x00)ch=0x2E;
    USART1->DR = (u8) ch;      
	return ch;
}

/***********************************************************************************************************/
//																					USART1,2,3 初始化
/***********************************************************************************************************/
            																									//接收状态标记
void USART1_Initialise(u32 bound){
																																										//GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	
																																										//使能USART1，GPIOA时钟
  
																																										//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;              														//PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	        													//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);                  													//初始化GPIOA.9
   
																																										//USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;              													//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   													//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);                  													//初始化GPIOA.10  

																																										//Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;														//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  													//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			   														//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);                        														//根据指定的参数初始化NVIC寄存器
  
																																										//USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;             													//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;												//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;  													//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;    														//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
																																										//收发模式

  USART_Init(USART1, &USART_InitStructure);              														//初始化串口1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);          													//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                              													//使能串口1 

}

void USART2_Initialise( u32 bound ){
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
																																										/* Enable the USART2 Pins Software Remapping */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB2Periph_AFIO, ENABLE); 
    
																																										/* Configure USART2 Rx (PA.03) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
																																										/* Configure USART2 Tx (PA.02) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
																																										/* Enable the USART2 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);    
    
    USART_InitStructure.USART_BaudRate = bound;                
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;     
    USART_InitStructure.USART_Parity = USART_Parity_No;        
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;      
    
    USART_Init(USART2, &USART_InitStructure);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
																																										/* Enable USART2 */
    USART_Cmd(USART2, ENABLE);
		
}

void USART3_Initialise(u32 bound)    
    
{    
   USART_InitTypeDef USART_InitStructure;    
   NVIC_InitTypeDef  NVIC_InitStructure;     
   GPIO_InitTypeDef  GPIO_InitStructure;    																				//声明一个结构体变量，用来初始化GPIO   
	
																																										//使能串口的RCC时钟    
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE); 													//使能UART3所在GPIOB的时钟    
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);    
    
																																										// Configure USART2 Rx (PB.11) as input floating      
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;    
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    
   GPIO_Init(GPIOB, &GPIO_InitStructure);    
    
																																										// Configure USART2 Tx (PB.10) as alternate function push-pull    
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;    
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    
   GPIO_Init(GPIOB, &GPIO_InitStructure);    
    
																																										//配置串口    
   USART_InitStructure.USART_BaudRate = bound;    
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;    
   USART_InitStructure.USART_StopBits = USART_StopBits_1;    
   USART_InitStructure.USART_Parity = USART_Parity_No;    
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;    
   USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;       
   USART_Init(USART3, &USART_InitStructure);      
   USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);          
   USART_Cmd(USART3, ENABLE);   
     
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);         
}   

void USART4_Initialise(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;  
	NVIC_InitTypeDef  NVIC_InitStructure; 
	USART_InitTypeDef USART_InitStructure;  
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);    
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE); 															//注意UART4是挂载在APB1总线上的，用RCC_APB1PeriphClockCmd()函数初始化！  

	//3?ê??ˉUART4-TX-PC10  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOC, &GPIO_InitStructure);  
	//UART-RX-PC11  
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
	GPIO_Init(GPIOC, &GPIO_InitStructure); 
	
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);  
	

	USART_InitStructure.USART_BaudRate = bound;  
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  
	USART_InitStructure.USART_StopBits = USART_StopBits_1;  
	USART_InitStructure.USART_Parity = USART_Parity_No ;  
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  

	USART_Init(UART4, &USART_InitStructure);   
	USART_Cmd(UART4, ENABLE);     
	USART_ClearFlag(UART4,USART_FLAG_TC);  
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);  
}
/***********************************************************************************************************/
//																			USART1,2,3 中断接收部分
/***********************************************************************************************************/

void USART1_IRQHandler(void)                																				//串口1中断服务程序 PC通讯
{
	if(USART1->SR&(1<<5))
	{	 
		usart1_receive(USART_ReceiveData(USART1));
	}
} 

void USART2_IRQHandler(void)                						 														//串口2中断服务程序 PWM控制器通讯
{
  //USART2_NewIstr();	
	if(USART2->SR&(1<<5))
	{	 
			wifi_receive(USART_ReceiveData(USART2));
	}
																			
}
void USART3_IRQHandler(void)                						 														//串口3中断服务程序 ESP通讯
{
	//USART3_NewIstr();
	if(USART3->SR&(1<<5))
	{	 
		key_get(USART_ReceiveData(USART3));
	}
}

#include "mouse.h"
void UART4_IRQHandler(void)
{
	if(UART4->SR&(1<<5))
	{	 
		uartMesRx(USART_ReceiveData(UART4));
	}
}



