#ifndef __GRAPHICAL_H
#define __GRAPHICAL_H	 	    					    
#include "stm32f10x.h"
#include "str_dealing.h"
#include "stdlib.h"


#define	VID_HSIZE		92  	// Horizontal resolution (in bytes) 100*8=800
#define	VID_VSIZE	  600		// Vertical resolution (in lines) 600*1=600

#define	VID_PIXELS_X	(VID_HSIZE * 8)
#define	VID_PIXELS_Y	VID_VSIZE
#define	VID_PIXELS_XR	(VID_PIXELS_X + 16)
#define	VID_HSIZE_R		(VID_HSIZE + 2)

// 系统字体
#define	GDI_SYSFONT_WIDTH		6		  //字体像素宽度
#define	GDI_SYSFONT_HEIGHT		9		//字体像素高度
#define	GDI_SYSFONT_BYTEWIDTH	1		// Width in bytes
#define	GDI_SYSFONT_OFFSET		0x20

//光栅操作
#define	GDI_ROP_COPY			0 //直接覆盖
#define	GDI_ROP_XOR				1 //做异或运算
#define	GDI_ROP_AND				2 //与运算
#define	GDI_ROP_OR				3 //或运算
#define	GDI_ROP_DEL				4 //删除

//格式操作
#define	GDI_WINCAPTION			0x0001
#define	GDI_WINBORDER			  0x0002
#define	GDI_WINCLOSEICON		0x0003

//文本对齐模式
#define	GDI_WINCAPTION_LEFT		0x0000
#define	GDI_WINCAPTION_CENTER	0x0010
#define	GDI_WINCAPTION_RIGHT	0x0020
#define	GDI_WINCAPTION_MASK		0x0030

typedef __packed struct
{
	int16_t	w;							// Width in bits
	int16_t	h;							// Height in bits
	int16_t	wb;							// width in bytes
	int16_t	wh;							// Height in bytes
	u8*   bm;						    // Pointer to bitmap bits

} GDI_BITMAP, PGDI_BITMAP;

typedef __packed  struct
{
	int16_t	x;							// X position
	int16_t	y;							// Y position
	int16_t	w;							// Width
	int16_t	h;							// Height
    
} GDI_RECT, *PGDI_RECT;

typedef __packed struct
{
	uint16_t style;				        // Mode, see GDI_WINxxx defines
	GDI_RECT rc;					    // Absolute rectangle
	u8*	caption;			            // Caption text

} GDI_WINDOW, *PGDI_WINDOW;

/*屏幕清除*/
void ga_clear_screen(void);
/*位图绘制*/
void ga_bitblt(PGDI_RECT prc, int16_t x, int16_t y, int16_t w, int16_t h,u8* bm, uint16_t rop);
/*字符串文本绘制*/
void ga_draw_text(int16_t x, int16_t y, u8* ptext, u16 rop);
/*数量文本绘制*/
void ga_draw_nwords(int16_t x, int16_t y, u8* ptext, u16 rop, u16 l);
/*在矩形内绘制文本*/
void ga_draw_textrec(PGDI_RECT prc, u8* ptext, uint16_t style, uint16_t rop);
/*绘制点*/
void ga_draw_point(u16 x, u16 y, u16 rop);
/*绘制线*/
void ga_draw_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t rop);
/*绘制矩形*/
void ga_draw_rec(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t rop);
/*绘制圆*/
void ga_draw_circle(uint16_t x,uint16_t y,uint16_t r,uint16_t rop);
/*绘制加载环*/
void ga_draw_ring(uint16_t x,uint16_t y,uint16_t r,uint16_t w,uint16_t ti);
/*绘制三角形*/
void ga_draw_triangle(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t rop);
/*绘制三角飘带*/
void ga_draw_rand_triangle(void);
/*绘制随即三角形*/
void ga_draw_ribbon(uint16_t xs,uint16_t y0,uint16_t yp,uint16_t rop);
/*位图旋转绘制*/
void ga_draw_bmp_rotate(uint32_t x,uint32_t y,uint16_t h,uint16_t w,uint16_t th,uint8_t * p,uint16_t rop);
/*世界线绘制*/
void ga_draw_worldline(void);

void sin_wave(void);

void sigmoid(void);

void N_distributio(void);

#endif




