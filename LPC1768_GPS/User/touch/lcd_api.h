#ifndef LCD_API_H_INCLUDED
#define LCD_API_H_INCLUDED

#include <stdint.h>

extern void LCD_write_english(uint16_t x,uint16_t y,uint8_t str,unsigned int color,unsigned int xcolor);//д�ַ�
extern void LCD_write_english_string(uint16_t x,uint16_t y,char *s,unsigned int color,unsigned int xcolor);//Ӣ���ַ�����ʾ

#endif // LCD_API_H_INCLUDED
