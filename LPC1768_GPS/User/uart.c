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
uint8_t PC_Array_Buffer[512] = {0};//���ݽ��ջ�����
uint16_t Index = 0;
uint16_t Frame_Length = 512;

uint8_t UART0_RX_Oscilloscope(int32_t *Data); //oscilloscope
#endif

#endif

extern void  delayNS (uint32_t  ulDly)	 ;
                         
uint8_t 		GucRcvNew0;                                            /* ���ڽ��������ݵı�־         */
uint8_t         GucRcvBuf0[8] ;                                       /* ���ڽ������ݻ�����           */
uint32_t        GulNum0;  

uint8_t 		GucRcvNew1;                                            /* ���ڽ��������ݵı�־         */
uint8_t         GucRcvBuf1[8] ;                                       /* ���ڽ������ݻ�����           */
uint32_t        GulNum1;  

unsigned int Uart0RecvBuf;	  				// UART0 RX DATA

uint8_t USART1_RX_BUF[USART1_MAX_RECV_LEN]; 				//���ջ���,���USART2_MAX_RECV_LEN���ֽ�.
uint16_t USART1_RX_STA=0; 
uint16_t  USART1_FLAG=0;
uint8_t USART1_RX_BUF2[USART1_MAX_RECV_LEN];

extern void TIM0_Set(uint8_t sta);
extern nmea_msg gpsx;
uint8_t GPS_FRAME=0; //����0���յ�0x0A�ĸ�������֮���ڷ��ͳ�ȥ
/*********************************************************************************************************
* Function Name:        UART0_IRQHandler
* Description:          UART0 �жϴ�����
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
	{                               /*  �ж��Ƿ����жϹ���          */
        switch (LPC_UART0->IIR & 0x0E)
		{                                 /*  �ж��жϱ�־                */
        
            case 0x04:                                                  /*  ���������ж�                */
                GucRcvNew0 = 1;                                          /*  �ý��������ݱ�־            */
                for (GulNum0 = 0; GulNum0 < 8; GulNum0++)
				{                 /*  ��������8���ֽ�             */
                	GucRcvBuf0[GulNum0] = LPC_UART0->RBR;

					#ifdef _UART0_RX_Long
					PC_Array_Buffer[Index] = GucRcvBuf0[GulNum0];    
				    if(PC_Array_Buffer[0] == 0x24 && Index < Frame_Length)
				    {				             
						if(PC_Array_Buffer[Index] == 0x0A)
						{
							GPS_Analysis(&gpsx,(uint8_t*)PC_Array_Buffer);//�����ַ���
							GPS_FRAME++;
							if(GPS_FRAME>=7)
							{
								Gps_Msg_Show();				//��ʾ��Ϣ
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
            
            case 0x0C:                                                  /*  �ַ���ʱ�ж�                */
                GucRcvNew0 = 1;
                while ((LPC_UART0->LSR & 0x01) == 0x01)
				{                /*  �ж������Ƿ�������        */ 
                    GucRcvBuf0[GulNum0] = LPC_UART0->RBR;

					#ifdef _UART0_RX_Long
					PC_Array_Buffer[Index] = GucRcvBuf0[GulNum0];    
				    if(PC_Array_Buffer[0] == 0xAA && Index < Frame_Length)
				    {
				        Index++;      //ֻ���յ�0XAA  Index���Լ�
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
** Descriptions:	    ���ڳ�ʼ��������Ϊ8λ����λ��1λֹͣλ������żУ�飬������Ϊ9600
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void uart0Init (uint32_t UART0_BPS)
{
    uint16_t ulFdiv;

    LPC_PINCON->PINSEL0 |= (0x01 << 4)|(0x01 << 6); 
    LPC_SC->PCONP |= 0x08;                                              /* �򿪴���0����                */    
    LPC_UART0->LCR  = 0x83;                                             /*  �������ò�����              */
    ulFdiv = (SystemCoreClock / 4 / 16) / UART0_BPS;                     /*  ���ò�����                  */
    LPC_UART0->DLM  = ulFdiv / 256;
    LPC_UART0->DLL  = ulFdiv % 256; 
    LPC_UART0->LCR  = 0x03;                                             /*  ����������                  */
    LPC_UART0->FCR  = 0x87;                                             /*  ʹ��FIFO������8���ֽڴ����� */
    NVIC_EnableIRQ(UART0_IRQn);
    NVIC_SetPriority(UART0_IRQn, 5);
    LPC_UART0->IER  = 0x01;                                             /*  ʹ�ܽ����ж�                */
}

/*********************************************************************************************************
** Function name:	    uart0SendByte
** Descriptions:	    �򴮿ڷ����ӽ����ݣ����ȴ����ݷ�����ɣ�ʹ�ò�ѯ��ʽ
** input parameters:    uiDat:   Ҫ���͵�����
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void uart0SendByte (uint8_t ucDat)
{
    LPC_UART0->THR = ucDat;                                             /*  д������                    */
    while ((LPC_UART0->LSR & 0x20) == 0);                               /*  �ȴ����ݷ������            */
}

/********************************************************************************
** Descriptions			��Uart0 Receive Data
**
** parameters			��None 
** Returned value		��Received Data
*********************************************************************************/
unsigned int Uart0RecvByte(void)	  //��ѯ��
{
	//��UnRBR����δ���ַ�ʱ��UnLSR[0]�ͻᱻ��λ����UARTn RBR FIFOΪ��ʱ��UnLSR[0]�ͻᱻ����
	//0  -  UnRBRΪ��
	//1  -  UnRBR������Ч����
	while(!((LPC_UART0->LSR) & 0x01));		//�ȴ��ж�LSR[0]�Ƿ���1,1ʱ��ʾRBR�н��յ�����		
	
	return(LPC_UART0->RBR);		  			//��ȡ��������
}

/********************************************************************************
** Descriptions			��Uart0 Send Data
**
** parameters			��None 
** Returned value		��None
*********************************************************************************/
int Uart0SendByte(unsigned int buf)
{
	//����⵽UARTn THR�ѿ�ʱ��THRE�ͻ����������á�дUnTHR������THRE
	//0  -  UnTHR������Ч�ַ�
	//1  -  UnTHRΪ��
	while(!((LPC_UART0->LSR) & 0x20));		//�ȴ��ж�LSR[5](��THRE)�Ƿ���1,1ʱ��ʾTHR��Ϊ��		
	
	LPC_UART0->THR = buf;		  			//��������

	return 0;
}


/*********************************************************************************************************
** Function name:	    uart0SendStr
** Descriptions:	    �򴮿ڷ����ַ���
** input parameters:    pucStr:  Ҫ���͵��ַ���ָ��
**                      ulNum:   Ҫ���͵����ݸ���
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void uart0SendStr (uint8_t const *pucStr, uint32_t ulNum)
{
    uint32_t i;
    
    for (i = 0; i < ulNum; i++) 
	{                                       /* ����ָ�����ֽ�����            */
        uart0SendByte(*pucStr++);
    }
}



/*********************************************************************************************************
** ��������:  uart1Init
** ��������:  ���ڳ�ʼ��������Ϊ8λ����λ��1λֹͣλ������żУ��
** �������:  ulBaud�����ڲ�����
** �������:  ��
** �� �� ֵ:  ��
*********************************************************************************************************/
void uart1Init (uint32_t ulBaud)
{
    uint16_t ulFdiv;
	LPC_PINCON->PINSEL0 |= (1 << 30);
	LPC_PINCON->PINSEL1 |= (1 << 0);			
	LPC_SC->PCONP |= 1<<4;	
	LPC_UART1->LCR = 0x03;
	LPC_UART1->LCR |= 1<<7;

    ulFdiv = (SystemCoreClock / 4 / 16) / ulBaud;                       /* ���ò�����                   */
    LPC_UART1->DLM  = ulFdiv / 256;
    LPC_UART1->DLL  = ulFdiv % 256;

    LPC_UART1->LCR  = 0x03;                                             /* ����������                   */

//	LPC_UART1->FCR  = 0x06;
    LPC_UART1->FCR  = 0x87;                                             /* ʹ��FIFO������8���ֽڴ�����  */
    NVIC_EnableIRQ(UART1_IRQn);
    NVIC_SetPriority(UART1_IRQn, 5);
    LPC_UART1->IER  = 0x01;                                             /* ʹ�ܽ����ж�                 */
}

/*********************************************************************************************************
** Function name:	    uart1GetByte
** Descriptions:	    �Ӵ��ڽ���1�ֽ����ݣ�ʹ�ò�ѯ��ʽ����
** input parameters:    ��
** output parameters:   ��
** Returned value:      ucRcvData:   ���յ�������
*********************************************************************************************************/
uint8_t uart1GetByte (void)
{
    uint8_t ucRcvData;
    
    while ((LPC_UART1->LSR & 0x01) == 0);                               /* �ȴ����ձ�־��λ             */
    ucRcvData = LPC_UART1->RBR;                                         /* ��ȡ����                     */
    return (ucRcvData);   
}

/*********************************************************************************************************
** Function name:	    uart1GetStr
** Descriptions:	    ���ڽ����ַ���
** input parameters:    pucStr:   ָ��������������ָ��
**                      ulNum:   �������ݵĸ���
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void uart1GetStr (uint8_t *pucStr, uint32_t ulNum)
{
    for (; ulNum > 0; ulNum--){
        *pucStr++ =  uart1GetByte ();
    }  
}
/*********************************************************************************************************
** Function name:	    uart1SendByte
** Descriptions:	    �򴮿ڷ����ӽ����ݣ����ȴ����ݷ�����ɣ�ʹ�ò�ѯ��ʽ
** input parameters:    uiDat:   Ҫ���͵�����
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void uart1SendByte (uint8_t ucDat)
{
    LPC_UART1->THR = ucDat;                                             /*  д������                    */
    while ((LPC_UART1->LSR & 0x20) == 0);                               /*  �ȴ����ݷ������            */
}

/*********************************************************************************************************
** Function name:	    uart1SendStr
** Descriptions:	    �򴮿ڷ����ַ���
** input parameters:    pucStr:  Ҫ���͵��ַ���ָ��
**                      ulNum:   Ҫ���͵����ݸ���
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void uart1SendStr (uint8_t const *pucStr, uint32_t ulNum)
{
    uint32_t i;
    
    for (i = 0; i < ulNum; i++) 
	{                                       /* ����ָ�����ֽ�����            */
        uart1SendByte(*pucStr++);
    }
}



/*********************************************************************************************************
** ��������:  UART1_IRQHandler
** ��������:  UART1 �жϴ�����
** �������:  ��
** �������:  ����ȡ�Ĵ�����x��y����ֵ���浽GsKeyXY������
** �� �� ֵ:  ��
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
	{                               /*  �ж��Ƿ����жϹ���          */
        switch (LPC_UART1->IIR & 0x0E)
		{                                 /*  �ж��жϱ�־                */
        
            case 0x04:                                                  /*  ���������ж�                */
                GucRcvNew1 = 1;                                          /*  �ý��������ݱ�־            */
                for (GulNum1 = 0; GulNum1 < 8; GulNum1++)
				{                 /*  ��������8���ֽ�             */
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
            
            case 0x0C:                                                  /*  �ַ���ʱ�ж�                */
                GucRcvNew1 = 1;
                while ((LPC_UART1->LSR & 0x01) == 0x01)
				{                /*  �ж������Ƿ�������        */ 
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
********************����ʾ��������***************************
************************************************************/
#ifdef 	_UART0_Oscilloscope
#ifdef _UART0_Transmit_Long
/*
*����ʾ��������:����*
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
	Temp_B_H = Data >> 24;//��������
	
	uart0SendByte(0xaa);
	uart0SendByte(0x09);
	uart0SendByte(channel);
	
	ch_Temp ^= 0x09;
	ch_Temp ^= channel;
	ch_Temp ^= Temp_B_H;
	ch_Temp ^= Temp_B_L;
	ch_Temp ^= Temp_A_H;
	ch_Temp ^= Temp_A_L;//�ж���

	uart0SendByte(Temp_A_L);
	uart0SendByte(Temp_A_H);
	uart0SendByte(Temp_B_L);
	uart0SendByte(Temp_B_H);

	uart0SendByte(ch_Temp);
	uart0SendByte(0x55);
}
#endif

/*
*����ʾ��������:����*
*/
#ifdef _UART0_RX_Long

//
uint8_t UART0_RX_PC_Data(uint8_t *p_Array, uint8_t ch_FL) 
{
    uint8_t i = 0;
    if(Frame_Length != ch_FL)  //֪ͨ��Ƭ������һ֡���ݵĴ�С
    {
        Frame_Length = ch_FL;
    }
	 	
    if(Index == Frame_Length)  //��������һ֡����ʱ���Զ��飬 ������Сʱ�����ˣ�������1
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
	uint8_t Temp_Array[9] = {0}; //����9λ��
    uint8_t i =0;  
    int32_t Temp_Rate = 0;

    if(UART0_RX_PC_Data(Temp_Array, 9)) 
    {
        if((Temp_Array[0] == 0xaa)&&(Temp_Array[8] == 0x55))   // �ٴμ���
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
