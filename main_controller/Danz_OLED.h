#ifndef Danz_OLED_H
#define DDanz_OLED_H

#include <Arduino.h>
#include <avr/io.h>

class Danz_OLED
{
  public:
	//--------[Start: constructor]-----
    Danz_OLED();
	//--------[End: constructor]-------
	
	//--------[Start: Function Declarations]----------
	void OLED_command(uint8_t cmd);
	void OLED_data(uint8_t data);
	void OLED_init();
	void OLED_setCursor(uint8_t page, uint8_t column);
	void OLED_update();
	void OLED_clearBuffer();
	void OLED_drawPixel(int x, int y, uint8_t color);
	void OLED_draw_line(int x0, int y0, int x1, int y1);
	void OLED_draw_circle(int x0, int y0, int r);
	void OLED_draw_filled_circle(int x0, int y0, int r);
	//--------[End: Function Declarations]------------
};

#endif