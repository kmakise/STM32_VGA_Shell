#ifndef __COMPONENTS_H
#define __COMPONENTS_H	 	    					    
#include "stm32f10x.h"
/*amdeus system COMPONENTS*/

/*外形参数结构体类型*/
typedef struct
{

	uint16_t x;             /*左上顶点的x坐标*/
	uint16_t y;             /*左上顶点的y坐标*/
	uint16_t h;             /*矩形高*/
	uint16_t w;             /*矩形宽*/
	
}rectangle;

/*编辑框结构体类型*/
typedef struct
{

	rectangle rt;           /*边框参数设定*/
	uint8_t key_str[100];	  /*按键输入缓冲区*/
	_Bool   key_cpt;	      /*按键输入完成标志位（回车=完成）*/
	uint8_t len_max;	      /*限制输入长度*/
	uint8_t len_now;	      /*已输入长度*/
	
	_Bool   ml;             /*是否多行显示*/
	uint8_t mod;            /*显示方式 0：常规 *：密码显示*/
	uint8_t nl;             //当前行数
	
}edit_boxes;

void editBox_init(edit_boxes * eb);
void editBox_active(edit_boxes * eb);
void editBox_loop(edit_boxes * eb);
_Bool getenter(edit_boxes * eb,_Bool clr);

#endif



