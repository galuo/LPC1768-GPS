#include "uart.h"                                                   
#include "gps.h" 
#include "system_LPC17xx.h"  
 
/**/
#define _UART0_Oscilloscope
#define _UART0_Transmit_Long
#define _UART0_RX_Long

#ifdef 	_UART0_Oscilloscope

#ifdef _UART0_Transmit_Long
void UART0_TX_Oscilloscope(int32_t Data , uint8_t channel); //oscilloscope
#endif

#ifdef _UART0_RX_Long
uint8_t PC_Array_Buffer[512] = {0};//数据接收缓冲区
uint16_t Index = 0;
uint16_t Frame_Length = 512;

uint8_t UART0_RX_Oscilloscope(int32_t *Data); //oscilloscope
#endif

#endif

extern void  delayNS (uint32_t  ulDly)	 ;
                         
uint8_t 		GucRcvNew0;                                            /* 串口接收新数据的标志         */
uint8_t         GucRcvBuf0[8] ;                                       /* 串口接收数据缓冲区           */
uint32_t        GulNum0;  

uint8_t 		GucRcvNew1;                                            /* 串口接收新数据的标志         */
uint8_t         GucRcvBuf1[8] ;                                       /* 串口接收数据缓冲区           */
uint32_t        GulNum1;  

unsigned int Uart0RecvBuf;	  				// UART0 RX DATA

uint8_t USART1_RX_BUF[USART1_MAX_RECV_LEN]; 				//接收缓冲,最大USART2_MAX_RECV_LEN个字节.
uint16_t USART1_RX_STA=0; 
uint16_t  USART1_FLAG=0;
uint8_t USART1_RX_BUF2[USART1_MAX_RECV_LEN];

extern void TIM0_Set(uint8_t sta);
extern nmea_msg gpsx;
uint8_t GPS_FRAME=0; //串口0接收的0x0A的个数到达之后，在发送出去
/*********************************************************************************************************
* Function Name:        UART0_IRQHandler
* Description:          UART0 中断处理函数
* Input:                None
* Output:               None
* Return:               None
*********************************************************************************************************/
void UART0_IRQHandler (void)
{
    uint8_t i;
	GulNum0 = 0;
	for(i = 0; i < 8;i++)
	{
   		GucRcvBuf0[i] ='\0';
	}
    while ((LPC_UART0->IIR & 0x01) == 0)
	{                               /*  判断是否有中断挂起          */
        switch (LPC_UART0->IIR & 0x0E)
		{                                 /*  判断中断标志                */
        
            case 0x04:                                                  /*  接收数据中断                */
                GucRcvNew0 = 1;                                          /*  置接收新数据标志            */
                for (GulNum0 = 0; GulNum0 < 8; GulNum0++)
				{                 /*  连续接收8个字节             */
                	GucRcvBuf0[GulNum0] = LPC_UART0->RBR;

					#ifdef _UART0_RX_Long
					PC_Array_Buffer[Index] = GucRcvBuf0[GulNum0];    
				    if(PC_Array_Buffer[0] == 0x24 && Index < Frame_Length)
				    {				             
						if(PC_Array_Buffer[Index] == 0x0A)
						{
							GPS_Analysis(&gpsx,(uint8_t*)PC_Array_Buffer);//分析字符串
							GPS_FRAME++;
							if(GPS_FRAME>=7)
							{
								Gps_Msg_Show();				//显示信息
								GPS_FRAME=0;
							}
							Index=0;
						}
						else
						{ 
							Index++;
						}
				    }
					#endif
                }
                break;
            
            case 0x0C:                                                  /*  字符超时中断                */
                GucRcvNew0 = 1;
                while ((LPC_UART0->LSR & 0x01) == 0x01)
				{                /*  判断数据是否接收完毕        */ 
                    GucRcvBuf0[GulNum0] = LPC_UART0->RBR;

					#ifdef _UART0_RX_Long
					PC_Array_Buffer[Index] = GucRcvBuf0[GulNum0];    
				    if(PC_Array_Buffer[0] == 0xAA && Index < Frame_Length)
				    {
				        Index++;      //只有收到0XAA  Index才自加
				    }
					#endif
                    GulNum0++;
                }
                break;
                
            default:
                break;
        }
    } 

}

/*********************************************************************************************************
** Function name:     	uart0Init
** Descriptions:	    串口初始化，设置为8位数据位，1位停止位，无奇偶校验，波特率为9600
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void uart0Init (uint32_t UART0_BPS)
{
    uint16_t ulFdiv;

    LPC_PINCON->PINSEL0 |= (0x01 << 4)|(0x01 << 6); 
    LPC_SC->PCONP |= 0x08;                                              /* 打开串口0功能                */    
    LPC_UART0->LCR  = 0x83;                                             /*  允许设置波特率              */
    ulFdiv = (SystemCoreClock / 4 / 16) / UART0_BPS;                     /*  设置波特率                  */
    LPC_UART0->DLM  = ulFdiv / 256;
    LPC_UART0->DLL  = ulFdiv % 256; 
    LPC_UART0->LCR  = 0x03;                                             /*  锁定波特率                  */
    LPC_UART0->FCR  = 0x87;                                             /*  使能FIFO，设置8个字节触发点 */
    NVIC_EnableIRQ(UART0_IRQn);
    NVIC_SetPriority(UART0_IRQn, 5);
    LPC_UART0->IER  = 0x01;                                             /*  使能接收中断                */
}

/*********************************************************************************************************
** Function name:	    uart0SendByte
** Descriptions:	    向串口发送子节数据，并等待数据发送完成，使用查询方式
** input parameters:    uiDat:   要发送的数据
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void uart0SendByte (uint8_t ucDat)
{
    LPC_UART0->THR = ucDat;                                             /*  写入数据                    */
    while ((LPC_UART0->LSR & 0x20) == 0);                               /*  等待数据发送完毕            */
}

/********************************************************************************
** Descriptions			：Uart0 Receive Data
**
** parameters			：None 
** Returned value		：Received Data
*********************************************************************************/
unsigned int Uart0RecvByte(void)	  //查询法
{
	//当UnRBR包含未读字符时，UnLSR[0]就会被置位；当UARTn RBR FIFO为空时，UnLSR[0]就会被清零
	//0  -  UnRBR为空
	//1  -  UnRBR包含有效数据
	while(!((LPC_UART0->LSR) & 0x01));		//等待判断LSR[0]是否是1,1时表示RBR中接收到数据		
	
	return(LPC_UART0->RBR);		  			//读取接收数据
}

/********************************************************************************
** Descriptions			：Uart0 Send Data
**
** parameters			：None 
** Returned value		：None
*********************************************************************************/
int Uart0SendByte(unsigned int buf)
{
	//当检测到UARTn THR已空时，THRE就会立即被设置。写UnTHR会清零THRE
	//0  -  UnTHR包含有效字符
	//1  -  UnTHR为空
	while(!((LPC_UART0->LSR) & 0x20));		//等待判断LSR[5](即THRE)是否是1,1时表示THR中为空		
	
	LPC_UART0->THR = buf;		  			//发送数据

	return 0;
}


/*********************************************************************************************************
** Function name:	    uart0SendStr
** Descriptions:	    向串口发送字符串
** input parameters:    pucStr:  要发送的字符串指针
**                      ulNum:   要发送的数据个数
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void uart0SendStr (uint8_t const *pucStr, uint32_t ulNum)
{
    uint32_t i;
    
    for (i = 0; i < ulNum; i++) 
	{                                       /* 发送指定个字节数据            */
        uart0SendByte(*pucStr++);
    }
}



/*********************************************************************************************************
** 函数名称:  uart1Init
** 函数功能:  串口初始化，设置为8位数据位，1位停止位，无奇偶校验
** 输入参数:  ulBaud：串口波特率
** 输出参数:  无
** 返 回 值:  无
*********************************************************************************************************/
void uart1Init (uint32_t ulBaud)
{
    uint16_t ulFdiv;
	LPC_PINCON->PINSEL0 |= (1 << 30);
	LPC_PINCON->PINSEL1 |= (1 << 0);			
	LPC_SC->PCONP |= 1<<4;	
	LPC_UART1->LCR = 0x03;
	LPC_UART1->LCR |= 1<<7;

    ulFdiv = (SystemCoreClock / 4 / 16) / ulBaud;                       /* 设置波特率                   */
    LPC_UART1->DLM  = ulFdiv / 256;
    LPC_UART1->DLL  = ulFdiv % 256;

    LPC_UART1->LCR  = 0x03;                                             /* 锁定波特率                   */

//	LPC_UART1->FCR  = 0x06;
    LPC_UART1->FCR  = 0x87;                                             /* 使能FIFO，设置8个字节触发点  */
    NVIC_EnableIRQ(UART1_IRQn);
    NVIC_SetPriority(UART1_IRQn, 5);
    LPC_UART1->IER  = 0x01;                                             /* 使能接收中断                 */
}

/*********************************************************************************************************
** Function name:	    uart1GetByte
** Descriptions:	    从串口接收1字节数据，使用查询方式接收
** input parameters:    无
** output parameters:   无
** Returned value:      ucRcvData:   接收到的数据
*********************************************************************************************************/
uint8_t uart1GetByte (void)
{
    uint8_t ucRcvData;
    
    while ((LPC_UART1->LSR & 0x01) == 0);                               /* 等待接收标志置位             */
    ucRcvData = LPC_UART1->RBR;                                         /* 读取数据                     */
    return (ucRcvData);   
}

/*********************************************************************************************************
** Function name:	    uart1GetStr
** Descriptions:	    串口接收字符串
** input parameters:    pucStr:   指向接收数据数组的指针
**                      ulNum:   接收数据的个数
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void uart1GetStr (uint8_t *pucStr, uint32_t ulNum)
{
    for (; ulNum > 0; ulNum--){
        *pucStr++ =  uart1GetByte ();
    }  
}
/*********************************************************************************************************
** Function name:	    uart1SendByte
** Descriptions:	    向串口发送子节数据，并等待数据发送完成，使用查询方式
** input parameters:    uiDat:   要发送的数据
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void uart1SendByte (uint8_t ucDat)
{
    LPC_UART1->THR = ucDat;                                             /*  写入数据                    */
    while ((LPC_UART1->LSR & 0x20) == 0);                               /*  等待数据发送完毕            */
}

/*********************************************************************************************************
** Function name:	    uart1SendStr
** Descriptions:	    向串口发送字符串
** input parameters:    pucStr:  要发送的字符串指针
**                      ulNum:   要发送的数据个数
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void uart1SendStr (uint8_t const *pucStr, uint32_t ulNum)
{
    uint32_t i;
    
    for (i = 0; i < ulNum; i++) 
	{                                       /* 发送指定个字节数据            */
        uart1SendByte(*pucStr++);
    }
}



/*********************************************************************************************************
** 函数名称:  UART1_IRQHandler
** 函数功能:  UART1 中断处理函数
** 输入参数:  无
** 输出参数:  将获取的触摸屏x，y坐标值保存到GsKeyXY数组中
** 返 回 值:  无
*********************************************************************************************************/
void UART1_IRQHandler (void)
{
    uint8_t i;
	GulNum1 = 0;
	for(i = 0; i < 30;i++)
	{
   		GucRcvBuf1[i] ='\0';
	}
    while ((LPC_UART1->IIR & 0x01) == 0)
	{                               /*  判断是否有中断挂起          */
        switch (LPC_UART1->IIR & 0x0E)
		{                                 /*  判断中断标志                */
        
            case 0x04:                                                  /*  接收数据中断                */
                GucRcvNew1 = 1;                                          /*  置接收新数据标志            */
                for (GulNum1 = 0; GulNum1 < 8; GulNum1++)
				{                 /*  连续接收8个字节             */
                	GucRcvBuf1[GulNum1] = LPC_UART1->RBR;
					USART1_RX_BUF[USART1_RX_STA]= GucRcvBuf1[GulNum1];
					if(USART1_RX_BUF[USART1_RX_STA]==0x0A)
					{
						USART1_FLAG = USART1_RX_STA;
						USART1_RX_STA=0;
					}
					else
					{
						USART1_RX_STA++;
					}
                }
                break;
            
            case 0x0C:                                                  /*  字符超时中断                */
                GucRcvNew1 = 1;
                while ((LPC_UART1->LSR & 0x01) == 0x01)
				{                /*  判断数据是否接收完毕        */ 
                    GucRcvBuf1[GulNum1] = LPC_UART1->RBR;
                    GulNum1++;
                }
                break;
                
            default:
                break;
        }

//		uart1SendStr(GucRcvBuf1,8);
    } 
}

void clear_buf_uart1(void)
{
	uint16_t i;
	LPC_UART1->IER  = 0x00;
	for(i=0;i<USART1_MAX_RECV_LEN;i++)
	{
	   USART1_RX_BUF[i]=0;
	}
	LPC_UART1->IER  = 0x01;
}







/************************************************************
********************串口示波器功能***************************
************************************************************/
#ifdef 	_UART0_Oscilloscope
#ifdef _UART0_Transmit_Long
/*
*串口示波器功能:发送*
*/
void UART0_TX_Oscilloscope(int32_t Data , uint8_t channel)
{	
	unsigned char ch_Temp = 0;
	unsigned char Temp_A_L;									
	signed char Temp_A_H;
	signed char Temp_B_L;
	signed char Temp_B_H;
	
	Temp_A_L = Data;
	Temp_A_H = Data >> 8;
	Temp_B_L = Data >> 16;
	Temp_B_H = Data >> 24;//存入数据
	
	uart0SendByte(0xaa);
	uart0SendByte(0x09);
	uart0SendByte(channel);
	
	ch_Temp ^= 0x09;
	ch_Temp ^= channel;
	ch_Temp ^= Temp_B_H;
	ch_Temp ^= Temp_B_L;
	ch_Temp ^= Temp_A_H;
	ch_Temp ^= Temp_A_L;//判断码

	uart0SendByte(Temp_A_L);
	uart0SendByte(Temp_A_H);
	uart0SendByte(Temp_B_L);
	uart0SendByte(Temp_B_H);

	uart0SendByte(ch_Temp);
	uart0SendByte(0x55);
}
#endif

/*
*串口示波器功能:接收*
*/
#ifdef _UART0_RX_Long

//
uint8_t UART0_RX_PC_Data(uint8_t *p_Array, uint8_t ch_FL) 
{
    uint8_t i = 0;
    if(Frame_Length != ch_FL)  //通知单片机接受一帧数据的大小
    {
        Frame_Length = ch_FL;
    }
	 	
    if(Index == Frame_Length)  //缓存中有一帧数据时可以读书， 超出大小时不存了，并返回1
    {
        for(i=0; i< Frame_Length; i++)
        {
            p_Array[i] = PC_Array_Buffer[i];
        }
        Index = 0;
        return 1;
    }
    return 0;
}

//
uint8_t UART0_RX_Oscilloscope(int32_t *pData)
{
	uint8_t Temp_Array[9] = {0}; //缓存9位数
    uint8_t i =0;  
    int32_t Temp_Rate = 0;

    if(UART0_RX_PC_Data(Temp_Array, 9)) 
    {
        if((Temp_Array[0] == 0xaa)&&(Temp_Array[8] == 0x55))   // 再次检验
        {
            for(i=3; i < 7; i++) 
            {               
                Temp_Rate |= Temp_Array[i] << ((i - 3) << 3);                  
            }

            *pData= Temp_Rate;   
            return 1;
        }
    }    
    return 0;
} //oscilloscope
#endif

#endif
