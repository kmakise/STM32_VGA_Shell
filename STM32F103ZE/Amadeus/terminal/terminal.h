#ifndef __TERMINAL_H
#define __TERMINAL_H	 	    					    
#include "stm32f10x.h"
#include "components.h"

void termainal_init(void);
void termainal_load(void);
void interactive_editbox(void);
void display_editbox(edit_boxes *eb,uint8_t (*tl)[70]);

#endif




