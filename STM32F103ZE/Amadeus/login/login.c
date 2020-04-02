#include "stm32f10x.h"
#include "vga.h"
#include "login.h"
#include "delay.h"
#include "stdio.h"
#include "math.h"
#include "graphical.h"
#include "stdlib.h"
#include "keyboard.h"
#include "components.h"

/*观测者图像数组引用*/
extern uint8_t fg_map[3040];
/*系统信息数组引用*/
extern uint8_t mes[10][60];
/*显示缓存引用*/
extern uint8_t fb[VID_VSIZE][VID_HSIZE+2];	
/*牧濑红莉栖图像数组引用*/
extern uint8_t bmp_makise[32768];
/*御坂美琴图像数组引用*/
extern uint8_t bmp_misaka[2048];
/*齿轮图像数组引用*/
extern uint8_t bmp_gear[2048];
/*Amadeus图像数组引用*/
extern uint8_t bmp_amdeus[16384];
/*配置信息引用*/
extern uint8_t cfg_mes[30][70];

/**
  * @brief    get_chipID.
  * @note     获取芯片ID.
  * @param    none.
  * @retval   none.
  * @note     96位的ID是stm32唯一身份标识，可以以8bit、16bit、32bit读取
  *            提供了大端和小端两种表示方法.
  */
void get_chipID(u32* id)
{
  id[0] = *(__IO u32 *)(0X1FFFF7E8);  // 低字节
  id[1] = *(__IO u32 *)(0X1FFFF7EC); // 
  id[2] = *(__IO u32 *)(0X1FFFF7F0); // 高字节
}
/**
  * @brief  系统硬件信息函数.
  * @note   内容见bitmap；
  *
  * @retval None
  */
void Sys_inf(void)
{
	uint8_t   line;
	uint8_t   str[50];
	uint32_t  id[3];
  /*观测者图像绘制*/
	ga_bitblt(NULL,424,0,304,80,fg_map,GDI_ROP_COPY);
	/*display information of system and mcu*/
	for(line = 0;line < 10;line++)
	{
		ga_draw_text(0,line*15+5,(u8*)mes[line], GDI_ROP_COPY);
		delay_ms(100);
	}
	get_chipID(id);
	sprintf((char*)str,"MCUID : %8X%8X%8X (96bit)",id[0],id[1],id[2]);
	ga_draw_text(0,line*15+5,(u8*)str, GDI_ROP_COPY);
	ga_draw_line(0,174,725,174, GDI_ROP_COPY);
	ga_draw_line(0,176,725,176, GDI_ROP_COPY);
}

/**
  * @brief  系统配置信息函数.
  * @note   内容见bitmap；
  *
  * @retval None
  */
void Sys_load(void)
{
	
	uint16_t r,line;

	/*清除屏幕*/
	ga_clear_screen();
	delay_ms(1000);
	delay_ms(1000);
	/*显示配置信息*/
	for(line = 0;line < 30;line++)
	{
		ga_draw_text(0,line*30+5,(u8*)cfg_mes[line],GDI_ROP_COPY);
		delay_ms(10+line);
	}
	/*清除屏幕*/
	ga_clear_screen();
	delay_ms(1000);
	/*绘制静态三角飘带*/
	ga_draw_ribbon(15,10,70,GDI_ROP_COPY);
	ga_draw_ribbon(16,510,70,GDI_ROP_COPY);
	/*绘制御坂美琴头像*/
  ga_bitblt(NULL,305,226,128,128,bmp_misaka,GDI_ROP_COPY);
	/*绘制进度环*/
	ga_draw_ring(370,290,80,13,2);
	/*清除屏幕*/
	ga_clear_screen();
	/*绘制牧濑红莉栖图片*/
	ga_bitblt(NULL,112,40,512,512,bmp_makise,GDI_ROP_COPY);
	
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	for(r = 0;r < 600;r++)
	{
		ga_draw_circle(368,296,r,GDI_ROP_DEL);
		ga_draw_circle(367,296,r,GDI_ROP_DEL);
	}
};
/**
  * @brief  syslogin.
  * @note   system login function.
  *
  * @retval None
  */
void Sys_login(void)
{
	uint16_t th;//齿轮旋转角度th
	
	uint8_t pswd_str[16] = "amadeus";//密码内容设置
	
	_Bool lock = SET;//锁定状态标志位
	edit_boxes id;//账户编辑框控件结构体类型定义
	edit_boxes pw;//密码编辑框控件结构体类型定义
	
	/*-----------------------账户编辑框-----------------------------*/
	/*账户编辑框配置*/
	id.rt.x = 340;      //设置左上顶点坐标
	id.rt.y = 395;  
	id.rt.h = 15;       //设置高和宽
	id.rt.w = 120;
	
	id.ml = 0;          //单行显示
	id.mod = 0;         //常规显示模式
	id.len_max = 16;    //设置最大输入长度
	
	/*账户编辑框初始化*/
	editBox_init(&id);  //初始化账户编辑框
	editBox_active(&id);//账户编辑框获得输入激活
	strtoarr((uint8_t *)"K.makise",id.key_str);//规定用户账户
	id.len_now = 8;
	
	/*显示账户编辑框内容*/
	editBox_loop(&id);
	
	/*-----------------------密码编辑框-----------------------------*/
	/*密码编辑框配置*/
	pw.rt.x = 340;      //设置左上顶点坐标
	pw.rt.y = 415;  
	pw.rt.h = 15;       //设置高和宽
	pw.rt.w = 120;
	
	pw.mod = '*';       //密码加密显示模式
	pw.len_max = 16;    //设置最大输入长度
	
	/*密码编辑框初始化*/
	editBox_init(&pw);  //初始化账户编辑框
	editBox_active(&pw);//密码编辑框获得输入激活
	
	
	/*-----------------------图片框和文本框-------------------------*/
	//绘制bmp_amdeus
	ga_bitblt(NULL,100,100,512,256,bmp_amdeus,GDI_ROP_COPY);
	//打印文本标签
	ga_draw_text(270,400,(uint8_t*)"USER_ID  :",GDI_ROP_COPY);
	ga_draw_text(270,420,(uint8_t*)"PASSWORD :",GDI_ROP_COPY);
	ga_draw_text(0,590,(uint8_t*)"Copyright ZhangDong 2018",GDI_ROP_COPY);
	ga_draw_text(610,590,(uint8_t *)"Amadeus System v2.76",GDI_ROP_COPY);
	/*-----------------验证密码,刷新输入与齿轮旋转------------------*/
	while(lock)
	{
		/*密码编辑框刷新显示*/
		editBox_loop(&pw);
		/*密码内容验证*/
		passward(&lock,pswd_str,&pw);
		/*齿轮动态旋转效果*/
		th+=5;if(th > 625)th = 0;
		ga_draw_bmp_rotate(460,140,128,128,th,bmp_gear,GDI_ROP_COPY);
		ga_draw_bmp_rotate(544,224,128,128,625-th,bmp_gear,GDI_ROP_COPY);
	}
	for(th = 730;th >0;th--)
	{
		ga_draw_line(th,0,th,600,GDI_ROP_DEL);
	}
}
/**
  * @brief  password.
  * @note   login password function.
  * @parma
  * @retval None
  */
void passward(_Bool * lock,uint8_t * pswd_str,edit_boxes * pw)
{
	uint8_t num;
	/*输入完成*/
	if(getenter(pw,0))
	{
		/*验证密码*/
		if(str_compare(pw->key_str,pswd_str))
		{
			/*状态解锁*/
			*lock = RESET;
		}
		else
		{
			/*密码错误*/
			ga_draw_text(325,440,(uint8_t*)"Your password is incorrect.",GDI_ROP_DEL);
			ga_draw_text(325,450,(uint8_t*)"Please re-enter your password. ",GDI_ROP_DEL);
			delay_ms(500);
			ga_draw_text(325,440,(uint8_t*)"Your password is incorrect.",GDI_ROP_COPY);
			ga_draw_text(325,450,(uint8_t*)"Please re-enter your password. ",GDI_ROP_COPY);
		}
		key_clr();
	}
}


