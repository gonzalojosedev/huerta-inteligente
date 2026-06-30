#include "lcd_i2c.h"

#define LCD_ADDR (0x27 << 1)

static I2C_HandleTypeDef *lcd_i2c;

static void LCD_SendInternal(uint8_t data, uint8_t flags)
{
    uint8_t high = data & 0xF0;
    uint8_t low  = (data << 4) & 0xF0;

    uint8_t data_arr[4];

    data_arr[0] = high | flags | 0x0C;
    data_arr[1] = high | flags | 0x08;
    data_arr[2] = low  | flags | 0x0C;
    data_arr[3] = low  | flags | 0x08;

    HAL_I2C_Master_Transmit(lcd_i2c, LCD_ADDR, data_arr, 4, 100);
    HAL_Delay(1);
}

static void LCD_SendCmd(uint8_t cmd)
{
    LCD_SendInternal(cmd, 0x00);
}

static void LCD_SendData(uint8_t data)
{
    LCD_SendInternal(data, 0x01);
}

void LCD_Init(I2C_HandleTypeDef *hi2c)
{
    lcd_i2c = hi2c;

    HAL_Delay(50);

    LCD_SendCmd(0x33);
    LCD_SendCmd(0x32);
    LCD_SendCmd(0x28);
    LCD_SendCmd(0x0C);
    LCD_SendCmd(0x06);
    LCD_SendCmd(0x01);

    HAL_Delay(5);
}

void LCD_Clear(void)
{
    LCD_SendCmd(0x01);
    HAL_Delay(2);
}

void LCD_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t address;

    if (row == 0)
    {
        address = 0x80 + col;
    }
    else
    {
        address = 0xC0 + col;
    }

    LCD_SendCmd(address);
}

void LCD_Print(char *str)
{
    while (*str)
    {
        LCD_SendData((uint8_t)*str);
        str++;
    }
}
