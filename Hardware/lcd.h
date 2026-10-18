#ifndef __LCD_H
#define __LCD_H

#include <stdint.h>

/* 分辨率：2.00 寸 ST7789，横向 320x240（旋转 270°） */
#define LCD_W 320
#define LCD_H 240

/* 画笔颜色（RGB565） */
#define WHITE       0xFFFF
#define BLACK       0x0000
#define BLUE        0x001F
#define RED         0xF800
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define MAGENTA     0xF81F
#define YELLOW      0xFFE0
#define GRAY        0x8430
#define GBLUE       0x07FF
#define BROWN       0xBC40
#define BRRED       0xFC07
#define DARKBLUE    0x01CF
#define LIGHTBLUE   0x7D7C
#define GRAYBLUE    0x5458
#define LIGHTGREEN  0x841F
#define LGRAY       0xC618
#define LGRAYBLUE   0xA651
#define LBBLUE      0x2B12

void LCD_Init(void);
void LCD_Clear(uint16_t color);

void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color);
void LCD_Fill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);

/* 显示一个 8x16 ASCII 字符 */
void LCD_ShowChar(uint16_t x, uint16_t y, char ch, uint16_t fc, uint16_t bc);
/* 显示一个 16x16 汉字，code 为 Unicode 码点（如 0x6211 = “我”） */
void LCD_ShowChinese(uint16_t x, uint16_t y, uint16_t code, uint16_t fc, uint16_t bc);
/* 显示字符串，支持 UTF-8 中英文混排（源码按 UTF-8 保存即可） */
void LCD_ShowString(uint16_t x, uint16_t y, const char *str, uint16_t fc, uint16_t bc);

#endif
