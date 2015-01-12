#ifndef __UART_H
#define __UART_H

//#include "stdio.h"
#include"lpc17xx.h"

#define USART1_MAX_RECV_LEN		512					//最大接收缓存字节数
extern uint8_t  USART1_RX_BUF[USART1_MAX_RECV_LEN]; 		//接收缓冲,最大USART2_MAX_RECV_LEN字节
extern uint16_t USART1_RX_STA;
extern uint16_t  USART1_FLAG;
extern uint8_t USART1_RX_BUF2[USART1_MAX_RECV_LEN];
   
extern uint8_t PC_Array_Buffer[512];//数据接收缓冲区
extern uint16_t Index;

extern uint8_t 			GucRcvNew0;                                            /* 串口接收新数据的标志         */
extern uint8_t          GucRcvBuf0[8] ;                                       /* 串口接收数据缓冲区           */
extern uint32_t         GulNum0;  
                                     /* 串口接收数据的个数           */
extern uint8_t 			GucRcvNew1;                                            /* 串口接收新数据的标志         */
extern uint8_t          GucRcvBuf1[8] ;                                       /* 串口接收数据缓冲区           */
extern uint32_t         GulNum1;  
                                     /* 串口接收数据的个数           */
/*********************************************************************************************************
* Function Name:        UART0_IRQHandler
* Description:          UART0 中断处理函数
* Input:                None
* Output:               None
* Return:               None
*********************************************************************************************************/
void UART0_IRQHandler (void);
/*********************************************************************************************************
** Function name:     	uart0Init
** Descriptions:	    串口初始化，设置为8位数据位，1位停止位，无奇偶校验，波特率为UART_BPS
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void uart0Init (uint32_t UART0_BPS);
/*********************************************************************************************************
** Function name:	    uart0SendByte
** Descriptions:	    向串口发送子节数据，并等待数据发送完成，使用查询方式
** input parameters:    uiDat:   要发送的数据
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void uart0SendByte (uint8_t ucDat);
/*********************************************************************************************************
** Function name:	    uart0SendStr
** Descriptions:	    向串口发送字符串
** input parameters:    pucStr:  要发送的字符串指针
**                      ulNum:   要发送的数据个数
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void uart0SendStr (uint8_t const *pucStr, uint32_t ulNum);



/*********************************************************************************************************
* Function Name:        UART0_IRQHandler
* Description:          UART0 中断处理函数
* Input:                None
* Output:               None
* Return:               None
*********************************************************************************************************/
void UART1_IRQHandler (void);
void clear_buf_uart1(void);
/*********************************************************************************************************
** Function name:     	uart1Init
** Descriptions:	    串口初始化，设置为8位数据位，1位停止位，无奇偶校验，波特率为UART_BPS
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void uart1Init (uint32_t ulBaud);
/*********************************************************************************************************
** Function name:	    uart0SendByte
** Descriptions:	    向串口发送子节数据，并等待数据发送完成，使用查询方式
** input parameters:    uiDat:   要发送的数据
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void uart1SendByte (uint8_t ucDat);
/*********************************************************************************************************
** Function name:	    uart0SendStr
** Descriptions:	    向串口发送字符串
** input parameters:    pucStr:  要发送的字符串指针
**                      ulNum:   要发送的数据个数
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void uart1SendStr (uint8_t const *pucStr, uint32_t ulNum);

uint8_t uart1GetByte (void);

void uart1GetStr (uint8_t *pucStr, uint32_t ulNum);

/************************************************************
********************串口示波器功能***************************
************************************************************/
extern void UART0_TX_Oscilloscope(int32_t Data , uint8_t channel);

extern uint8_t UART0_RX_Oscilloscope(int32_t *Data);

#endif                                                                
/*********************************************************************************************************
 End Of File
*********************************************************************************************************/
