#include "ili_lcd_general.h"
#include "debug_frmwrk.h"
/*---------------------------- Global variables ------------------------------*/

static void delay (int cnt)
{

    cnt <<= DELAY_2N;
    while (cnt--);
}

#if   defined ( __CC_ARM )
__asm void wait()
{	
    NOP
    BX lr
}
#elif defined ( __ICCARM__ )

void wait()
{
	__ASM(
	"NOP\n"
	"BX lr"
	);
}
#endif

__INLINE void wait_delay(int count)
{
    volatile unsigned int tmp = count;
    while( tmp--);
}

__INLINE void lcd_send (unsigned short byte)
{
    LPC_GPIO2->FIODIR |= 0xFF;        //P2.0...P2.7 Output
    LCD_DIR(1);		   				  //Interface A->B
    LCD_EN(0);	                      //Enable 2A->2B
    LPC_GPIO2->FIOPIN =  byte;        //Write D0..D7
    LCD_LE(1);
    LCD_LE(0);						  //latch D0..D7
    LPC_GPIO2->FIOPIN =  byte >> 8;   //Write D8..D15
}

__INLINE unsigned short lcd_read (void)
{
    unsigned short id;
    LPC_GPIO2->FIODIR &= ~(0xFF);                  //P2.0...P2.7 Input
    LCD_DIR(0);		   				               //Interface B->A
    LCD_EN(0);	                                   //Enable 2B->2A
    wait_delay(50);							       //delay some times
    id = LPC_GPIO2->FIOPIN0;                       //Read D8..D15
    LCD_EN(1);	                                   //Enable 1B->1A
    wait_delay(50);							       //delay some times
    id = (id << 8) | LPC_GPIO2->FIOPIN0;           //Read D0..D7
    LCD_DIR(1);
    return(id);
}

static void lcd_port_init(void)
{
    /* Configure the LCD Control pins
        set 
        P0[23..20]
        P1[25..23]
        as output.
    */
    FIO_SetDir(0,(1<<20)|(1<<21)|(1<<22)|(1<<23),1);
    FIO_SetDir(1,(1<<23)|(1<<24)|(1<<25),1);
    FIO_SetValue(0,(1<<20)|(1<<21)|(1<<22)|(1<<23));
    FIO_SetValue(1,(1<<23)|(1<<24)|(1<<25));
}

__INLINE void write_cmd (unsigned char c)
{
    /**********************************
    // ** nCS      ---\________/------*
    // ** RS       ----\______/-------*
    // ** nRD      -------------------*
    // ** nWR      -----\___/---------*
    // ** DB[0-15] ------[###]--------*
    **********************************/
    LCD_CS(0);
    LCD_RS(0)
    LCD_RD(1);
    lcd_send(c);
    wait_delay(50);
    LCD_WR(0);
    wait();
    LCD_WR(1);
    LCD_CS(1);
}

__INLINE unsigned short read_data(void)
{
    /**********************************
    // ** nCS      ---\________/-----**
    // ** RS       ------------------**
    // ** nRD      -----\___/--------**
    // ** nWR      ------------------**
    // ** DB[0-15] ------[###]-------**
    **********************************/
    unsigned short val = 0;
    LCD_CS(0);
    LCD_RS(1);
    LCD_WR(1);
    LCD_RD(0);
    wait();
    val = lcd_read();
    LCD_RD(1);
    LCD_CS(1);
    return val;
}

void rw_data_prepare(void)
{
    write_cmd(0x22);
}

void write_data(unsigned short data )
{
    /**********************************
    // ** nCS      ---\________/-----**
    // ** RS       ------------------**
    // ** nRD      ------------------**
    // ** nWR      -----\___/--------**
    // ** DB[0-15] ------[###]-------**
    **********************************/
    LCD_CS(0);
    LCD_RS(1);
    lcd_send(data);
    LCD_WR(0);
    wait();
    LCD_WR(1);
    LCD_CS(1);
}

__INLINE unsigned short read_reg(unsigned char reg_addr)
{
    /*************************************************
    // ** nCS      ---\________/------\________/----**
    // ** RS       ----\______/---------------------**
    // ** nRD      ---------------------\___/-------**
    // ** nWR      -----\___/-----------------------**
    // ** DB[0-15] ------[###]-----------[###]------**
    *************************************************/
    unsigned short tmp;
    write_cmd(reg_addr);
    tmp = read_data();
    return tmp;
}

__INLINE void write_reg(unsigned char reg_addr,unsigned short reg_val)
{
    /*************************************************
    // ** nCS      ---\________/------\________/----**
    // ** RS       ----\______/---------------------**
    // ** nRD      ---------------------------------**
    // ** nWR      -----\___/-----------\___/-------**
    // ** DB[0-15] ------[###]-----------[###]------**
    *************************************************/
    write_cmd(reg_addr);
    write_data(reg_val);
}

/********* control <只移植以上函数即可> ***********/

static unsigned short deviceid=0;//设置一个静态变量用来保存LCD的ID

//返回LCD的ID
unsigned int lcd_getdeviceid(void)
{
    return deviceid;
}

unsigned short BGR2RGB(unsigned short c)
{
    uint16_t  r, g, b, rgb;

    b = (c>>0)  & 0x1f;
    g = (c>>5)  & 0x3f;
    r = (c>>11) & 0x1f;

    rgb =  (b<<11) + (g<<5) + (r<<0);

    return( rgb );
}

void lcd_SetCursor(unsigned int x,unsigned int y)
{
    // SSD1289 控制器的屏使用不同的寄存器
    if( deviceid == 0x8989 )
    {
        write_reg(0x004e,x);    /* 0-239 */
        write_reg(0x004f,y);    /* 0-319 */
    }
    write_reg(32,x);    /* 0-239 */
    write_reg(33,y);    /* 0-319 */
}

/* 读取指定地址的GRAM */
unsigned short lcd_read_gram(unsigned int x,unsigned int y)
{
    unsigned short temp;
    lcd_SetCursor(x,y);
    rw_data_prepare();
    /* dummy read */
    temp = read_data();
    temp = read_data();
    return temp;
}

void lcd_clear(unsigned short Color)
{
    unsigned int index=0;
    lcd_SetCursor(0,0);
    rw_data_prepare();                      /* Prepare to write GRAM */
    for (index=0; index<(LCD_WIDTH*LCD_HEIGHT); index++)
    {
        write_data(Color);
    }
}

void lcd_data_bus_test(void)
{
    unsigned short temp1;
    unsigned short temp2;

    if(deviceid == 0x8989)
    {
        /* [5:4]-ID~ID0 [3]-AM-1垂直-0水平 */
        write_reg(0x0011,0x6030 | (0<<3)); // AM=0 hline
    }
    else
    {
        /* [5:4]-ID~ID0 [3]-AM-1垂直-0水平 */
        write_reg(0x0003,(1<<12)|(1<<5)|(1<<4) | (0<<3) );
    }

    /* wirte */
    lcd_SetCursor(0,0);
    rw_data_prepare();
    write_data(0x5555);
    write_data(0xAAAA);

    /* read */
    lcd_SetCursor(0,0);
    if (
        (deviceid ==0x9325)
        || (deviceid ==0x9328)
        || (deviceid ==0x9320)
    )
    {
        temp1 = BGR2RGB( lcd_read_gram(0,0) );
        temp2 = BGR2RGB( lcd_read_gram(1,0) );
    }
    else if( (deviceid ==0x4531) || (deviceid == 0x8989 ) )
    {
        temp1 = lcd_read_gram(0,0);
        temp2 = lcd_read_gram(1,0);
    }

    if( (temp1 == 0x5555) && (temp2 == 0xAAAA) )
    {
        _printf("data bus test pass!\n");
    }
    else
    {
        _printf("data bus test error: %04X %04X\n",temp1,temp2);
    }
}

void lcd_gram_test(void)
{
    unsigned short temp;
    unsigned int test_x;
    unsigned int test_y;

    _printf("LCD GRAM test....\n");

    if( deviceid != 0x8989 )
    {
        /* [5:4]-ID~ID0 [3]-AM-1垂直-0水平 */
        write_reg(0x0003,(1<<12)|(1<<5)|(1<<4) | (0<<3) );
    }

    /* write */
    temp=0;
    lcd_SetCursor(0,0);
    rw_data_prepare();
    for(test_y=0; test_y<76800; test_y++)
    {
        write_data(temp);
        temp++;
    }

    /* read */
    temp=0;

    if (
        (deviceid ==0x9320)
        || (deviceid ==0x9325)
        || (deviceid ==0x9328)
    )
    {
        for(test_y=0; test_y<320; test_y++)
        {
            for(test_x=0; test_x<240; test_x++)
            {
                if( BGR2RGB( lcd_read_gram(test_x,test_y) ) != temp++)
                {
                    _printf("LCD GRAM ERROR!!\n");
                    return;
                }
            }
        }
        _printf("TEST PASS!\n");
    }
    else if( (deviceid ==0x4531) || (deviceid == 0x8989) )
    {
        for(test_y=0; test_y<320; test_y++)
        {
            for(test_x=0; test_x<240; test_x++)
            {
                if(  lcd_read_gram(test_x,test_y) != temp++)
                {
                    _printf("LCD GRAM ERROR!\n");
                    return;
                }
            }
        }
        _printf("TEST PASS!\n");
    }
}


void lcd_Initializtion(void)
{
    lcd_port_init();
    delay(15);
    deviceid = read_reg(0x00);
	

    /* deviceid check */
    if(
        (deviceid != 0x4531)
        && (deviceid != 0x7783)
        && (deviceid != 0x9320)
        && (deviceid != 0x9325)
        && (deviceid != 0x9328)
        && (deviceid != 0x9300)
        && (deviceid != 0x8989)
    )
    {
        _printf("Invalid LCD ID:%08X\n",deviceid);
        _printf("Please check you hardware and configure.\n");
        return;
    }
    else
    {
        _printf("LCD Device ID : %04X \n",deviceid);
    }

    if (deviceid==0x9325|| deviceid==0x9328)
    {
        write_reg(0x00e7,0x0010);
        write_reg(0x0000,0x0001);  			        //start internal osc
#if defined(_ILI_REVERSE_DIRECTION_)
        write_reg(0x0001,0x0000);                    //Reverse Display
#else
        write_reg(0x0001,0x0100);                    //
#endif
        write_reg(0x0002,0x0700); 				    //power on sequence
        /* [5:4]-ID1~ID0 [3]-AM-1垂直-0水平 */
        write_reg(0x0003,(1<<12)|(1<<5)|(0<<4) | (1<<3) );
        write_reg(0x0004,0x0000);
        write_reg(0x0008,0x0207);
        write_reg(0x0009,0x0000);
        write_reg(0x000a,0x0000); 				//display setting
        write_reg(0x000c,0x0001);				//display setting
        write_reg(0x000d,0x0000); 				//0f3c
        write_reg(0x000f,0x0000);
        //Power On sequence //
        write_reg(0x0010,0x0000);
        write_reg(0x0011,0x0007);
        write_reg(0x0012,0x0000);
        write_reg(0x0013,0x0000);
        delay(15);
        write_reg(0x0010,0x1590);
        write_reg(0x0011,0x0227);
        delay(15);
        write_reg(0x0012,0x009c);
        delay(15);
        write_reg(0x0013,0x1900);
        write_reg(0x0029,0x0023);
        write_reg(0x002b,0x000e);
        delay(15);
        write_reg(0x0020,0x0000);
        write_reg(0x0021,0x0000);
        delay(15);
        write_reg(0x0030,0x0007);
        write_reg(0x0031,0x0707);
        write_reg(0x0032,0x0006);
        write_reg(0x0035,0x0704);
        write_reg(0x0036,0x1f04);
        write_reg(0x0037,0x0004);
        write_reg(0x0038,0x0000);
        write_reg(0x0039,0x0706);
        write_reg(0x003c,0x0701);
        write_reg(0x003d,0x000f);
        delay(15);
        write_reg(0x0050,0x0000);
        write_reg(0x0051,0x00ef);
        write_reg(0x0052,0x0000);
        write_reg(0x0053,0x013f);
#if defined(_ILI_REVERSE_DIRECTION_)
        write_reg(0x0060,0x2700);
#else
        write_reg(0x0060,0xA700);
#endif
        write_reg(0x0061,0x0001);
        write_reg(0x006a,0x0000);
        write_reg(0x0080,0x0000);
        write_reg(0x0081,0x0000);
        write_reg(0x0082,0x0000);
        write_reg(0x0083,0x0000);
        write_reg(0x0084,0x0000);
        write_reg(0x0085,0x0000);
        write_reg(0x0090,0x0010);
        write_reg(0x0092,0x0000);
        write_reg(0x0093,0x0003);
        write_reg(0x0095,0x0110);
        write_reg(0x0097,0x0000);
        write_reg(0x0098,0x0000);
        //display on sequence
        write_reg(0x0007,0x0133);
        write_reg(0x0020,0x0000);
        write_reg(0x0021,0x0000);
    }
    else if( deviceid==0x9320|| deviceid==0x9300)
    {
        write_reg(0x00,0x0000);
#if defined(_ILI_REVERSE_DIRECTION_)
        write_reg(0x0001,0x0100);                    //Reverse Display
#else
        write_reg(0x0001,0x0000);                    // Driver Output Contral.
#endif
        write_reg(0x02,0x0700);	//LCD Driver Waveform Contral.
//		write_reg(0x03,0x1030);	//Entry Mode Set.
        write_reg(0x03,0x1018);	//Entry Mode Set.

        write_reg(0x04,0x0000);	//Scalling Contral.
        write_reg(0x08,0x0202);	//Display Contral 2.(0x0207)
        write_reg(0x09,0x0000);	//Display Contral 3.(0x0000)
        write_reg(0x0a,0x0000);	//Frame Cycle Contal.(0x0000)
        write_reg(0x0c,(1<<0));	//Extern Display Interface Contral 1.(0x0000)
        write_reg(0x0d,0x0000);	//Frame Maker Position.
        write_reg(0x0f,0x0000);	//Extern Display Interface Contral 2.

        delay(15);
        write_reg(0x07,0x0101);	//Display Contral.
        delay(15);

        write_reg(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4));	//Power Control 1.(0x16b0)
        write_reg(0x11,0x0007);								//Power Control 2.(0x0001)
        write_reg(0x12,(1<<8)|(1<<4)|(0<<0));					//Power Control 3.(0x0138)
        write_reg(0x13,0x0b00);								//Power Control 4.
        write_reg(0x29,0x0000);								//Power Control 7.

        write_reg(0x2b,(1<<14)|(1<<4));

        write_reg(0x50,0);		//Set X Start.
        write_reg(0x51,239);	//Set X End.
        write_reg(0x52,0);		//Set Y Start.
        write_reg(0x53,319);	//Set Y End.

#if defined(_ILI_REVERSE_DIRECTION_)
        write_reg(0x0060,0x2700);  //Driver Output Control.
#else
        write_reg(0x0060,0xA700);
#endif
        write_reg(0x61,0x0001);	//Driver Output Control.
        write_reg(0x6a,0x0000);	//Vertical Srcoll Control.

        write_reg(0x80,0x0000);	//Display Position? Partial Display 1.
        write_reg(0x81,0x0000);	//RAM Address Start? Partial Display 1.
        write_reg(0x82,0x0000);	//RAM Address End-Partial Display 1.
        write_reg(0x83,0x0000);	//Displsy Position? Partial Display 2.
        write_reg(0x84,0x0000);	//RAM Address Start? Partial Display 2.
        write_reg(0x85,0x0000);	//RAM Address End? Partial Display 2.

        write_reg(0x90,(0<<7)|(16<<0));	//Frame Cycle Contral.(0x0013)
        write_reg(0x92,0x0000);	//Panel Interface Contral 2.(0x0000)
        write_reg(0x93,0x0001);	//Panel Interface Contral 3.
        write_reg(0x95,0x0110);	//Frame Cycle Contral.(0x0110)
        write_reg(0x97,(0<<8));	//
        write_reg(0x98,0x0000);	//Frame Cycle Contral.


        write_reg(0x07,0x0173);	//(0x0173)
    }
    else if( deviceid==0x4531 )
    {
        // Setup display
        write_reg(0x00,0x0001);
        write_reg(0x10,0x0628);
        write_reg(0x12,0x0006);
        write_reg(0x13,0x0A32);
        write_reg(0x11,0x0040);
        write_reg(0x15,0x0050);
        write_reg(0x12,0x0016);
        delay(15);
        write_reg(0x10,0x5660);
        delay(15);
        write_reg(0x13,0x2A4E);
#if defined(_ILI_REVERSE_DIRECTION_)
        write_reg(0x01,0x0100);
#else
        write_reg(0x01,0x0000);
#endif
        write_reg(0x02,0x0300);

        write_reg(0x03,0x1030);
//	    write_reg(0x03,0x1038);

        write_reg(0x08,0x0202);
        write_reg(0x0A,0x0000);
        write_reg(0x30,0x0000);
        write_reg(0x31,0x0402);
        write_reg(0x32,0x0106);
        write_reg(0x33,0x0700);
        write_reg(0x34,0x0104);
        write_reg(0x35,0x0301);
        write_reg(0x36,0x0707);
        write_reg(0x37,0x0305);
        write_reg(0x38,0x0208);
        write_reg(0x39,0x0F0B);
        delay(15);
        write_reg(0x41,0x0002);

#if defined(_ILI_REVERSE_DIRECTION_)
        write_reg(0x0060,0x2700);
#else
        write_reg(0x0060,0xA700);
#endif

        write_reg(0x61,0x0001);
        write_reg(0x90,0x0119);
        write_reg(0x92,0x010A);
        write_reg(0x93,0x0004);
        write_reg(0xA0,0x0100);
//	    write_reg(0x07,0x0001);
        delay(15);
//	    write_reg(0x07,0x0021);
        delay(15);
//	    write_reg(0x07,0x0023);
        delay(15);
//	    write_reg(0x07,0x0033);
        delay(15);
        write_reg(0x07,0x0133);
        delay(15);
        write_reg(0xA0,0x0000);
        delay(20);
    }
    else if( deviceid ==0x7783)
    {
        // Start Initial Sequence
        write_reg(0x00FF,0x0001);
        write_reg(0x00F3,0x0008);
        write_reg(0x0001,0x0100);
        write_reg(0x0002,0x0700);
        write_reg(0x0003,0x1030);  //0x1030
        write_reg(0x0008,0x0302);
        write_reg(0x0008,0x0207);
        write_reg(0x0009,0x0000);
        write_reg(0x000A,0x0000);
        write_reg(0x0010,0x0000);  //0x0790
        write_reg(0x0011,0x0005);
        write_reg(0x0012,0x0000);
        write_reg(0x0013,0x0000);
        delay(20);
        write_reg(0x0010,0x12B0);
        delay(20);
        write_reg(0x0011,0x0007);
        delay(20);
        write_reg(0x0012,0x008B);
        delay(20);
        write_reg(0x0013,0x1700);
        delay(20);
        write_reg(0x0029,0x0022);

        //################# void Gamma_Set(void) ####################//
        write_reg(0x0030,0x0000);
        write_reg(0x0031,0x0707);
        write_reg(0x0032,0x0505);
        write_reg(0x0035,0x0107);
        write_reg(0x0036,0x0008);
        write_reg(0x0037,0x0000);
        write_reg(0x0038,0x0202);
        write_reg(0x0039,0x0106);
        write_reg(0x003C,0x0202);
        write_reg(0x003D,0x0408);
        delay(20);
        write_reg(0x0050,0x0000);
        write_reg(0x0051,0x00EF);
        write_reg(0x0052,0x0000);
        write_reg(0x0053,0x013F);
        write_reg(0x0060,0xA700);
        write_reg(0x0061,0x0001);
        write_reg(0x0090,0x0033);
        write_reg(0x002B,0x000B);
        write_reg(0x0007,0x0133);
        delay(20);
    }
    else if( deviceid == 0x8989 )
    {
        // power supply setting
        // set R07h at 0021h (GON=1,DTE=0,D[1:0]=01)
        write_reg(0x0007,0x0021);
        // set R00h at 0001h (OSCEN=1)
        write_reg(0x0000,0x0001);
        // set R07h at 0023h (GON=1,DTE=0,D[1:0]=11)
        write_reg(0x0007,0x0023);
        // set R10h at 0000h (Exit sleep mode)
        write_reg(0x0010,0x0000);
        // Wait 30ms
        delay(3000);
        // set R07h at 0033h (GON=1,DTE=1,D[1:0]=11)
        write_reg(0x0007,0x0033);
        // Entry mode setting (R11h)
        // R11H Entry mode
        // vsmode DFM1 DFM0 TRANS OEDef WMode DMode1 DMode0 TY1 TY0 ID1 ID0 AM LG2 LG2 LG0
        //   0     1    1     0     0     0     0      0     0   1   1   1  *   0   0   0
        write_reg(0x0011,0x6070);
        // LCD driver AC setting (R02h)
        write_reg(0x0002,0x0600);
        // power control 1
        // DCT3 DCT2 DCT1 DCT0 BT2 BT1 BT0 0 DC3 DC2 DC1 DC0 AP2 AP1 AP0 0
        // 1     0    1    0    1   0   0  0  1   0   1   0   0   1   0  0
        // DCT[3:0] fosc/4 BT[2:0]  DC{3:0] fosc/4
        write_reg(0x0003,0x0804);//0xA8A4
        write_reg(0x000C,0x0000);//
        write_reg(0x000D,0x080C);//
        // power control 4
        // 0 0 VCOMG VDV4 VDV3 VDV2 VDV1 VDV0 0 0 0 0 0 0 0 0
        // 0 0   1    0    1    0    1    1   0 0 0 0 0 0 0 0
        write_reg(0x000E,0x2900);
        write_reg(0x001E,0x00B8);
        write_reg(0x0001,0x2B3F);//驱动输出控制320*240  0x6B3F
        write_reg(0x0010,0x0000);
        write_reg(0x0005,0x0000);
        write_reg(0x0006,0x0000);
        write_reg(0x0016,0xEF1C);
        write_reg(0x0017,0x0003);
        write_reg(0x0007,0x0233);//0x0233
        write_reg(0x000B,0x0000|(3<<6));
        write_reg(0x000F,0x0000);//扫描开始地址
        write_reg(0x0041,0x0000);
        write_reg(0x0042,0x0000);
        write_reg(0x0048,0x0000);
        write_reg(0x0049,0x013F);
        write_reg(0x004A,0x0000);
        write_reg(0x004B,0x0000);
        write_reg(0x0044,0xEF00);
        write_reg(0x0045,0x0000);
        write_reg(0x0046,0x013F);
        write_reg(0x0030,0x0707);
        write_reg(0x0031,0x0204);
        write_reg(0x0032,0x0204);
        write_reg(0x0033,0x0502);
        write_reg(0x0034,0x0507);
        write_reg(0x0035,0x0204);
        write_reg(0x0036,0x0204);
        write_reg(0x0037,0x0502);
        write_reg(0x003A,0x0302);
        write_reg(0x003B,0x0302);
        write_reg(0x0023,0x0000);
        write_reg(0x0024,0x0000);
        write_reg(0x0025,0x8000);   // 65hz
        write_reg(0x004f,0);        // 行首址0
        write_reg(0x004e,0);        // 列首址0
		lcd_clear( Blue );
        //数据总线测试,用于测试硬件连接是否正常.
//        lcd_data_bus_test();
        //清屏
//        lcd_clear( Blue );
        //return ;
    }

    //数据总线测试,用于测试硬件连接是否正常.
//    lcd_data_bus_test();
    //GRAM测试,此测试可以测试LCD控制器内部GRAM.测试通过保证硬件正常
//    lcd_gram_test();

    //清屏
    lcd_clear( Blue );
}
