#ifndef __VGA_H
#define __VGA_H	 
#include "sys.h"
#include "stm32f10x.h"

void vga_device_init(void);
void vga_timer_init(void);
void spi_config(void);
void vga_GPIOinit(void);
void vga_loop(void);
void test(void);

#endif

















