#ifndef __LOGIN_H
#define __LOGIN_H	 	    					    
#include "stm32f10x.h"
#include "components.h"
void get_chipID(u32* id);
void Sys_inf(void);
void Sys_load(void);
void Sys_login(void);
void passward(_Bool * lock,uint8_t * pswd_str,edit_boxes * pw);
#endif








