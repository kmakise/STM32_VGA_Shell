#include <components.h>
#include <graphical.h>
#include <keyboard.h>

/*amadeus system 交互控件支持库*/

/*-----------------------------编辑框类----------------------------*/
//编辑框 参数结构体类名 edit_boxes

/**
  * @brief  Edit box initialization.
  * @note   编辑框初始化函数.
  * @parma  
  * @retval None
  */
void editBox_init(edit_boxes * eb)
{
	/*绘制编辑框外框*/
	ga_draw_rec(
			eb->rt.x,
    	eb->rt.y,
	    eb->rt.x + eb->rt.w,
	    eb->rt.y + eb->rt.h,
			GDI_ROP_COPY);
}
/**
  * @brief  Edit box active.
  * @note   编辑框激活函数.
  * @parma  
  * @retval None
  */
void editBox_active(edit_boxes * eb)
{
	/*输入允许标志位激活*/
	eb->key_cpt = RESET;
	/*将当前激活的编辑框地址传递给键盘抽象层*/
	key_eb = eb;
}
/**
  * @brief  Edit box loop.
  * @note   编辑框刷新显示基础函数.
  * @parma  
  * @retval None
  */
void editBox_loop(edit_boxes * eb)
{
	uint8_t num;
	uint8_t str[100];
	if(!eb->ml)
	{
		/*未输入部分空格补充*/
		for(num = 0;num < eb->len_max;num++)
		{
			if(num < (key_eb->len_now))
			{
				if(eb->mod)
				{
					str[num] = eb ->mod;
				}
				else
				{
					str[num] = key_eb->key_str[num];
				}
			}
			else if(num >= (key_eb->len_now))
			{
				str[num] = ' ';
			}
		}
		for(num = 0;num < 100-eb->len_max;num++)
		{
			str[num+eb->len_max] = 0;
		}
		/*刷新编辑框内显示的内容*/
		ga_draw_text(eb->rt.x+5,eb->rt.y+5,(uint8_t*)str,GDI_ROP_COPY);
	}
}
/**
  * @brief  getenter.
  * @note   获得enter事件并返回和相关标志位的清除.
  * @parma  pw  指向相应编辑框结构体
  *         clr 是否清除输入
  * @retval None
  */
_Bool getenter(edit_boxes * eb,_Bool clr)
{
	if(eb->key_cpt)
	{
		if(clr)
		{
			key_clr();
		}
		eb->key_cpt = RESET;
		return 1;
	}
	return 0;
}



