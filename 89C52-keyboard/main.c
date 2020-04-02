#include <reg52.h>

/*Variable type definition*/
#define uint8_t  unsigned char
#define uint16_t unsigned int 
/*Numerical definition*/
#define SET       1
#define RESET     0
#define R_GROUP_1 0
#define R_GROUP_2 1
#define C_GROUP_1 3
#define C_GROUP_2 4
/*Register bits declaration group*/

/*LED registerbits declaration*/
sbit LEDR  = P1^3;
sbit LEDY  = P1^4;
sbit LEDG  = P1^5;

/*Row registerbit declaration*/
sbit R0   = P2^0;
sbit R1   = P2^1;
sbit R2   = P2^2;
sbit R3   = P0^1;
sbit R4   = P0^0;
sbit R5   = P1^0;
sbit R6   = P1^1;
sbit R7   = P1^1;

/*Column registerbit declaration*/
sbit C0   = P2^3;
sbit C1   = P2^4;
sbit C2   = P2^5;
sbit C3   = P2^6;
sbit C4   = P2^7;
sbit C5   = P3^2;
sbit C6   = P1^7;
sbit C7   = P1^6;
sbit C8   = P0^7;
sbit C9   = P0^6;
sbit C10  = P0^2;
sbit C11  = P0^3;
sbit C12  = P0^4;
sbit C13  = P0^5;

/*Private function declaration*/
void sys_setup(void);
void sys_loop(void);
void delay_ms(uint16_t i);
void uart1_init(void);
void uart1_intrrupt(void);
void uart1_senddata(uint8_t dat);
void uart1_sendstring(uint8_t *s);
void keyboard_scan(void);
void keyboard_send(uint8_t row,uint8_t col);
void key_setgroup(uint8_t pos,uint8_t state);
void key_readgroup(uint8_t group,uint8_t * value);
/*Private variable definition*/
bit	B_TX1_Busy;
bit capslock = 1;
/*========================== application ==============================*/
/**
 * @brief  Application entry point.
 *
 * @param   void.
 *
 * @return  none.
 *
 */
void main()
{
	sys_setup();
	while(1)
	{
		sys_loop();
	}
}
/**
 * @brief  Application setup function.
 *
 * @param   void.
 *
 * @return  none.
 *
 */
void sys_setup(void)
{
	uart1_init();
	LEDG  = RESET;
}
/**
 * @brief  Application loop function.
 *
 * @param   void.
 *
 * @return  none.
 *
 */
void sys_loop(void)
{
  keyboard_scan();
}
/*========================== delay_ms ===============================*/
/**
 * @brief   delay_ms function.
 *
 * @param   i the value of delay ms.
 *
 * @return  none.
 *
 */
void delay_ms(uint16_t i)
{
	uint16_t x,y;
	for(x = 0; x < i;x++)
		for(y = 120;y > 0;y--);
}
/*======================== keyboard ================================*/
/**
 * @brief   keyboard scan function.
 *
 * @param   void.
 *
 * @return  none.
 *
 */
void keyboard_scan(void)
{
	static uint8_t row,col;
	key_setgroup(C_GROUP_1,SET);
	key_setgroup(R_GROUP_1,RESET);
	key_readgroup(C_GROUP_1,&col);
	if(col != 0xff)
	{
		delay_ms(50);
		key_setgroup(R_GROUP_1,SET);
	  key_setgroup(C_GROUP_1,RESET);
		key_readgroup(R_GROUP_1,&row);
		if(row != 0xff)
		{
			LEDY = RESET;
			keyboard_send(row,col);
			while(row != 0xff)
			{
				key_readgroup(R_GROUP_1,&row);
			}
			LEDY = SET;
		}
	}
	key_setgroup(C_GROUP_2,SET);
	key_setgroup(R_GROUP_2,RESET);
	key_readgroup(C_GROUP_2,&col);
	if(col != 0xff)
	{
		delay_ms(50);
		key_setgroup(R_GROUP_2,SET);
	  key_setgroup(C_GROUP_2,RESET);
		key_readgroup(R_GROUP_2,&row);
		if(row != 0xff)
		{
			LEDY = RESET;
			keyboard_send(row,col);
			while(row != 0xff)
			{
				key_readgroup(R_GROUP_2,&row);
			}
			LEDY = SET;
		}
	}
}
/**
 * @brief   keyboard message send function.
 *
 * @param   row the row of key map.
 *          col the col of key map.
 *
 * @return  none.
 *
 */
void keyboard_send(uint8_t row,uint8_t col)
{
	static uint8_t key_map1[4][10] = {
		'q','w','e','r','t','y','u','i','o','p',
		' ','a','s','d','f','g','h','j','k','l',
		'\n',0x01,0x02,'z','x','c','v','b','n','m',
	};
	static uint8_t key_map2[5][4] = {
	  '7','8','9','-',
		'4','5','6','+',
		'1','2','3','/',
		'0','.',0x03,'*',
	  0x04,0x06,0x05,0x07,
	};
	if((row == 2)&&(col == 2))
	{
		capslock = ~capslock;
		LEDR = capslock;
	}
	if((row < 3)&&(col < 10))
	{
		if((96 < key_map1[row][col] )&&(key_map1[row][col] < 123))
		{
			uart1_senddata(key_map1[row][col] - 32 * ~capslock);
		}
		else
		{
			uart1_senddata(key_map1[row][col]);
		}
	}
	else
	{
		uart1_senddata(key_map2[row - 3][col - 10]);
	}
}
/**
 * @brief   keyboard map read function.
 *
 * @param   value the value of row or col.
 *
 * @return  none.
 *
 */
void key_readgroup(uint8_t group,uint8_t * value)
{
	*value = 0xff;
	switch(group)
	{
		case R_GROUP_1 :
		  if(R0 == RESET)      *value = 0u;
		  else if(R1 == RESET) *value = 1u;
		  else if(R2 == RESET) *value = 2u;
		  break;
			
		case R_GROUP_2 :
		  if(R3 == RESET)      *value = 3u;
		  else if(R4 == RESET) *value = 4u;
		  else if(R5 == RESET) *value = 5u;
		  else if(R6 == RESET) *value = 6u;
		  else if(R7 == RESET) *value = 7u;
		  break;
		
		case C_GROUP_1 :
		  if(C0 == RESET)      *value = 0u;
		  else if(C1 == RESET) *value = 1u;
		  else if(C2 == RESET) *value = 2u;
		  else if(C3 == RESET) *value = 3u;
		  else if(C4 == RESET) *value = 4u;
		  else if(C5 == RESET) *value = 5u;
		  else if(C6 == RESET) *value = 6u;
		  else if(C7 == RESET) *value = 7u;
		  else if(C8 == RESET) *value = 8u;
		  else if(C9 == RESET) *value = 9u;
		  break;
		
		case C_GROUP_2 :
		  if(C10 == RESET)      *value = 10u;
		  else if(C11 == RESET) *value = 11u;
		  else if(C12 == RESET) *value = 12u;
		  else if(C13 == RESET) *value = 13u;
		  break;
		
		default:break;
	}
}
/**
 * @brief   key map control function.
 * @note    set the row or col to low or high volage level.
 * @param   pos control the row or col group.
 *          state the volage level of set
 * @return  the pos of key.
 *
 */
void key_setgroup(uint8_t group,uint8_t state)
{
	switch(group)
	{
		case R_GROUP_1 :
		  R0 = state;
		  R1 = state;
		  R2 = state;
		  break;
			
		case R_GROUP_2 :
			R3 = state;
		  R4 = state;
		  R5 = state;
		  R6 = state;
		  R7 = state;
		  break;
		
		case C_GROUP_1 :
		  C0 = state;
      C1 = state;
      C2 = state;
      C3 = state;
      C4 = state;
      C5 = state;
      C6 = state;
      C7 = state;	
		  C8 = state;
      C9 = state;	
		  break;
		
		case C_GROUP_2 :
		  C10 = state;
		  C11 = state;
		  C12 = state;
		  C13 = state;
		  break;
		
		default:break;
	}
}
/*========================== usart =================================*/
/**
 * @brief   chip uart1 init function.
 * @noet    bps115200 = 11059200/{32x[65535-(RCAP2H,RCAP2L)]}
 *          
 * @param   void.
 * @return  none.
 *
 */
void uart1_init(void)											
{
  SCON=0x50;															
  TH2=0xFF;
  TL2=0xFD;														
  RCAP2H=0xFF;
  RCAP2L=0xFD;														
  TCLK=1;
  RCLK=1;
  C_T2=0;
  EXEN2=0;
  TR2=1;				
	ES=1;
	EA=1;
}
/**
 * @brief   uart1 interrupt function.
 *       
 * @param   void.
 * @return  none.
 *
 */
void uart1_intrrupt(void) interrupt 4
{
	if(RI)
	{
		RI = 0;
    /*user codeRX*/
	}
	if(TI)
	{
		TI = 0;
		B_TX1_Busy = 0;
	}
}
/**
 * @brief   uart1 send byte function.
 *       
 * @param   dat a byte of string.
 * @return  none.
 *
 */
void uart1_senddata(uint8_t dat)
{
    while (B_TX1_Busy);
    B_TX1_Busy = 1;
    SBUF = dat;
}
/**
 * @brief   uart1 send string funation.
 *       
 * @param   s the string of send.
 * @return  none.
 *
 */
void uart1_sendstring(uint8_t *s)
{
    while (*s)
    {
        uart1_senddata(*s++);
    }
}
/*======================== end of file ==============================*/

