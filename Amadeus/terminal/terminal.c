#include "stm32f10x.h"
#include "terminal.h"
#include "components.h"
#include "graphical.h"
#include "delay.h"
#include "math.h"
#include "TerminalMain.h"

/*amadeus terminal*/
extern uint8_t bmp_kelisu[8192];
/**
  * @brief  termainal load.
  * @note   终端加载函数.
  * @parma  void.
  * @retval None
  */
void termainal_load(void)
{
	uint16_t th;
	//ga_draw_worldline();
	for(th = 0;th < 780;th++)
	{
		ga_draw_line(th,0,th,600,GDI_ROP_DEL);
	}
}

/**
  * @brief  termainal initialization function.
  * @note   终端初始化函数.
  * @parma  void.
  * @retval None
  */
void termainal_init(void)
{
	/*框体线条绘制布局*/
	ga_draw_rec(0,0,730,596,GDI_ROP_COPY);
	ga_draw_line(0,15,730,15,GDI_ROP_COPY);
	ga_draw_line(0,340,730,340,GDI_ROP_COPY);
	
	/*终端窗口标题*/
	ga_draw_text(580,5,(uint8_t*)"Copyright ZhangDong 2018",GDI_ROP_COPY);
	ga_draw_text(5,5,(uint8_t *)"Terminal - Amadeus System [v2.76.180829-191002]",GDI_ROP_COPY);
	
	/*牧濑红莉栖头像绘制*/
	ga_bitblt(NULL,474,340,256,256,bmp_kelisu,GDI_ROP_COPY);
}
/**
  * @brief  Terminal interactive edit box.
  * @note   终端交互窗口.
  * @parma  void.
  * @retval None
  */
void interactive_editbox(void)
{
//	/*sigmoid函数曲线显示*/
//	sigmoid();
//	/*正态分布柱状图显示*/
//	N_distributio();
	
	//底层初始化完成开始逻辑应用部分  从这里开始逻辑将于底层分离 将采用完全不同的程序风格
	TerminalMainSetup();
	while(1)
	{
		TerminalMainLoop();
	}
}





