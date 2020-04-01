#ifndef __KEYBOARD_H
#define __KEYBOARD_H	 	    					    
#include "stm32f10x.h"
#include "components.h"
///*按键输入缓冲区*/
//extern uint8_t key_str[100];
///*按键输入完成标志位（回车=完成）*/
//extern _Bool key_cpt;
///*限制输入长度*/
//extern uint8_t len_max;
///*已输入长度*/
//extern uint8_t len_now;
typedef struct
{
	uint8_t ch;	//字符输入
	uint8_t ct;	//控制输入
	
}KeyDataTyepdef;


extern edit_boxes * key_eb;

uint8_t getCharkey(void);
uint8_t getCtrlkey(void);

void key_get(uint8_t key);
void key_clr(void);


#endif

