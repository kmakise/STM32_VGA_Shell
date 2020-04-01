#include "graphical.h"
#include "math.h"
#include "delay.h"


uint8_t fb[VID_VSIZE][VID_HSIZE+2];	            /* 帧缓冲器 200x(50+2)x8*/

extern const u8 ascii_word[];

//*****************************************************************************
//	位块转移函数。
//  这个函数使用STM32的是位带模式来简化复杂的BITBLT功能。
//
//	从 cortex STM32 F10X参考手册（RM00 08）：
//	映射公式示出了如何将别名区域中的每个单词引用到位带区域中的相应比特。映射公式为：
//	bit_word_addr = bit_band_base + (byte_offset x 32) + (bit_number ?4)
//	位置:
//	bit_word_addr 是映射到目标位的别名内存区域中的地址。
//	bit_band_base 是别名区域的起始地址。
//	byte_offset 包含目标位比特数的位带区域中的字节数是目标位的位（0～7）。
//	例如:
//	下面的示例演示如何在别名区域中映射位于SRAM地址0x2000 300的字节的比特2：
//	0x22006008 = 0x22000000 + (0x300*32) + (2*4).
//	写入地址0x2200、600与SRAM地址0x2000 300字节上的位2的读修改写操作具有相同的效果。
//	读取地址0x2200、600在SRAM地址0x2000 300（0x01：位集；0x00：位重置）
//  返回字节的位2的值（0x01或0x00）。
//
//	进一步了解请参考 Cortex M3 技术参考手册
//
//	参数:
//
//		prc		剪辑矩形。所有的X/Y坐标在"prc"内，如果"prc"坐标将是整个显示区域。
//		x			位图X开始位置
//		y			位图Y起始位置
//		w			位图宽度，以像素为单位
//		h			位图高度，以像素为单位
//		bm		指向位图起始位置的指针
//		rop		光栅操作。参见GdiyRopxxxx定义
//
//	返回类型	无
//
//*****************************************************************************
void ga_bitblt(PGDI_RECT prc, int16_t x, int16_t y, int16_t w, int16_t h, u8* bm, uint16_t rop)
{
    uint16_t i, xz, xb, xt;
    uint32_t wb;				// 字节宽度
    uint32_t r;					//以位（相对于x）开始x位置
    uint32_t k;				
    uint32_t d;
    uint32_t offs;
    uint8_t  c;
    u8*	     fbPtr;				// 指向帧缓冲器位带区域的指针
    u8*	     fbBak;
    u8	     fb1;
    uint32_t fb2;
    uint32_t rp;
    u8*	     bmPtr;				//指向位图位的指针

    //计算剪辑区域
	if (prc != NULL) {
		x = prc->x + x;
		y = prc->y + y;
	}

    //以字节为单位获取总位图宽度
	wb = (uint32_t) w >> 3;
    //如果宽度小于1字节，则将其设置为1字节
	if ((wb << 3) < (uint32_t) w) ++wb;

    //在第一个字节内获得起始位
	d = (uint32_t) x >> 3;
	r  = ((uint32_t) x - (d << 3));

    //剪辑X
	if (prc == NULL) {
		if ((x + w) >= VID_PIXELS_X ) {
			xt =  VID_PIXELS_X - x;
		} else {
			xt = w;
		}
	} else {
		if ((x + w) >= (x + prc->w)) {
			xt = prc->w - x;
		} else {
			xt = w;
		}
	}

  //绘制位
	for (i = 0; i < h; i++) {
        // Clip Y
		if ((i + y) > (VID_VSIZE - 1)) return;

    //在位带模式下对帧缓冲区进行偏移
		offs = (((u32) x >> 3)) + ((u32) (y + i)  * VID_HSIZE_R);
		k = (u32) (&fb - 0x20000000);
		k += offs;
		fbPtr = (u8*) (0x22000000 + (k * 32) + ((7 - r) * 4));
		fbBak = (u8*) (0x22000000 + (k * 32) + 28);

    //获得位图位偏移
		bmPtr = bm + ((u32) i * wb);
		xz = w;

		xb = 0;
		for (xz = 0; xz < xt; xz++) {
			fb1 = ((u32) fbPtr) & 0x000000E0;
			if (xb++ == 0) {
				c = *bmPtr;
				++bmPtr;
			}
			xb &= 0x07;
			(c & 0x80) ? (rp = 1) : (rp = 0);
			switch(rop) {
				case GDI_ROP_COPY:	*fbPtr = rp;		break;
				case GDI_ROP_XOR:	*fbPtr ^= rp;		break;
				case GDI_ROP_AND:	*fbPtr &= rp;		break;
				case GDI_ROP_OR:	*fbPtr |= rp;		break;
				case GDI_ROP_DEL:	*fbPtr &= ~rp;		break;
			}
			fbPtr -= 4;
			fb2 = ((u32) fbPtr) & 0x000000E0;
			if (fb1 != fb2) 
			{
				fbPtr = fbBak + 32;
				fbBak = fbPtr;
			}
			c <<= 1;
		}
	}
}

//*****************************************************************************
//	使用系统字体在X/Y位置绘制文本。
//
//	参数：
//		x			  X起始位置
//		y			  Y起始位置
//		ptext		指向文本的指针
//		rop			光栅操作。参见GdiyRopxxxx定义
//
//	return			none
//*****************************************************************************
void ga_draw_text(int16_t x, int16_t y, u8* ptext, u16 rop)
{

    u16	l, i, pos, xp;
    u8	c;
    u8*	ptx;

	l = strLen(ptext);
	xp = x;
	for (i = 0; i < l; i++) {
		c = *(ptext++);
		if (c >= GDI_SYSFONT_OFFSET) {
			pos = (u16)(c - GDI_SYSFONT_OFFSET) * GDI_SYSFONT_BYTEWIDTH * GDI_SYSFONT_HEIGHT;
			ptx = ((u8*) ascii_word) + pos;
			ga_bitblt(NULL, xp, y, GDI_SYSFONT_WIDTH, GDI_SYSFONT_HEIGHT, ptx, rop);
			xp += GDI_SYSFONT_WIDTH;
			if (xp >= VID_PIXELS_X) return;
		}
	}
}
//指定要绘制的字母的数量
void ga_draw_nwords(int16_t x, int16_t y, u8* ptext, u16 rop, u16 l)
{

    u16	i, pos, xp;
    u8	c;
    u8*	ptx;

	xp = x;
	for (i = 0; i < l; i++) {
		c = *(ptext++);
		if (c >= GDI_SYSFONT_OFFSET) {
			pos = (u16) (c - GDI_SYSFONT_OFFSET) * GDI_SYSFONT_BYTEWIDTH * GDI_SYSFONT_HEIGHT;
			ptx = ((u8*) ascii_word) + pos;
			ga_bitblt(NULL, xp, y, GDI_SYSFONT_WIDTH, GDI_SYSFONT_HEIGHT, ptx, rop);
			xp += GDI_SYSFONT_WIDTH;
			if (xp >= VID_PIXELS_X) return;
		}
	}
}
//*****************************************************************************
//	在矩形内绘制文本
//
//	参数:
//		prc			矩形裁剪指针
//		ptext		指向文本的指针
//		style		文本样式（参见GdiyWiCabPosithXXX定义）
//		rop			Raster operation. See GDI_ROP_xxx defines
//
//	return			none
//*****************************************************************************
void ga_draw_textrec(PGDI_RECT prc,u8* ptext, uint16_t style, uint16_t rop) 
{
    uint16_t l1, l2, i, pos, xp;
    uint8_t	c;
    u8*	ptx;

    l1 = strLen(ptext);
	l2 = l1 * GDI_SYSFONT_WIDTH;
    
    // Decide where to start painting
	switch(style) {
		case GDI_WINCAPTION_RIGHT:		
            if (l2 < prc->w)
                prc->x += (prc->w - l2);
            break;
		case GDI_WINCAPTION_CENTER:	
            if (l2 < prc->w) 							
                prc->x += ((prc->w - l2) / 2);										
            break;
	}
	xp = 1; //prc->x;
	for (i = 0; i < l1; i++) {
		c = *(ptext++);
		if (c >= GDI_SYSFONT_OFFSET) {
            // Get the position of the letter in vga_word[]
			pos = (uint16_t) (c - GDI_SYSFONT_OFFSET) * GDI_SYSFONT_BYTEWIDTH * GDI_SYSFONT_HEIGHT;
			ptx = ((u8*) ascii_word) + pos;
            
			ga_bitblt(prc, xp, 0, GDI_SYSFONT_WIDTH, GDI_SYSFONT_HEIGHT, ptx, rop);
			xp += GDI_SYSFONT_WIDTH;
			if (xp >= ((prc->x + prc->w) - GDI_SYSFONT_WIDTH)) return;
		}
	}
}

//*****************************************************************************
//	使用存储在GrMODE变量中的当前图形模式在X/Y位置显示点
//
//	参数:
//		x			X 坐标
//		y			Y 坐标
//		rop		栅操作。你会看到_ ROP的GDI _ XXX
//
//	return:			none
//*****************************************************************************
void ga_draw_point(u16 x, u16 y, u16 rop)
{

  u16	w, r;
  u8	m;


    //点外显示区域测试

	if (x >= VID_PIXELS_X || y >= VID_PIXELS_Y) return;

	w = x >> 3;
	r = x - (w << 3);

    // Prepare mask
	m = (0x80 >> r);

	switch(rop) {
		case GDI_ROP_COPY:	fb[y][w] |= m;break;
		case GDI_ROP_XOR:		fb[y][w] ^= m;break;
		case GDI_ROP_AND:		fb[y][w] &= m;break;
		case GDI_ROP_DEL:		fb[y][w] &= ~m;break;
	}
}
//*****************************************************************************
//	用Bresenham算法绘制直线
//
//	这个函数取自这本书：
//	交互式计算机图形学，自顶向下的OpenGL方法，Edward Angel
//
//	参数:
//		x1			X起始位置
//		y1			Y起始位置
//		x2			X结束位置
//		y2			Y结束位置
//		rop			光栅操作。参照GDI_ROPE_XXX的定义
//
//	return			none
//*****************************************************************************
void ga_draw_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t rop) 
{
    int16_t	dx, dy, i, e;
    int16_t	incx = 1, incy = 1, inc1, inc2;
    int16_t	x, y;

	dx = x2 - x1;
	dy = y2 - y1;

	if(dx < 0) {
        dx = -dx;
        incx = -1;
    }
	if(dy < 0) { 
        dy = -dy;
        incy = -1;
    }

	x=x1;
	y=y1;

	if (dx > dy) {
		ga_draw_point(x, y, rop);
		e = 2*dy - dx;
		inc1 = 2 * ( dy -dx);
		inc2 = 2 * dy;
		for (i = 0; i < dx; i++) {
			if (e >= 0) {
				y += incy;
				e += inc1;
			}
			else {
				e += inc2;
			}
			x += incx;
			ga_draw_point(x, y, rop);
		}
	} else {
		ga_draw_point(x, y, rop);
		e = 2 * dx - dy;
		inc1 = 2 * (dx - dy);
		inc2 = 2 * dx;
		for(i = 0; i < dy; i++) {
			if (e >= 0) {
				x += incx;
				e += inc1;
			} else {
				e += inc2;
			}
			y += incy;
			ga_draw_point(x, y, rop);
		}
	}
}
//*****************************************************************************
//	绘制矩形
//
//	参数:
//		x1			X 起始位置
//		y1			Y 起始位置
//		x2			X 结束位置
//		y2			Y 结束位置
//		rop			光栅操作。参照GDI_ROPE_XXX的定义
//
//	return			none
//*****************************************************************************
void ga_draw_rec(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t rop) 
{
	ga_draw_line(x0, y0, x1, y0, rop);
	ga_draw_line(x0, y1, x1, y1, rop);
	ga_draw_line(x0, y0, x0, y1, rop);
	ga_draw_line(x1, y0, x1, y1, rop);
}
//*****************************************************************************
//	用Bresenham算法绘制单像素圆形 改进算法
//
//	这个函数取自这本书：
//	交互式计算机图形学，自顶向下的OpenGL方法，Edward Angel
//
//	参数:
//		x			  圆心X位置
//		y 			圆心Y位置、
//    r       半径长
//		rop			光栅操作。参照GDI_ROPE_XXX的定义
//
//	return			none
//*****************************************************************************
void ga_draw_circle(uint16_t x,uint16_t y,uint16_t r,uint16_t rop) 
{
	int p,dx,dy;
	p=3-2*r;
	dx=0;
	dy=r;
	for(;dx<=dy;)
	{
		ga_draw_point(dx+x,dy+y,rop);
		ga_draw_point(-dx+x,-dy+y,rop);
		ga_draw_point(-dx+x,dy+y,rop);
		ga_draw_point(dx+x,-dy+y,rop);
		ga_draw_point(dy+x,dx+y,rop);
		ga_draw_point(-dy+x,dx+y,rop);
		ga_draw_point(-dy+x,-dx+y,rop);
		ga_draw_point(dy+x,-dx+y,rop);
		if(p<0)
		{
			p=p+4*dx+6;
		}
		else 
		{
			dy--;p=p+4*(dx-dy)+10;
		}
		dx++;
	}
}
//*****************************************************************************
//	极坐标法绘制加载环
//
//
//	参数:
//		x			  圆心X位置
//		y 			圆心Y位置、
//    r       半径长
//    w       环宽
//    ti      移动一次的速度速度 毫秒
//
//	return			none
//*****************************************************************************
void ga_draw_ring(uint16_t x,uint16_t y,uint16_t r,uint16_t w,uint16_t ti) 
{
	
	uint16_t x0,y0,x1,y1;
	int16_t th; 
	float t;

	
	for(th = 0;th < 1270;th++)
	{
		t = (th / 100.0);
		x0 = x + r*cos(t);
		y0 = y + r*sin(t);
		x1 = x + (r-w)*cos(t);
		y1 = y + (r-w)*sin(t);
		ga_draw_line(x0,y0,x1,y1,GDI_ROP_COPY);
		
		t = (th/ 100.0)/2;
		x0 = x + r*cos(t);
		y0 = y + r*sin(t);
		x1 = x + (r-w)*cos(t);
		y1 = y + (r-w)*sin(t);
		ga_draw_line(x0,y0,x1,y1,GDI_ROP_DEL);
		ga_draw_line(x0+1,y0+1,x1+1,y1+1,GDI_ROP_DEL);
		delay_ms(ti); 
	}
	ga_draw_line(x+r-w,y-1,x+r-1,y-1,GDI_ROP_COPY);
	for(th = 2;th < 1270;th++)
	{
		t = (th / 100.0);
		x0 = x + r*cos(t);
		y0 = y + r*sin(t);
		x1 = x + (r-w)*cos(t);
		y1 = y + (r-w)*sin(t);
		ga_draw_line(x0,y0,x1,y1,GDI_ROP_DEL);
		ga_draw_line(x0+1,y0,x1+1,y1,GDI_ROP_DEL);
		ga_draw_line(x0-1,y0,x1-1,y1,GDI_ROP_DEL);
		ga_draw_line(x0,y0+1,x1,y1+1,GDI_ROP_DEL);
		ga_draw_line(x0,y0-1,x1,y1-1,GDI_ROP_DEL);

		t = (th/ 100.0)/2;
		x0 = x + r*cos(t);
		y0 = y + r*sin(t);
		x1 = x + (r-w)*cos(t);
		y1 = y + (r-w)*sin(t);
		ga_draw_line(x0,y0,x1,y1,GDI_ROP_COPY);
		delay_ms(ti);
	}
}
//*****************************************************************************
//	三点方式绘制三角形
//  以Bresenham直线算法为基础
//
//	参数:
//		x0			  p0x坐标
//		y0 			  p0y坐标
//    x1        p1x坐标
//		y1        p1y坐标
//    x2        p2x坐标
//    y2        p2y坐标
//	return			none
//*****************************************************************************
void ga_draw_triangle(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t rop) 
{
	ga_draw_line(x0, y0, x1, y1, rop);
	ga_draw_line(x1, y1, x2, y2, rop);
	ga_draw_line(x2, y2, x0, y0, rop);
}
//*****************************************************************************
//	绘制全屏随机动态三角形
//
//	return			none
//*****************************************************************************
#define POINT_NUM 10
void ga_draw_rand_triangle(void) 
{
	static uint16_t  x[POINT_NUM],y[POINT_NUM];
	static uint16_t  dx[POINT_NUM],dy[POINT_NUM];
	static _Bool flag = 0;
	uint16_t  num;
	 
	if(flag == 0)
	{
		flag = 1;
		for(num = 0; num < POINT_NUM;num++)
		{
			x[num] = 0;
			y[num] = 0;
			dx[num] = rand()%720;
			dy[num] = rand()%580;
		}
	}
	for(num = 0;num < POINT_NUM;num++)
	{
		if(dx[num] == x[num])
		{
			dx[num] = rand()%720;
		}
		else if (dx[num] > x[num]){x[num]++;}
		else{x[num]-- ;}
		
		if(dy[num] == y[num])
		{
			dy[num] = rand()%580;
		}
		else if (dy[num] > y[num]){y[num]++;}
		else{y[num]--;}
	}
	
	for(num = 0;num < (POINT_NUM -3);num++)
	{
		ga_draw_triangle(x[num],y[num],x[num+1],y[num+1],x[num+2],y[num+2],GDI_ROP_COPY);
	}
	delay_ms(70);
	for(num = 0;num < (POINT_NUM -3);num++)
	{
		ga_draw_triangle(x[num],y[num],x[num+1],y[num+1],x[num+2],y[num+2],GDI_ROP_DEL);
	}
}
//*****************************************************************************
//  绘制静态三角形飘带
//
//	参数:
//		xs			  x坐标步进最大距离
//		y0 			  y的起始坐标
//    yp        y的震荡幅度
//    rop       光栅操作
//	return			none
//*****************************************************************************
void ga_draw_ribbon(uint16_t xs,uint16_t y0,uint16_t yp,uint16_t rop) 
{
	uint16_t x[80],y[80];
	uint16_t num;

	for(num = 0;num < 80;num++)
	{
		x[num] = 0;
		y[num] = 0;
	}
	x[0] = 0;
	y[0] = y0;
	x[1] = 0;
	y[1] = y0+yp;
	num = 1;
	while(1)
	{
		num++;
		do
		{
			x[num] = x[num-1] + rand()%xs + 5;
		}
		while((x[num]-x[num-1]) < xs);
		if(x[num] > 800)return;
		do
		{
				y[num] = rand()%yp + y0 + 5;
		}
		while((y[num] - y[num-1]) > yp);
		ga_draw_triangle(x[num-2],y[num-2],x[num-1],y[num-1],x[num],y[num],GDI_ROP_COPY);
	}
}
//*****************************************************************************
//	位图旋转显示
//	旋转圆心参照位图中心点
//
//  旋转方式：
//
//  x0 = r*cos(g)
//  y0 = r*sin(g)
//
//  x = r*cos(g+th) = r*cos(g)*cos(th) - r*sin(g)sin(th) = x0*cos(th) - y0*sin(th)
//  y = r*sin(g+th) = r*sin(g)*cos(th) + r*cos(g)sin(th) = x0*sin(th) + y0*cos(th)
//
//  - -   -                   -  -  -
// | x | | cos(th) -sin(th) -a || x0 |
// | y |=| sin(th) cos(th)  -b || y0 |
// | 1 | | 0       0         1 || 1  |
//  - -   -                   -  -  -
//
//  -  -   -     -  - -
// | xp | | 1 0 a || x |
// | yp |=| 0 1 b || y |
// | 1  | | 0 0 1 || 1 |
//  -  -   -     -  - -
//
//	参数:
//		x			    图片圆心x的偏移
//		y 			  图片圆心y的偏移
//    h  				图片高度      	
//		w         图片宽度
//    th        旋转角度(0rad-6.28rad * 100)
//    p         指向图形数组的首地址
//
//    rop       光栅操作（参见宏定义组注释）
//
//	return			none
//*****************************************************************************
/*用于旋转显示静态显示*/
void ga_draw_bmp_rotate(uint32_t x,uint32_t y,uint16_t h,uint16_t w,uint16_t th,uint8_t * p,uint16_t rop) 
{
	
	/*旋转圆心坐标 = 起始坐标 + 宽(高) / 2*/
	uint16_t xo = w / 2;
	uint16_t yo = h / 2;
	
	/*点位置的临时变量*/
	int32_t xt;
	int32_t yt;
	
	/*当前使能坐标*/
	int32_t dx;
	int32_t dy;
	
	/*剩余的高和宽*/
	uint16_t wn;
	uint16_t hn;
	
	/*像素激活标志位*/
	_Bool m;
	
	/*行移位*/
	uint8_t har = w/8;
	
	/*字节中的位*/
	uint8_t bit;
	
	/*当前旋转矩阵 通过整数方式计算提高速度*/
	int16_t t[4]; 
	
	/*矩阵赋值*/
	t[0] =  cos(th/100.0)*255;
	t[1] = -sin(th/100.0)*255;
	t[2] =  sin(th/100.0)*255;
	t[3] =  cos(th/100.0)*255;
	
	/*统一移位 为整数运算做准备*/
	x = x<<8;
	y = y<<8;
	
	/*历遍位图*/
	for(hn = 0;hn < h;hn++)
	{
		for(wn = 0;wn < w;wn++)
		{
			/*计算字节内的位偏移*/
			bit = wn & 0x07;
			/*获得当前坐标的激活值*/
			m = (*(p + wn/8 + hn * har) & (0x80 >> bit))<<bit;
			/*平移坐标系 将圆心移动到原点*/
			xt = wn - xo;
			yt = hn - yo;
			
			/*旋转矩阵变换*/
			dx = xt*t[0] + yt*t[1];
			dy = xt*t[2] + yt*t[3];
			/*恢复坐标系*/
			dx = (dx + x)>>8;
			dy = (dy + y)>>8;
			if(m)
			{
				/*使能图形显示缓存对应位*/
				ga_draw_point(dx,dy,rop);
			}
			else
			{
				/*失能图形显示缓存对应位*/
				ga_draw_point(dx,dy,GDI_ROP_DEL);
			}
			
		}
	} 
}
//*****************************************************************************
//  绘制随机世界线线图
//
//	参数:       void.
//	return			none.
//*****************************************************************************
#define POINT_N 100
void ga_draw_worldline(void) 
{
	/*路径分支的目标点*/
	uint16_t xe[POINT_N];
	uint16_t ye[POINT_N];
	
	/*路径分支的坐标*/
	uint16_t x[POINT_N];
	uint16_t y[POINT_N];
	
	
	/*当前分支数*/
	uint16_t pn;
	
	/*xy步进距离幅度*/
	uint8_t xs = 20,ys = 30;
	
	uint16_t num,temp;
	_Bool flag = 0;
	
	pn = 20;
	
	for(num = 0;num<pn;num++)
	{
		y[num]  = num*20 + 100;
		xe[num] = rand()%xs;
		ye[num] = rand()%ys + y[num];
	}
	while(1)
	{
		/*设置下一列符合要求的随机目标点*/
		for(num = 0;num < pn;num++)
		{
			xe[num] += rand()%xs+40;
			if(x[num] > 730)
			{
				/*到达屏幕另一端后结束程序*/
				return;
			}
			if(rand()%2)
			{
				if(rand()%2)
				{
					ye[num] += rand()%xs;
				}
				else
				{
					ye[num] -= rand()%xs;
				}
			}
			else
			{
				if(rand()%2)
				{
					ye[num] -=  rand()%ys;
				}
				else
				{
					ye[num] +=  rand()%ys;
				}
			}
		}
		/*失能标志位*/
		flag = 0;
		/*开始目标路径规划，全部路径同时*/
		for(num = 0;flag == 0;num++)
		{
			if(num == pn)
			{
				num = 0;
			}
			if(x[num] < xe[num])
			{
				x[num]++;
			}
			else if(x[num] > xe[num])
			{
				x[num]--;
			}
			if(y[num] < ye[num])
			{
				y[num]++;
			}
			else if(y[num] > ye[num])
			{
				y[num]--;
			}
			for(temp = 0;temp < pn;temp++)
			{
				flag = 0;
				if((x[num]==xe[num])&&(y[num]==ye[num]))
				{
					flag = 1;
					temp = pn;
				}
			}
			ga_draw_point(x[num],y[num],GDI_ROP_COPY);
			delay_us(500);
		}
	}	
}
//*****************************************************************************
//  绘制sin wave
//
//	参数:       void.
//	return			none.
//*****************************************************************************
void sin_wave(void)
{
	static uint16_t x,y,num;
	ga_draw_circle(200,200,50,GDI_ROP_COPY);
	for(num = 0;num < 12;num++)
	{
		for(x = 200;x < 500;x++)
		{
			y =  (x-200)/5*(sin((x-200)/10.0-num))+200;
			ga_draw_point(x,y,GDI_ROP_COPY);
		}
		delay_ms(50);
		for(x = 200;x < 500;x++)
		{
			y =  (x-200)/5*(sin((x-200)/10.0-num))+200;
			ga_draw_point(x,y,GDI_ROP_XOR);
		}
	}
}
//*****************************************************************************
//  绘制sigmoid
//
//	参数:       void.
//	return			none.
//*****************************************************************************
void sigmoid(void)
{
	uint16_t x,y;
	uint16_t xh,yh;
	uint16_t num;
	_Bool flag;
	xh = 0;
	yh = 335 - 300*1/(1+exp(-(x/72.6-5)));
	for(x = 0;x < 726;x++)
	{
		y = 335 - 315*1/(1+exp(-(x/72.6-5)));
		ga_draw_line(xh+2,yh,x+2,y,GDI_ROP_COPY);
		ga_draw_line(xh+2,yh-1,x+2,y-1,GDI_ROP_COPY);
		yh = y;
		xh = x;
		delay_ms(5);
	}
}
//*****************************************************************************
//  绘制正态分布
//
//	参数:       void.
//	return			none.
//*****************************************************************************
void N_distributio(void)
{
	uint16_t num,p;
	int16_t x,y;
	uint16_t yt = 335;
	_Bool flag = 0;
	float u,b2;
	u =  5;
	b2 = 2;
/*正态分布曲线*/
	for(x= 0;x < 726;x++)
	{
		y = 335 - 600.0*exp(-(x/72.6-u)*(x/72.6-u)/(2*b2))/sqrt(2*b2*3.141592);
		ga_draw_point(x,y,GDI_ROP_COPY);
		ga_draw_point(x,y+1,GDI_ROP_COPY);
	}
	/*正态分布柱状图*/
	for(num = 0;num < 60;num++)
	{
		/*正态分布柱状图定点坐标计算*/
		x = num *12 + 6;
		y = 335 - 600.0 * 
		    exp(-(x/72.6-u)*(x/72.6-u) /
		    (2*b2))/sqrt(2*b2*3.141592);
		/*柱状图的绘制*/
		while(!flag)
		{
			ga_draw_line(x+1,yt,x+10,yt,GDI_ROP_XOR);
			if(yt==y)
			{
				flag = 1;
			}
			delay_ms((uint16_t)(50/(50*(yt-y)))+1);
			yt--;
		}
		yt = 335;
		flag = 0;
	}
}



