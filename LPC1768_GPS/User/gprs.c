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
 
uint8_t AT_ATE0[]="ATE0\r\n";   //关闭回显
uint8_t AT_CGATT[]="AT+CGATT=1\r\n";  //附着测试，返回OK
uint8_t AT_CGDCONT[]="AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n";  //OK	，配置APN
uint8_t AT_CGRGE[]="AT+CGREG?\r\n";  //OK	 //+CGREG: 0,1   这条是确定能否连接GPRS服务
uint8_t AT_ETCPIP[]="AT%ETCPIP=\"user\",\"gprs\"\r\n";	//OK
uint8_t AT_ETCPIP_Q[]="AT%ETCPIP?\r\n";			//%ETCPIP:1,"10.40.153.234",,"211.137.96.205","211.136.20.203"  查看IP等信息
uint8_t AT_DNSR[]="AT%DNSR=\"galuo.eicp.net\"\r\n";					 //220.170.79.208貌似免费花生壳用户都是这一个IP  域名解析
uint8_t AT_IPOPEN[]="AT%IPOPEN=\"TCP\",\"202.105.21.214\",58950\r\n";//CONNECT 建立TCP连接，如果本地没有建立TCP服务监听，则会马上继续返回%IPCLOSE: 1
uint8_t AT_IOMODE[]="AT%IOMODE=1,1,1\r\n";
uint8_t AT_IPSEND_HEAD[]="AT%IPSEND=\"";	   //发送数据的数据头
uint8_t AT_IPSEND_END[]="\"\r\n";	   //发送数据的数据尾
uint8_t AT_IPCLOSE[]="AT%IPCLOSE\r\n";		  //关闭连接

uint8_t Demo_Str[]="31323334354142";

#define MAX_ERROR_COUNT 30000
uint16_t err = 0;

unsigned char is_gprs_mode_ok = 1;                  //gprs start succeed
unsigned char is_gprs_mode_start_succeed = 0;       //gprs connect ok
GTM900C_STATUS current_status = GPRS_AT_CGATT_1;

void delay_GSM(unsigned int i)          //延时函数
{
    unsigned int i_delay,j_delay;
    for(i_delay=0;i_delay<i;i_delay++)
    {for(j_delay=0;j_delay<3000;j_delay++)
        {;}}
}

/*********************初始化函数 发送关闭回显*******************************/
static void func_send_AT_ATE0()                     
{

//    PUT("S_INIT0 \r\n");

    clear_buf_uart1();
    uart1SendStr(AT_ATE0,sizeof(AT_ATE0)-1);
    current_status = GPRS_AT_CGATT_1 ;
}

/*************************附着测试***************************/
static void func_send_AT_CGATT()                           
{
    if(strstr((const char*)USART1_RX_BUF,"OK"))
    {

//        PUT("ECHO0 OK \r\n");
        delay_GSM(5000);
        is_gprs_mode_start_succeed = 1;         //说明GPRS模块存在

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

/*************************配置APN***************************/
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


/*************************确定能否连接GPRS服务***************************/
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

/*************************开启TCP/IP服务***************************/
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

/*************************查询该模块的IP地址等信息***************************/
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

/*************************域名解析***************************/
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

/*************************打开IP地址和端口号***************************/
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
* Description:          初始化GSM模块
* Input:                None
* Output:               None
* Return:               None

********************************************/
void init_gprs_mode(void)
{
//	GPRS_ATE0();          //	关闭回显
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
** Descriptions:	    向GPRS发送字符串
** input parameters:    pucStr:  要发送的字符串指针
**                      ulNum:   要发送的数据个数
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
uint8_t GPRS_send_str(uint8_t const *pucStr, uint32_t ulNum)
{
	uart1SendStr(AT_IPSEND_HEAD,sizeof(AT_IPSEND_HEAD)-1);
	uart1SendStr(pucStr,ulNum);
	uart1SendStr(AT_IPSEND_END,sizeof(AT_IPSEND_END)-1);
	return 0;
}









