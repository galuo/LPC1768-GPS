#include <stdint.h>
#include "ili_lcd_general.h"

extern const char english[][16];

void LCD_write_english(uint16_t x,uint16_t y,uint8_t str,unsigned int color,unsigned int xcolor)//Ð´×Ö·û
{
    uint16_t xpos = x;
    uint16_t ypos = y;

    unsigned char avl,i,n;

    for(i=0; i<16; i++) //16ÐÐ
    {
        avl= (english[str-32][i]);
        lcd_SetCursor(xpos,ypos);
        ypos++;
        rw_data_prepare();
        for(n=0; n<8; n++) //8ÁÐ
        {
            if(avl&0x80) write_data(color);
            else write_data(xcolor);
            avl<<=1;
        }
    }
}

void LCD_write_english_string(uint16_t x,uint16_t y,char *s,unsigned int color,unsigned int xcolor)//Ó¢ÎÄ×Ö·û´®ÏÔÊ¾
{
    while (*s)
    {
        LCD_write_english(x,y,*s,color,xcolor);
        s++;
        x += 8;
    }
}

