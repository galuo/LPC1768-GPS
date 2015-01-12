#include <stdlib.h>
#include <string.h>
//#include <stdio.h>
#include "gprs.h"
#include "uart.h"

#include "lpc17xx_libcfg.h"
#include "touch_panel.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_spi.h"
#include "lpc17xx_ssp.h"
#include "debug_frmwrk.h"
 
uint8_t AT_ATE0[]="ATE0\r\n";   //�رջ���
uint8_t AT_CGATT[]="AT+CGATT=1\r\n";  //���Ų��ԣ�����OK
uint8_t AT_CGDCONT[]="AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n";  //OK	������APN
uint8_t AT_CGRGE[]="AT+CGREG?\r\n";  //OK	 //+CGREG: 0,1   ������ȷ���ܷ�����GPRS����
uint8_t AT_ETCPIP[]="AT%ETCPIP=\"user\",\"gprs\"\r\n";	//OK
uint8_t AT_ETCPIP_Q[]="AT%ETCPIP?\r\n";			//%ETCPIP:1,"10.40.153.234",,"211.137.96.205","211.136.20.203"  �鿴IP����Ϣ
uint8_t AT_DNSR[]="AT%DNSR=\"galuo.eicp.net\"\r\n";					 //220.170.79.208ò����ѻ������û�������һ��IP  ��������
uint8_t AT_IPOPEN[]="AT%IPOPEN=\"TCP\",\"202.105.21.214\",58950\r\n";//CONNECT ����TCP���ӣ��������û�н���TCP���������������ϼ�������%IPCLOSE: 1
uint8_t AT_IOMODE[]="AT%IOMODE=1,1,1\r\n";
uint8_t AT_IPSEND_HEAD[]="AT%IPSEND=\"";	   //�������ݵ�����ͷ
uint8_t AT_IPSEND_END[]="\"\r\n";	   //�������ݵ�����β
uint8_t AT_IPCLOSE[]="AT%IPCLOSE\r\n";		  //�ر�����

uint8_t Demo_Str[]="31323334354142";

#define MAX_ERROR_COUNT 30000
uint16_t err = 0;

unsigned char is_gprs_mode_ok = 1;                  //gprs start succeed
unsigned char is_gprs_mode_start_succeed = 0;       //gprs connect ok
GTM900C_STATUS current_status = GPRS_AT_CGATT_1;

void delay_GSM(unsigned int i)          //��ʱ����
{
    unsigned int i_delay,j_delay;
    for(i_delay=0;i_delay<i;i_delay++)
    {for(j_delay=0;j_delay<3000;j_delay++)
        {;}}
}

/*********************��ʼ������ ���͹رջ���*******************************/
static void func_send_AT_ATE0()                     
{

//    PUT("S_INIT0 \r\n");

    clear_buf_uart1();
    uart1SendStr(AT_ATE0,sizeof(AT_ATE0)-1);
    current_status = GPRS_AT_CGATT_1 ;
}

/*************************���Ų���***************************/
static void func_send_AT_CGATT()                           
{
    if(strstr((const char*)USART1_RX_BUF,"OK"))
    {

//        PUT("ECHO0 OK \r\n");
        delay_GSM(5000);
        is_gprs_mode_start_succeed = 1;         //˵��GPRSģ�����

        clear_buf_uart1();
        current_status = GPRS_AT_CGDCONT_2 ;
        uart1SendStr(AT_CGATT,sizeof(AT_CGATT)-1);

    }else{
//        PUT(buf_uart3.buf);
//        PUT("ECHO0 ERR \r\n");

        clear_buf_uart1();
		uart1SendStr(AT_CGATT,sizeof(AT_CGATT)-1);
        
    }
}

/*************************����APN***************************/
static void func_send_AT_CGDCONT()                           
{
    if(strstr((const char*)USART1_RX_BUF,"OK"))
    {

//        PUT("ECHO0 OK \r\n");
        delay_GSM(5000);

        clear_buf_uart1();
        current_status = GPRS_AT_CGRGE_3 ;
        uart1SendStr(AT_CGDCONT,sizeof(AT_CGDCONT)-1);

    }else{
//        PUT(buf_uart3.buf);
//        PUT("ECHO0 ERR \r\n");

        clear_buf_uart1();
		uart1SendStr(AT_CGATT,sizeof(AT_CGATT)-1);
        
    }
}


/*************************ȷ���ܷ�����GPRS����***************************/
static void func_send_AT_CGRGE()                       
{
    if(strstr((const char*)USART1_RX_BUF,"OK"))
    {

//        PUT("ECHO0 OK \r\n");
        delay_GSM(5000);      

        clear_buf_uart1();
        current_status = GPRS_AT_ETCPIP_4 ;
        uart1SendStr(AT_CGRGE,sizeof(AT_CGRGE)-1);

    }else{
//        PUT(buf_uart3.buf);
//        PUT("ECHO0 ERR \r\n");

        clear_buf_uart1();
		uart1SendStr(AT_CGDCONT,sizeof(AT_CGDCONT)-1);
        
    }
}

/*************************����TCP/IP����***************************/
static void func_send_AT_ETCPIP()                       
{
    if(strstr((const char*)USART1_RX_BUF,"OK"))
    {

//        PUT("ECHO0 OK \r\n");
        delay_GSM(5000);      

        clear_buf_uart1();
        current_status = GPRS_AT_ETCPIP_Q_5 ;
        uart1SendStr(AT_ETCPIP,sizeof(AT_ETCPIP)-1);

    }else{
//        PUT(buf_uart3.buf);
//        PUT("ECHO0 ERR \r\n");

        clear_buf_uart1();
		uart1SendStr(AT_CGRGE,sizeof(AT_CGRGE)-1);
        
    }
}

/*************************��ѯ��ģ���IP��ַ����Ϣ***************************/
static void func_send_AT_ETCPIP_Q()                       
{
    if(strstr((const char*)USART1_RX_BUF,"OK"))
    {

//        PUT("ECHO0 OK \r\n");
        delay_GSM(5000);      

        clear_buf_uart1();
        current_status = GPRS_AT_DNSR_6 ;
        uart1SendStr(AT_ETCPIP_Q,sizeof(AT_ETCPIP_Q)-1);

    }else{
//        PUT(buf_uart3.buf);
//        PUT("ECHO0 ERR \r\n");

//        clear_buf_uart1();
//		uart1SendStr(AT_ETCPIP,sizeof(AT_ETCPIP)-1);
        
    }
}

/*************************��������***************************/
static void func_send_AT_DNSR()                       
{
    if(strstr((const char*)USART1_RX_BUF,"OK"))
    {

//        PUT("ECHO0 OK \r\n");
        delay_GSM(5000);      

        clear_buf_uart1();
        current_status = GPRS_AT_IPOPEN_7 ;
        uart1SendStr(AT_DNSR,sizeof(AT_DNSR)-1);

    }else{
//        PUT(buf_uart3.buf);
//        PUT("ECHO0 ERR \r\n");

        clear_buf_uart1();
		uart1SendStr(AT_ETCPIP_Q,sizeof(AT_ETCPIP_Q)-1);
        
    }
}

/*************************��IP��ַ�Ͷ˿ں�***************************/
static void func_send_AT_IPOPEN()                       
{
    if(strstr((const char*)USART1_RX_BUF,"OK"))
    {

//        PUT("ECHO0 OK \r\n");
        delay_GSM(5000);      

        clear_buf_uart1();
        current_status = GPRS_OPEN_FINISH ;
        uart1SendStr(AT_IPOPEN,sizeof(AT_IPOPEN)-1);

    }else{
//        PUT(buf_uart3.buf);
//        PUT("ECHO0 ERR \r\n");

        clear_buf_uart1();
		uart1SendStr(AT_DNSR,sizeof(AT_DNSR)-1);
//        uart1SendStr(AT_IPOPEN,sizeof(AT_IPOPEN)-1);
    }
}

/********************************************
 * Function Name:        init_gprs_mode
* Description:          ��ʼ��GSMģ��
* Input:                None
* Output:               None
* Return:               None

********************************************/
void init_gprs_mode(void)
{
//	GPRS_ATE0();          //	�رջ���
	uart1SendStr(AT_CGATT,sizeof(AT_CGATT)-1);
	LCD_write_english_string(0,20,"AT+CGATT=1",Green,Blue);
	delay_GSM(30000);
	uart1SendStr(AT_CGDCONT,sizeof(AT_CGDCONT)-1);
	LCD_write_english_string(0,40,"AT+CGDCONT=1,\"IP\",\"CMNET\"",Green,Blue);
	delay_GSM(30000);
	uart1SendStr(AT_CGRGE,sizeof(AT_CGRGE)-1);
	LCD_write_english_string(0,60,"AT+CGREG?",Green,Blue);
	delay_GSM(30000);
	uart1SendStr(AT_ETCPIP,sizeof(AT_ETCPIP)-1);
	LCD_write_english_string(0,80,"AT%ETCPIP=\"user\",\"gprs\"",Green,Blue);
	delay_GSM(30000);
	uart1SendStr(AT_ETCPIP_Q,sizeof(AT_ETCPIP_Q)-1);
	LCD_write_english_string(0,100,"AT%ETCPIP?",Green,Blue);
	delay_GSM(30000);
	uart1SendStr(AT_DNSR,sizeof(AT_DNSR)-1);
	LCD_write_english_string(0,120,"AT%DNSR=\"galuo.eicp.net\"",Green,Blue);
	delay_GSM(30000);
	uart1SendStr(AT_IPOPEN,sizeof(AT_IPOPEN)-1);
	LCD_write_english_string(0,140,"AT%IPOPEN=\"TCP\",\"202.105.21.214\",58950",Green,Blue);
	delay_GSM(60000);
//	while(1)
//	{
//		delay_GSM(20000);
//		GPRS_send_str(Demo_Str,sizeof(Demo_Str)-1);
//	}
//	while(1)
//	{	
//		err ++;
//        if(err > MAX_ERROR_COUNT)         //error
//        {
//            err = 0 ;
//
//            break;
//        }
//	    delay_GSM(3000);
//		
//		switch(current_status)
//        {
//	        case GPRS_AT_ATE0_0:
//	            func_send_AT_ATE0();     
//	            break ;
//	
//	        case GPRS_AT_CGATT_1:
//	            func_send_AT_CGATT();    
//	            break ;
//	
//	        case GPRS_AT_CGDCONT_2:
//	            func_send_AT_CGDCONT();   
//	            break ;
//	
//	        case GPRS_AT_CGRGE_3:
//	            func_send_AT_CGRGE();       
//	            break ;
//				
//			case GPRS_AT_ETCPIP_4:
//	            func_send_AT_ETCPIP();       
//	            break ;
//
//			case GPRS_AT_ETCPIP_Q_5:
//	            func_send_AT_ETCPIP_Q();       
//	            break ;
//
//			case GPRS_AT_DNSR_6:
//	            func_send_AT_DNSR();       
//	            break ;
//
//			case GPRS_AT_IPOPEN_7:
//	            func_send_AT_IPOPEN();       
//	            break ;
//
//	        case GPRS_OPEN_FINISH:
//	            return ;
//
//	        default:
//	            clear_buf_uart1();
//	            break ;
//        }
//
//	}

}

/*********************************************************************************************************
** Function name:	    GPRS_send_str
** Descriptions:	    ��GPRS�����ַ���
** input parameters:    pucStr:  Ҫ���͵��ַ���ָ��
**                      ulNum:   Ҫ���͵����ݸ���
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
uint8_t GPRS_send_str(uint8_t const *pucStr, uint32_t ulNum)
{
	uart1SendStr(AT_IPSEND_HEAD,sizeof(AT_IPSEND_HEAD)-1);
	uart1SendStr(pucStr,ulNum);
	uart1SendStr(AT_IPSEND_END,sizeof(AT_IPSEND_END)-1);
	return 0;
}









