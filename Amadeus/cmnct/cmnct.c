#include "cmnct.h"
#include "vga.h"
#include "graphical.h"

/**
  * @brief    usart1_receive.
  * @note     接收串口1发送的消息.
  * @param    res 接收消息的单字节.
  * @retval   none.
  */
void usart1_receive(uint8_t res)
{
	
}

/**
  * @brief    wifi_receive.
  * @note     接收WIFI模块串口发送的消息.
  * @param    res 接收消息的单字节.
  * @retval   none.
  */
void wifi_receive(uint8_t res)
{
	
	wifi_mes(res);
}
/**
  * @brief    wifi_mes.
  * @note     初始化信息的显示.
  * @param    res 接收消息的单字节.
  * @retval   none.
  */
void wifi_mes(uint8_t res)
{
	static uint8_t x,y;
	if(res == 0x0d)
	{
		y++;
		x = 0;
	}
	else
	{
		if((x*6) > 580)return;
		if((y*10) > 700)return;
		ga_draw_text((x++)*6,y*10+180,&res, GDI_ROP_COPY);
	}
}
