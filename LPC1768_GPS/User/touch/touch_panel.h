#include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "ili_lcd_general.h"
#include "lcd_api.h"
#ifndef __TOUCH_PANEL_H
#define __TOUCH_PANEL_H
/*
7  6 - 4  3      2     1-0
s  A2-A0 MODE SER/DFR PD1-PD0
*/

#define TOUCH_MSR_Y  0x90   //读X轴坐标指令 addr:1
#define TOUCH_MSR_X  0xD0   //读Y轴坐标指令 addr:3
// 触摸硬件连接: (POWERAVR 路虎 LPC1768开发板)
// SPI    <==> SSP1
// TP_CS  <==> P0.6  GPIO
// TP_INT <==> P0.19 中断 (EINT3)

                        /*P0.6 == TOUCH_CS# LOW*/
#define TP_CS_LOW()     GPIO_SetDir (0,(1<<6),1);   \
                        GPIO_ClearValue (0,(1<<6));

#define TP_CS_HIGH()    GPIO_SetDir (0,(1<<6),1);   \
                        GPIO_SetValue (0,(1<<6));

#define TP_DOWN()       (!((GPIO_ReadValue (0)) & (1<<19)))

uint8_t SPI_WriteByte(uint8_t data);
void touch_init(void);
void touch_debug(void);


#endif


