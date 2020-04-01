/**
  ******************************************************************************
  * @file    tmnio.c
  * @author  张东
  * @version V1.0.0			
  * @date    2019-10-2 	工程变更C89标准为C99 
  * @brief   终端窗口的输入输出实现方法集合
  ******************************************************************************
  */
	
/*include --------------------------------------------------------------------*/	
#include "tmnio.h"
#include "components.h"
#include "str_dealing.h"
#include "graphical.h"
#include "keyboard.h"
#include "libMaster.h"

#include "stdio.h"
/*Golbal Data Space ----------------------------------------------------------*/	
edit_boxes 				g_tmlobj;    		//终端交互编辑框
TmlStateTypedef		g_TmlState = Application;//终端状态机

uint8_t g_tmlbuf[24][70];		//终端交互文本缓冲区
uint8_t *g_tmlbufpnt[24];		//当前显示行指针
uint8_t g_lineKeyCur = 0;		//当前输入行焦点

uint8_t g_tmlcmd[50];				//终端指令

//终端交互显示刷新
void display_editbox(edit_boxes *eb,uint8_t *tl[24])
{
	uint8_t str[70];     //行缓冲区
	uint8_t num,line;    //暂存变量
	uint16_t x,y;        //xy绘制坐标
	
	for(line = 0;line < eb->nl;line++)
	{
		uint8_t clrflag = 0;
		
		/*行显示*/
		for(num = 0;num < eb->len_max;num++)
		{
			
			/*判断当前是否不为空字符*/
			if(tl[line][num] == 0x00)
			{
				clrflag = 1;
			}
			
			//由第一个结束符号开始后面填充空格
			if(clrflag == 0)
			{
				str[num] = tl[line][num];
			}
			else
			{
				str[num] = ' ';
			}
		}
		/*刷新当前行*/
		ga_draw_text(eb->rt.x+5,eb->rt.y+5+line*10,(uint8_t*)str,GDI_ROP_COPY);
	}
}
void tmnUpdate(void)
{
	display_editbox(&g_tmlobj,g_tmlbufpnt);
}


//移动全屏光标
void moveLineCur(void)
{
	static uint8_t fullflag = 0; //滚动标志位
	
	if(fullflag == 0) //当前状态未使能滚动显示
	{
		//使能滚动显示
		g_lineKeyCur++;
		if(g_lineKeyCur == 24)
		{
			g_lineKeyCur = 0;
			fullflag = 1;
		}
	}
	else//当前状态使能滚动显示 
	{
		g_lineKeyCur = (g_lineKeyCur >= 23) ? 0 : g_lineKeyCur + 1;
		//移动消息显示行指针
		for(int i = 0;i < 24;i++)
		{
			if((i + g_lineKeyCur) <= 23)
			{
				g_tmlbufpnt[i] = g_tmlbuf[i + g_lineKeyCur];
			}
			else
			{
				g_tmlbufpnt[i] = g_tmlbuf[(i + g_lineKeyCur - 1) - 23];
			}
		}
	}
}

//将消息写入显示缓冲区的新一行
void DisMessageNewLine(uint8_t * str)
{
	uint8_t cur = 0;//光标
		
	//将消息写入显示缓冲区
	for(cur = 0;str[cur] != 0;cur++)
	{
		g_tmlbuf[g_lineKeyCur][cur] = str[cur];
	}
	//写入消息结尾
	g_tmlbuf[g_lineKeyCur][cur] = 0;
	
	//移动光标
	moveLineCur();

}

//系统显示输出 自动换行
void SysOutInfo(uint8_t * str)
{
	uint16_t num = 0;
	uint8_t flag = 0;
	uint16_t len = strLen(str);
	uint8_t strbuf[70];
	
	//消息长度超出限制 开始自动换行
	while(len > 65)
	{
		//复制内容
		for(int i = 0;i < 66;i++)
		{
			strbuf[i] = str[num + i];
		}
		strbuf[66] = 0;
		
		//显示新行
		DisMessageNewLine(strbuf);
		
		//更新下标与标志位
		len -= 66;
		num += 66;
		flag = 1;
	}
	
	//消息长度在限度内
	if(len != 0)
	{
		for(int i = 0;i < len;i++)
		{
			strbuf[i] = str[num + i];
		}
		strbuf[len] = 0;
		
		DisMessageNewLine(strbuf);
	}
}


extern KeyDataTyepdef g_keydata;	//键盘数据存储结构

//键盘输入显示
void keyboardInput(void)
{
	//g_tmlbufpnt[g_lineKeyCur][0] = 0
	
	static uint8_t USER[18] = "k.makise@Amadeus>:"; 
	static uint8_t strbuf[70] = "k.makise@Amadeus>:";
	static uint8_t len = 0;//输入长度
	
	static uint8_t firstflag = 0;
	
	//重新调用时使用一个新行作为输入显示
	if(firstflag == 0)
	{
		firstflag = 1;
		SysOutInfo(" \0");
		moveLineCur();
		g_lineKeyCur = (g_lineKeyCur > 0) ? g_lineKeyCur - 1:23;
	}

	//字符输入
	if(g_keydata.ch != 0)
	{
		//用户名显示 
		for(int i = 0;i < 18;i++)
		{
			strbuf[i] = USER[i];
		}
		//字符显示
		strbuf[18 + len] = g_keydata.ch;
		len = (len < 40) ? len + 1:len;
		
		g_keydata.ch = 0;
	}

	//输出显示
	for(int i = 0;i < 65;i++)
	{
		if(i < (len + 18))
		{
			g_tmlbuf[g_lineKeyCur][i] = strbuf[i];
		}
		else
		{
			g_tmlbuf[g_lineKeyCur][i] = 0;
			strbuf[i] = 0;
		}
	}
	

	
	//控制输入
	if(g_keydata.ct != 0)
	{
		switch(g_keydata.ct)
		{
			/*ctrl*/
			case 0x01: break;
			/*cap*/
			case 0x02: break;
			/*del*/
			case 0x03:
				if(len >= 1)
				{
					len--;
					strbuf[18 + len] = 0;
				}
				break;
			/*up*/
			case 0x04:break;
			/*down*/
			case 0x05:break;
			/*lift*/
			case 0x06:break;
			/*right*/
			case 0x07:break;
			/*enter*/
			case 0x0d:
			case 0x0A:
			{
				moveLineCur();//移动显示行焦点
			
				key_eb->key_cpt = SET;
				g_TmlState = Application;
			
				len = 0;
				firstflag = 0;
			
				uint8_t str[100];
				sprintf((char *)str,"Searching \"%s\"...",&strbuf[18]);
				SysOutInfo(str);
				
				for(int i = 0;i < 40;i++)
				{
					g_tmlcmd[i] = strbuf[18 + i];
				}
				
				break;
			}
		}

		g_keydata.ct = 0;
	}
}

void KeyBoardEnable(void)
{
	editBox_active(&g_tmlobj);
}


void Test(void)
{
	static uint16_t line = 0;
	
	uint8_t str[50];
	
	sprintf((char *)str,"TEST message No. %d.",line++);
	
	SysOutInfo(str);
	
	delay_ms (500);
}



/*main func ------------------------------------------------------------------*/
void tmnioSetup(void)
{
	/*终端交互编辑框配置*/
	g_tmlobj.rt.x = 3;      //设置左上顶点坐标
	g_tmlobj.rt.y = 343;  
	g_tmlobj.rt.h = 250;       //设置高和宽
	g_tmlobj.rt.w = 468;
	
	g_tmlobj.mod = 0;          //默认显示模式
	g_tmlobj.len_max = 70;     //设置最大输入长度
	g_tmlobj.ml = 1;           //多行显示
	g_tmlobj.nl = 24;           //行数20

	SysOutInfo("Hello! I'm Makise Kurisu. yoroshiku~ ");
	SysOutInfo("Please enter \"help\" to see the command list.");
	
	/*终端交互编辑框初始化*/
	editBox_init(&g_tmlobj);  //初始化账户编辑框
	editBox_active(&g_tmlobj);//终端交互编辑框获得输入激活
	
	//执行应用
	g_TmlState = WaitCommand;
	
	//清除按键缓存
	g_keydata.ch = 0;
	g_keydata.ct = 0;
	
	//指针数组赋值
	for(int i = 0;i < 24;i++)
	{
		g_tmlbufpnt[i] = g_tmlbuf[i];
	}
	
}

void tmnioLoop(void)
{
	
	switch(g_TmlState)
	{
		case Application:
		{
			//搜索已注册的应用程序并执行
			uint8_t state = libMaster(g_tmlcmd);
			
			//执行结果
			if(state == 0)//未搜索到方法或者指令
			{
				uint8_t str[50];
				sprintf((char *)str,"\"%s\" Not installed.",g_tmlcmd);
				SysOutInfo("Please check and re-enter.[unknown command or function]");
				SysOutInfo(str);
			}
			else
			{
				uint8_t str[50];
				sprintf((char *)str,"Program has ended Return Code %d.",state - 1);
				SysOutInfo(str);
			}
			

			g_TmlState = WaitCommand;
			break;
		}
		case WaitCommand:
		{
			KeyBoardEnable();
			keyboardInput();
			break;
		}
		default:SysOutInfo("terminal state error.");break;
	}
	
	
	//Test();
	tmnUpdate();
}


void tmnioInterrupt(void)
{
	
}




