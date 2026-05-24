#define F_CPU 16000000UL   // CPU frequency (Arduino Uno = 16 MHz)
#include "Danz_USART.h"
#include <util/delay.h>

//--------[Start: constructor]-----
Danz_USART::Danz_USART()
{
}
//--------[End: constructor]-------


//--------[Start: Functions Definition]-------
// Serial (USART) Communication
void Danz_USART::USART_Init(uint16_t ubrr_value){
  // Set baud rate
  UBRR0H = (unsigned char)(ubrr_value >> 8);
  UBRR0L = (unsigned char)ubrr_value;

  // Enable RX complete interrupt, RX & TX
  UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);

  // Set frame format: 8 data bits, 1 stop bit
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

char Danz_USART::USART_Receive(){
  // Return received data
  return UDR0;
}

// Transmit bit by bit
void Danz_USART::USART_Transmit(char data)
{
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

// Send string (set of bits)
void Danz_USART::USART_SendString(const char *str)
{
    while (*str)
    {
        USART_Transmit(*str++);
    }
}
//--------[End: Functions Definition]-------