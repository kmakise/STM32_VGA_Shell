#include "usart.h"	 
#include "sys.h"
#include "esp8266.h"
#include "delay.h"
#include "vga.h"
/***********************************************************************************************************/
//																		ESP8266的初始化配置
/***********************************************************************************************************/		
void ESP_USART2_SendString(uint8_t * Send)  																	//向ESP（USART3）下达指令
{
    while (*Send)                   																					//检测字符串结束标志
    {
			  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);  							//等待发送结束
        USART_SendData(USART2,*Send++);                        								//向ESP发送数据
    }
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);  									//等待发送结束

}

void ESP_SevericeInit(void)
{
	ESP_USART2_SendString((uint8_t*)"AT+CWMODE=2\r\n");
	delay_ms(300);
	ESP_USART2_SendString((uint8_t*)"AT+RST\r\n");
	delay_ms(800);
	ESP_USART2_SendString((uint8_t*)"AT+CWSAP=\"Amadeus\",\"k.makise\",11,3\r\n");
	delay_ms(300);
	ESP_USART2_SendString((uint8_t*)"AT+CIPMUX=1\r\n");
	delay_ms(300);
	ESP_USART2_SendString((uint8_t*)"AT+CIPSERVER=1,8080\r\n");
	delay_ms(300);
	USART_ITConfig(USART2,USART_IT_RXNE, ENABLE);
}





