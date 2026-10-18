#ifndef __LCD_PORT_H
#define __LCD_PORT_H

#include <stdint.h>
#include "main.h"
#include "spi.h"

/* ============================ 屏幕接口 ============================
 *  屏上没有排针，只有一根 16pin / 0.5mm 间距的 FFC 软排线，
 *  插在板上的 FPC1 = FPC-05F-16PH20 座子上（17/18 脚为屏蔽/地，非信号）。
 *  屏侧为 22pin 的 FPC2 = AFC07-S22ECA-00，中间经转接板。
 *  排线内信号：SCK / SDA / DC / CS / RST / BLK / TE + VDD / GND / LEDK
 *  即 4 线单向 SPI（无 MISO），引脚 100% 由 PCB 走线决定，不可自选。
 * ======================== 引脚（原理图 / 厂家例程一致） ========================
 *   PA5  -> LCD_RST        复位（GPIO 推挽输出，低有效）
 *   PA6  -> LCD_BLK        背光（GPIO 推挽输出，高点亮）
 *   PA7  -> LCD_CS         片选（GPIO 推挽输出，常低）
 *   PB13 -> LCD_SCK        SPI2_SCK  (AF5)  板上网络 EXT_SPI2_SCK
 *   PB14 -> LCD_DC         数据/命令（GPIO 推挽输出，SPI2_MISO 复用）
 *   PB15 -> LCD_SDA        SPI2_MOSI (AF5)，单向 1LINE 模式
 *
 *  因为 PB14 被当 DC 用，SPI2 必须配成 Half-Duplex Master（1LINE），
 *  否则 CubeMX 会把 PB14 占成 SPI2_MISO。
 *  PB13 原来是 FDCAN2_TX，本工程只保留 FDCAN1（PA11/PA12），故已释放。
 *
 *  CubeMX 里给这 6 个脚打上 User Label（LCD_RST / LCD_BLK / LCD_CS /
 *  LCD_SCK / LCD_DC / LCD_SDA）后，main.h 会自动生成 LCD_xxx_Pin 宏；
 *  下面用 #ifndef 兜底，防止还没重新生成工程时编译不过。
 * ================================================================== */
#ifndef LCD_RST_Pin
#define LCD_RST_Pin         GPIO_PIN_5
#define LCD_RST_GPIO_Port   GPIOA
#endif
#ifndef LCD_BLK_Pin
#define LCD_BLK_Pin         GPIO_PIN_6
#define LCD_BLK_GPIO_Port   GPIOA
#endif
#ifndef LCD_CS_Pin
#define LCD_CS_Pin          GPIO_PIN_7
#define LCD_CS_GPIO_Port    GPIOA
#endif
#ifndef LCD_DC_Pin
#define LCD_DC_Pin          GPIO_PIN_14
#define LCD_DC_GPIO_Port    GPIOB
#endif

/* 端口层：只做“引脚 + SPI”的搬运，不关心屏幕型号 */
void LCD_PortInit(void);
void LCD_Delay(uint32_t ms);

void LCD_RstPin(uint8_t level);
void LCD_BlPin(uint8_t level);
void LCD_CsPin(uint8_t level);
void LCD_DcPin(uint8_t level);

void LCD_WriteCmd(uint8_t cmd);
void LCD_WriteData(uint8_t data);
void LCD_WriteDataHalfWord(uint16_t data);
void LCD_WriteDataBulk(const uint8_t *buf, uint32_t len);

#endif
