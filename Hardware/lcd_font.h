#ifndef __LCD_FONT_H
#define __LCD_FONT_H

#include <stdint.h>

/* 8x16 ASCII（0x20~0x7E，共 95 个字符） */
#define LCD_ASCII_W 8
#define LCD_ASCII_H 16

/* 16x16 汉字（取模方式：逐行、每行 2 字节、高位在左） */
#define LCD_HZ_W    16
#define LCD_HZ_H    16

extern const unsigned char ascii_1608[95][16];

/* 汉字点阵与 Unicode 码点一一对应，顺序必须一致 */
extern const uint16_t hz_code[];
extern const unsigned char hz_16x16[][32];
extern const uint16_t hz_count;

#endif
