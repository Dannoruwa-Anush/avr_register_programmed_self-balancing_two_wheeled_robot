#define F_CPU 16000000UL   // CPU frequency (Arduino Uno = 16 MHz)
#include "Danz_OLED.h"
#include "Danz_I2C.h"	   // MPU6050: based on i2c
#include <avr/io.h>
#include <util/delay.h>

//--------[Start: Objects]-----
Danz_I2C twi_OLED;
//--------[End: Objects]-------


//--------[Start: Constants]-----
#define OLED_ADDR 0x3C   // 7-bit address (assigned by the hardware design of the device's controller chip)
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
// Bytes in OLED = 64 Rows x 128 columns = 8192 pixels = 8192/8 = 1024 bytes
#define OLED_BUFFER_SIZE (OLED_WIDTH * OLED_HEIGHT / 8)
//--------[End: Constants]-------


//--------[Start: Global variables, arrays ...]-----
// Array : Represents byte in OLED
uint8_t OLED_Buffer[OLED_BUFFER_SIZE]; 
//--------[End: Global variables, arrays ...]-------


//--------[Start: constructor]-----
Danz_OLED::Danz_OLED()
{
}
//--------[End: constructor]-------


//--------[Start: Functions Definition]-------
//OLED
void Danz_OLED::OLED_command(uint8_t cmd)
{
    twi_OLED.TWI_Start();               // Start I2C transaction

    twi_OLED.TWI_Write(OLED_ADDR << 1); // I2C 7-bit slave (OLED) address | I2C R/W  bit
    twi_OLED.TWI_Write(0x00);           // I2C data frame 1 : Enable OLED command mode (D/C#=0, R/W#=0)
    twi_OLED.TWI_Write(cmd);            // I2C data frame 2 : OLED command

    twi_OLED.TWI_Stop();                // End I2C transaction
}

void Danz_OLED::OLED_data(uint8_t data)
{
    twi_OLED.TWI_Start();               //Start I2C transaction

    twi_OLED.TWI_Write(OLED_ADDR << 1); // I2C 7-bit slave (OLED) address | I2C R/W  bit
    twi_OLED.TWI_Write(0x40);           // I2C data frame 1 : Enable OLED data mode (D/C#=1, R/W#=0)
    twi_OLED.TWI_Write(data);           // I2C data frame 2 : OLED command

    twi_OLED.TWI_Stop();                // End I2C transaction
}

void Danz_OLED::OLED_init(void)
{
    _delay_ms(100);

    OLED_command(0xAE); // Display OFF

    OLED_command(0xD5); // Set display clock divide ratio
    OLED_command(0x80);

    OLED_command(0xA8); // Set multiplex ratio
    OLED_command(0x3F);

    OLED_command(0xD3); // Set display offset
    OLED_command(0x00);

    OLED_command(0x40); // Set display start line

    OLED_command(0x8D); // Charge pump
    OLED_command(0x14);

    OLED_command(0x20); // Memory addressing mode
    OLED_command(0x00); // Horizontal addressing mode

    OLED_command(0xA1); // Segment remap

    OLED_command(0xC8); // COM scan direction

    OLED_command(0xDA); // COM pins hardware config
    OLED_command(0x12);

    OLED_command(0x81); // Contrast
    OLED_command(0xCF);

    OLED_command(0xD9); // Pre-charge period
    OLED_command(0xF1);

    OLED_command(0xDB); // VCOMH deselect level
    OLED_command(0x40);

    OLED_command(0xA4); // Display follows RAM content

    OLED_command(0xA6); // Normal display

    OLED_command(0x21); // Set column address
    OLED_command(0);
    OLED_command(127);

    OLED_command(0x22); // Set page address
    OLED_command(0);
    OLED_command(7);

    OLED_command(0xAF); // Display ON
}

void Danz_OLED::OLED_setCursor(uint8_t page, uint8_t column)
{
    // Set Page (row block) Address
    OLED_command(0xB0 + page); // 0xB0 = Page 0

    // The SSD1306 stores the column address in TWO parts: Lower nibble (4 bits), Upper nibble (4 bits)
    // Set Lower 4 Bits of Column Address
    // 0x00 to 0x0F : SSD1306 command bases used for setting the column address
    OLED_command(0x00 + (column & 0x0F));
    // Set Upper 4 Bits of Column Address
    OLED_command(0x10 + ((column >> 4) & 0x0F));
}

void Danz_OLED::OLED_clearBuffer()
{
    for(uint16_t i = 0; i < OLED_BUFFER_SIZE; i++)
    {
        OLED_Buffer[i] = 0x00;
    }
}

void Danz_OLED::OLED_update()
{
    for(uint8_t page = 0; page < 8; page++)
    {
        OLED_setCursor(page, 0);

        twi_OLED.TWI_Start();

        twi_OLED.TWI_Write(OLED_ADDR << 1);

        // Data mode
        twi_OLED.TWI_Write(0x40);

        for(uint8_t col = 0; col < 128; col++)
        {
            twi_OLED.TWI_Write(OLED_Buffer[(page * 128) + col]);
        }

        twi_OLED.TWI_Stop();
    }
}

void Danz_OLED::OLED_drawPixel(int x, int y, uint8_t color)
{
    if(x < 0 || x >= OLED_WIDTH || y < 0 || y >= OLED_HEIGHT)
    {
        return;
    }

    uint16_t index;

    index = x + (y / 8) * OLED_WIDTH;

    if(color)
    {
        OLED_Buffer[index] |= (1 << (y % 8));
    }
    else
    {
        OLED_Buffer[index] &= ~(1 << (y % 8));
    }
}

// Bresenham Line Algorithm
void Danz_OLED::OLED_draw_line(int x0, int y0, int x1, int y1)
{
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;

    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;

    int err = dx + dy;

    while(1)
    {
        OLED_drawPixel(x0, y0, 1);

        if(x0 == x1 && y0 == y1)
        {
            break;
        }

        int e2 = 2 * err;

        if(e2 >= dy)
        {
            err += dy;
            x0 += sx;
        }

        if(e2 <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

// Midpoint Circle Algorithm
void Danz_OLED::OLED_draw_circle(int x0, int y0, int r)
{
    int x = r;
    int y = 0;
    int err = 0;

    while(x >= y)
    {
        OLED_drawPixel(x0 + x, y0 + y, 1);
        OLED_drawPixel(x0 + y, y0 + x, 1);
        OLED_drawPixel(x0 - y, y0 + x, 1);
        OLED_drawPixel(x0 - x, y0 + y, 1);

        OLED_drawPixel(x0 - x, y0 - y, 1);
        OLED_drawPixel(x0 - y, y0 - x, 1);
        OLED_drawPixel(x0 + y, y0 - x, 1);
        OLED_drawPixel(x0 + x, y0 - y, 1);

        y++;

        if(err <= 0)
        {
            err += 2 * y + 1;
        }

        if(err > 0)
        {
            x--;
            err -= 2 * x + 1;
        }
    }
}

void Danz_OLED::OLED_draw_filled_circle(int x0, int y0, int r)
{
    for (int y = -r; y <= r; y++)
    {
        for (int x = -r; x <= r; x++)
        {
            if (x*x + y*y <= r*r)
            {
                OLED_drawPixel(x0 + x, y0 + y, 1);
            }
        }
    }
}
//--------[End: Functions Definition]-------