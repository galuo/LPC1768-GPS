/**********************************************************************
* $Id$		main.c 				2011-12-28
*//**
* @file		main.c
* @brief	PS2 keyboard example
* @version	0.1
* @date		28. 12. 2011
* @author	zhoujie
*
* Copyright(C) 2010, NXP Semiconductor
* Copyright(C) 2011, WaveShare
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
**********************************************************************/
/* Include files */
#include "lpc17xx_libcfg.h"
#include "lpc17xx_gpio.h"
#include "debug_frmwrk.h"

#include "ili_lcd_general.h"
#include "lcd_api.h"
#include "touch_panel.h"

#include "LPC17xx.h"                                                    /* LPC17xx外设寄存器            */
#include  "system_LPC17xx.h"
#include "uart.h"  
#include "gps.h"
#include "gprs.h"
//#define UART_PORT 3 //debug uart port def
/* Add Private Types */
/* <<add private type here >> */

/* Add Private Variables */
/* <<add private variables here >> */

/* Add Private Functions */
/* <<add private functions here >> */

/**
 * @brief Main program body
 */
void delay(void)
{
    volatile unsigned int dl;
    for(dl=0; dl<7500000; dl++);
}

char device_str[20];
uint16_t deviceid;

nmea_msg gpsx;
const char* fixmode_tbl[4]={"Fail","Fail"," 2D "," 3D "};	//fix mode字符串 
__align(4) char dtbuf[100];   								//打印缓存器
uint8_t GPRS_Send_Str[100];

//软件延时
void  delayNS (uint32_t  ulDly)
{
    uint32_t  i;
    for (; ulDly > 0; ulDly--) {
        for (i = 0; i < 5000; i++);
    }
}

/***************************************** 
ASCII 到HEX的转换函数
入口参数：O_data: 转换数据的入口指针，
N_data: 转换后新数据的入口指针
len : 需要转换的长度
返回参数：-1:  转换失败
其它：转换后数据长度
注意：O_data[]数组中的数据在转换过程中会被修改。
****************************************/ 
int ascii_2_hex(uint8_t *O_data, uint8_t *N_data, int len) 
{ 
	int i,j,tmp_len; 
	uint8_t tmpData; 
	uint8_t *O_buf = O_data; 
	uint8_t *N_buf = N_data; 
	for(i = 0; i < len; i++) 
	{ 
		if ((O_buf[i] >= '0') && (O_buf[i] <= '9')) 
		{ 
		 	tmpData = O_buf[i] - '0'; 
		} 
		else if ((O_buf[i] >= 'A') && (O_buf[i] <= 'F')) //A....F 
		{ 
		 	tmpData = O_buf[i] - 0x37; 
		} 
		else if((O_buf[i] >= 'a') && (O_buf[i] <= 'f')) //a....f 
		{ 
		 	tmpData = O_buf[i] - 0x57; 
		} 
		else 
		{ 
			return -1; 
		} 
		O_buf[i] = tmpData; 
	} 
	for(tmp_len = 0,j = 0; j < i; j+=2) 
	{ 
	 	N_buf[tmp_len++] = (O_buf[j]<<4) | O_buf[j+1]; 
	} 
	return tmp_len;  
}

/***************************************** 
HEX 到ASCII的转换函数
入口参数：data: 转换数据的入口指针
buffer: 转换后数据入口指针
len : 需要转换的长度
返回参数：转换后数据长度
*******************************************/ 
int hex_2_ascii(uint8_t *data, char *buffer, int len) 
{ 
	const char ascTable[17] = {"0123456789ABCDEF"}; 
	char *tmp_p = buffer; 
	int i, pos; 
	pos = 0; 
	for(i = 0; i < len; i++) 
	{ 
		 tmp_p[pos++] = ascTable[data[i] >> 4]; 
		 tmp_p[pos++] = ascTable[data[i] & 0x0f];
	} 
	tmp_p[pos] = '\0'; 
	return pos;    
} 

//显示GPS定位信息 
void Gps_Msg_Show(void)
{
	uint16_t str_len;
 	float tp;
			   
	tp=gpsx.longitude;	   
	str_len=sprintf((char *)dtbuf,"$%.5f%1c",tp/=100000,gpsx.ewhemi);	//得到经度字符串	
	LCD_write_english_string(0,0,dtbuf,Cyan,Blue);
	str_len=hex_2_ascii(dtbuf,GPRS_Send_Str,str_len);
	GPRS_send_str(GPRS_Send_Str,str_len);
	   
	tp=gpsx.latitude;	   
	str_len=sprintf((char *)dtbuf,"$%.5f%1c",tp/=100000,gpsx.nshemi);	//得到纬度字符串
	LCD_write_english_string(0,20,dtbuf,Cyan,Blue);
	str_len=hex_2_ascii(dtbuf,GPRS_Send_Str,str_len);
	GPRS_send_str(GPRS_Send_Str,str_len);
	 
	tp=gpsx.altitude;	   
 	sprintf((char *)dtbuf,"Altitude:%.1fm     ",tp/=10);	    			//得到高度字符串
	LCD_write_english_string(0,40,dtbuf,Cyan,Blue);
	
	tp=gpsx.direction;	   
 	sprintf((char *)dtbuf,"Direction:%.1fd     ",tp/=10);	    			//得到地面航向字符串
	LCD_write_english_string(0,80,dtbuf,Cyan,Blue);	
	
	tp=gpsx.speed;	   
 	sprintf((char *)dtbuf,"Speed:%.3fkm/h     ",tp/=1000);		    		//得到速度字符串
	LCD_write_english_string(0,100,dtbuf,Cyan,Blue);	 			    
	
	if(gpsx.fixmode<=3)														//定位状态
	{  
		sprintf((char *)dtbuf,"Fix Mode:%s",fixmode_tbl[gpsx.fixmode]);
		LCD_write_english_string(0,120,dtbuf,Cyan,Blue);	
	}	 	   
	
	sprintf((char *)dtbuf,"Valid satellite:%02d",gpsx.posslnum);	 		//用于定位的卫星数
	LCD_write_english_string(0,140,dtbuf,Cyan,Blue);	    
	
	sprintf((char *)dtbuf,"Visible satellite:%02d",gpsx.svnum%100);	 		//可见卫星数
	LCD_write_english_string(0,180,dtbuf,Cyan,Blue);	 
	
	sprintf((char *)dtbuf,"BTC Date:%04d/%02d/%02d   ",gpsx.btc.year,gpsx.btc.month,gpsx.btc.date);	//显示BTC日期
	LCD_write_english_string(0,200,dtbuf,Cyan,Blue); 
	
	sprintf((char *)dtbuf,"BTC Time:%02d:%02d:%02d   ",gpsx.btc.hour,gpsx.btc.min,gpsx.btc.sec);	//显示BTC时间
	LCD_write_english_string(0,220,dtbuf,Cyan,Blue);
}

/* Support required entry point for other toolchain */
int main (void)
{
	debug_frmwrk_init();
//    _DBG("hello\n");
//    _DBG_("hello");
//    _printf("CoreClock: %d\n",SystemCoreClock);    

    //初始化LCD
    lcd_Initializtion();

    // 在屏上面显示LCD控制器的型号
    //deviceid = lcd_getdeviceid();

    sprintf(device_str,"LCD control: %04X",lcd_getdeviceid());
    LCD_write_english_string(0,0,device_str,Cyan,Blue);

//    touch_init();   
//	SystemInit(); 
	uart1Init (9600);
	init_gprs_mode();
	lcd_clear( Blue );
    uart0Init (9600);

    while(1)
	{

	}

	return 1;
}

#ifdef  DEBUG
/*******************************************************************************
* @brief		Reports the name of the source file and the source line number
* 				where the CHECK_PARAM error has occurred.
* @param[in]	file Pointer to the source file name
* @param[in]    line assert_param error line source number
* @return		None
*******************************************************************************/
void check_failed(uint8_t *file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while(1);
}
#endif

