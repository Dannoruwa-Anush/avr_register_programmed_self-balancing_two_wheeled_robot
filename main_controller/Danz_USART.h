#ifndef Danz_USART_H
#define Danz_USART_H

#include <Arduino.h>
#include <avr/io.h>

class Danz_USART
{
  public:
	//--------[Start: constructor]-----
    Danz_USART();
	//--------[End: constructor]-------
	
	//--------[Start: Function Declarations]----------
	// Serial (USART) Communication
	void USART_Init(uint16_t ubrr_value);
	char USART_Receive();
	void USART_Transmit(char data);
	void USART_SendString(const char *str);
	//--------[End: Function Declarations]------------
};

#endif