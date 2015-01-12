#include "lpc17xx_libcfg.h"
#include "touch_panel.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_spi.h"
#include "lpc17xx_ssp.h"
#include "debug_frmwrk.h"
#define BUF_SIZE    0x01   // 1 Byte

// SPI Data Setup structure variable
SSP_DATA_SETUP_Type LCDSPI1Data;

uint8_t SPI_WriteByte(uint8_t data)
{
//    uint8_t touch_data_tx[BUF_SIZE];
    uint8_t touch_data_rx[BUF_SIZE];
    uint8_t rl;

    LCDSPI1Data.length = BUF_SIZE;
    LCDSPI1Data.tx_data = &data;

    rl = SSP_ReadWrite (LPC_SSP1,&LCDSPI1Data,SSP_TRANSFER_POLLING);
    if(rl != 0)
    {
        LCDSPI1Data.rx_data = &touch_data_rx;
        return  touch_data_rx[0];
        //_printf("%d\n",rl);
    }
    return 0;
}


void spi1_init()
{
    
    //pin configuration P0.[9..7]
    PINSEL_CFG_Type LCDSPI1_pins;

    // SPI Configuration structure variable
    SSP_CFG_Type LCDSPI1_ConfigSetup;    

    LCDSPI1_pins.Portnum = 0;    
    LCDSPI1_pins.Funcnum = PINSEL_FUNC_2;
    LCDSPI1_pins.Pinmode = PINSEL_PINMODE_PULLUP;
    LCDSPI1_pins.OpenDrain = PINSEL_PINMODE_NORMAL;
    LCDSPI1_pins.Pinnum = 7;
    PINSEL_ConfigPin(&LCDSPI1_pins);
    LCDSPI1_pins.Pinnum = 8;
    PINSEL_ConfigPin(&LCDSPI1_pins);
    LCDSPI1_pins.Pinnum = 9;
    PINSEL_ConfigPin(&LCDSPI1_pins);
    
    /*SPI1 configuration*/
    
    LCDSPI1_ConfigSetup.Databit = 8;
    LCDSPI1_ConfigSetup.CPHA = SPI_CPHA_FIRST ;
    LCDSPI1_ConfigSetup.CPOL = SSP_CPOL_HI;
    LCDSPI1_ConfigSetup.Mode = SSP_MASTER_MODE;    
    LCDSPI1_ConfigSetup.FrameFormat = SSP_FRAME_SPI;
    LCDSPI1_ConfigSetup.ClockRate = 400000; //400 KHz
    // initialize SSP configuration structure to default
    SSP_ConfigStructInit(&LCDSPI1_ConfigSetup);    
    // Initialize SSP peripheral with parameter given in structure above
    SSP_Init(LPC_SSP1,&LCDSPI1_ConfigSetup);
    // Enable SSP peripheral
    SSP_Cmd(LPC_SSP1, ENABLE);
}

void touch_init(void)
{
	/*
	 * Initialize pin connect
	 */                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
//    PINSEL_CFG_Type touch_init_cfg;
//	touch_init_cfg.Funcnum = PINSEL_FUNC_0;	//AS GPIO
//	touch_init_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
//	touch_init_cfg.Pinmode = PINSEL_PINMODE_PULLUP;
//	touch_init_cfg.Pinnum = PINSEL_PIN_19 ;          
//	touch_init_cfg.Portnum = PINSEL_PORT_0;                //P0.19 = touch_int pin
//  PINSEL_ConfigPin(&touch_init_cfg);

    GPIO_SetDir (0,(1<<19),0);  /* PORT0.19 defined as input        */
    GPIO_IntCmd (0,(1<<19),1);  /*P0.19 Falling edge interrupt      */
   
    NVIC_EnableIRQ(EINT3_IRQn);
    
    TP_CS_HIGH();

    //spi1_master_init();
    spi1_init();
    TP_CS_LOW();
    SPI_WriteByte( 1<<7 ); /* 打开中断 */
    TP_CS_HIGH();

    LCD_write_english_string(0,20,"touch init",Green,Blue);
    _printf("touch init\r\n");

}

void touch_debug(void)
{
    uint16_t tmpx[10];
    uint16_t tmpy[10];
    uint16_t touch_x,touch_y;
    unsigned int i;

    LCD_write_english_string(0,100,"          ",White,Blue);
    LCD_write_english_string(0,40,"touch down",White,Blue);

    // 如果一直按下,就一直读取并显示原始坐标
    while( TP_DOWN() )
    {
        for(i=0; i<10; i++)
        {
            TP_CS_LOW();
            SPI_WriteByte(TOUCH_MSR_X);                  /* read X */
            tmpx[i] = (SPI_WriteByte(0x00)&0x7F)<<5;     /* read MSB bit[11:8] */
            tmpx[i] |= SPI_WriteByte(TOUCH_MSR_Y)>>3;    /* read LSB bit[7:0] and prepare read Y */


            tmpy[i] = (SPI_WriteByte(0x00)&0x7F)<<5;     /* read MSB bit[11:8] */
            tmpy[i] |= SPI_WriteByte(0x00)>>3;           /* read LSB bit[7:0] */
            SPI_WriteByte( 1<<7 ); /* 再次打开中断 */
            TP_CS_HIGH();
        }

        //去最高值与最低值,再取平均值
        {
            uint32_t min_x = 0xFFFF,min_y = 0xFFFF;
            uint32_t max_x = 0,max_y = 0;
            uint32_t total_x = 0;
            uint32_t total_y = 0;
            unsigned int i;

            for(i=0; i<10; i++)
            {
                if( tmpx[i] < min_x )
                {
                    min_x = tmpx[i];
                }
                if( tmpx[i] > max_x )
                {
                    max_x = tmpx[i];
                }
                total_x += tmpx[i];

                if( tmpy[i] < min_y )
                {
                    min_y = tmpy[i];
                }
                if( tmpy[i] > max_y )
                {
                    max_y = tmpy[i];
                }
                total_y += tmpy[i];
            }
            total_x = total_x - min_x - max_x;
            total_y = total_y - min_y - max_y;
            touch_x = total_x / 8;
            touch_y = total_y / 8;
        }//去最高值与最低值,再取平均值

        //display
        {
            char x_str[20];
            char y_str[20];
            sprintf(x_str,"X: %04d",touch_x);
            sprintf(y_str,"Y: %04d",touch_y);
            LCD_write_english_string(0,60,x_str,Cyan,Blue);
            LCD_write_english_string(0,80,y_str,Cyan,Blue);
            _printf("\rtouch down!");
            _printf("X:%04d \r\nY:%04d \r\n        ",touch_x,touch_y);
        }
    }// 如果一直按下,就一直读取并显示原始坐标

    // touch up
    LCD_write_english_string(0,40,"           ",White,Blue);
    LCD_write_english_string(0,100,"touch up  ",White,Blue);
    _printf("\rtouch up!                        ");
}

/****************************EOF*************************************/