#ifndef __UART_H
#define __UART_H

//#include "stdio.h"
#include"lpc17xx.h"

#define USART1_MAX_RECV_LEN		512					//�����ջ����ֽ���
extern uint8_t  USART1_RX_BUF[USART1_MAX_RECV_LEN]; 		//���ջ���,���USART2_MAX_RECV_LEN�ֽ�
extern uint16_t USART1_RX_STA;
extern uint16_t  USART1_FLAG;
extern uint8_t USART1_RX_BUF2[USART1_MAX_RECV_LEN];
   
extern uint8_t PC_Array_Buffer[512];//���ݽ��ջ�����
extern uint16_t Index;

extern uint8_t 			GucRcvNew0;                                            /* ���ڽ��������ݵı�־         */
extern uint8_t          GucRcvBuf0[8] ;                                       /* ���ڽ������ݻ�����           */
extern uint32_t         GulNum0;  
                                     /* ���ڽ������ݵĸ���           */
extern uint8_t 			GucRcvNew1;                                            /* ���ڽ��������ݵı�־         */
extern uint8_t          GucRcvBuf1[8] ;                                       /* ���ڽ������ݻ�����           */
extern uint32_t         GulNum1;  
                                     /* ���ڽ������ݵĸ���           */
/*********************************************************************************************************
* Function Name:        UART0_IRQHandler
* Description:          UART0 �жϴ�����
* Input:                None
* Output:               None
* Return:               None
*********************************************************************************************************/
void UART0_IRQHandler (void);
/*********************************************************************************************************
** Function name:     	uart0Init
** Descriptions:	    ���ڳ�ʼ��������Ϊ8λ����λ��1λֹͣλ������żУ�飬������ΪUART_BPS
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void uart0Init (uint32_t UART0_BPS);
/*********************************************************************************************************
** Function name:	    uart0SendByte
** Descriptions:	    �򴮿ڷ����ӽ����ݣ����ȴ����ݷ�����ɣ�ʹ�ò�ѯ��ʽ
** input parameters:    uiDat:   Ҫ���͵�����
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void uart0SendByte (uint8_t ucDat);
/*********************************************************************************************************
** Function name:	    uart0SendStr
** Descriptions:	    �򴮿ڷ����ַ���
** input parameters:    pucStr:  Ҫ���͵��ַ���ָ��
**                      ulNum:   Ҫ���͵����ݸ���
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void uart0SendStr (uint8_t const *pucStr, uint32_t ulNum);



/*********************************************************************************************************
* Function Name:        UART0_IRQHandler
* Description:          UART0 �жϴ�����
* Input:                None
* Output:               None
* Return:               None
*********************************************************************************************************/
void UART1_IRQHandler (void);
void clear_buf_uart1(void);
/*********************************************************************************************************
** Function name:     	uart1Init
** Descriptions:	    ���ڳ�ʼ��������Ϊ8λ����λ��1λֹͣλ������żУ�飬������ΪUART_BPS
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void uart1Init (uint32_t ulBaud);
/*********************************************************************************************************
** Function name:	    uart0SendByte
** Descriptions:	    �򴮿ڷ����ӽ����ݣ����ȴ����ݷ�����ɣ�ʹ�ò�ѯ��ʽ
** input parameters:    uiDat:   Ҫ���͵�����
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void uart1SendByte (uint8_t ucDat);
/*********************************************************************************************************
** Function name:	    uart0SendStr
** Descriptions:	    �򴮿ڷ����ַ���
** input parameters:    pucStr:  Ҫ���͵��ַ���ָ��
**                      ulNum:   Ҫ���͵����ݸ���
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void uart1SendStr (uint8_t const *pucStr, uint32_t ulNum);

uint8_t uart1GetByte (void);

void uart1GetStr (uint8_t *pucStr, uint32_t ulNum);

/************************************************************
********************����ʾ��������***************************
************************************************************/
extern void UART0_TX_Oscilloscope(int32_t Data , uint8_t channel);

extern uint8_t UART0_RX_Oscilloscope(int32_t *Data);

#endif                                                                
/*********************************************************************************************************
 End Of File
*********************************************************************************************************/
