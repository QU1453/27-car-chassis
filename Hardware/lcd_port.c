#include "lcd_port.h"

/* SPI2 由 CubeMX 生成（spi.c 里的 hspi2），这里只引用不重定义 */

static void lcd_spi_tx(const uint8_t *buf, uint32_t len)
{
    uint16_t n;

    /* HAL 单次最多 0xFFFF 字节，超长分段发送 */
    while (len > 0U) {
        n = (len > 0xFFFFU) ? 0xFFFFU : (uint16_t)len;
        (void)HAL_SPI_Transmit(&hspi2, (uint8_t *)buf, n, 1000U);
        buf += n;
        len -= n;
    }
}

void LCD_PortInit(void)
{
    /* 片选常低（与例程一致），复位/背光电平由 LCD_Init 的时序控制 */
    LCD_CsPin(0);
    LCD_DcPin(1);
}

void LCD_Delay(uint32_t ms)
{
    HAL_Delay(ms);
}

void LCD_RstPin(uint8_t level)
{
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, (level != 0U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void LCD_BlPin(uint8_t level)
{
    HAL_GPIO_WritePin(LCD_BLK_GPIO_Port, LCD_BLK_Pin, (level != 0U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void LCD_CsPin(uint8_t level)
{
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, (level != 0U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void LCD_DcPin(uint8_t level)
{
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, (level != 0U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/* 写命令：DC=0 */
void LCD_WriteCmd(uint8_t cmd)
{
    LCD_DcPin(0);
    lcd_spi_tx(&cmd, 1U);
}

/* 写 8bit 数据：DC=1 */
void LCD_WriteData(uint8_t data)
{
    LCD_DcPin(1);
    lcd_spi_tx(&data, 1U);
}

/* 写 16bit 数据（颜色/坐标）：屏幕要求高字节在前，而 HAL 按内存顺序发，
   故先交换字节；配合上层不再重复交换 */
void LCD_WriteDataHalfWord(uint16_t data)
{
    uint16_t tmp = (uint16_t)((data << 8) | (data >> 8));

    LCD_DcPin(1);
    lcd_spi_tx((const uint8_t *)&tmp, 2U);
}

/* 批量写数据：调用前 buf 里的字节顺序必须已经是“高字节在前” */
void LCD_WriteDataBulk(const uint8_t *buf, uint32_t len)
{
    LCD_DcPin(1);
    lcd_spi_tx(buf, len);
}
