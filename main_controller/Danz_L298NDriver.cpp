#define F_CPU 16000000UL   // CPU frequency (Arduino Uno = 16 MHz)
#include "Danz_L298NDriver.h"
#include <util/delay.h>

//--------[Start: Constants]-----
// L298N Pins

// Motor-L
#define enA PB2 // digital pin: 10 (Timer1 - OC1B - PWM)
#define in1 PB4 // digital pin 12
#define in2 PB5 // digital pin 13

// Motor-R 
#define enB PB1 // digital pin: 9 (Timer1 - OC1A - PWM)
#define in3 PB0 // digital pin 8
#define in4 PD7 // digital pin 7
//--------[End: Constants]-------


//--------[Start: constructor]-----
Danz_L298NDriver::Danz_L298NDriver()
{
}
//--------[End: constructor]-------


//--------[Start: Functions Definition]-------
void Danz_L298NDriver::motorDriverInit()
{
  // Direction pins as output
  DDRB |= (1 << in1) | (1 << in2) | (1 << in3);
  DDRD |= (1 << in4);

  // PWM setup
  pwmInit();
}


void Danz_L298NDriver::pwmInit()
{
  // Set PWM pins as output
  DDRB |= (1 << enA) | (1 << enB);

  // Fast PWM 8-bit, non-inverting
  TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM10);
  TCCR1B = (1 << WGM12) | (1 << CS11); // prescaler = 8

  // Start with motors off
  OCR1A = 0; // Motor - R
  OCR1B = 0; // Motor - L
}


// Move forward
void Danz_L298NDriver::moveForward(uint8_t speed)
{
  // Motor - L
  OCR1B = speed;
  PORTB |= (1 << in1);
  PORTB &= ~(1 << in2);

  // Motor - R
  OCR1A = speed;
  PORTB |= (1 << in3);
  PORTD &= ~(1 << in4);
}


// Move backward
void Danz_L298NDriver::moveBackward(uint8_t speed)
{
  // Motor - L
  OCR1B = speed;
  PORTB &= ~(1 << in1);
  PORTB |= (1 << in2);

  // Motor - R
  OCR1A = speed;
  PORTB &= ~(1 << in3);
  PORTD |= (1 << in4);
}


// Turn (curve)
void Danz_L298NDriver::moveLeftRightAlongCurve(uint8_t leftSpeed, uint8_t rightSpeed)
{
  // Motor - L
  OCR1B = leftSpeed;
  PORTB |= (1 << in1);
  PORTB &= ~(1 << in2);

  // Motor - R
  OCR1A = rightSpeed;
  PORTB |= (1 << in3);
  PORTD &= ~(1 << in4);
}


// Stop motors
void Danz_L298NDriver::moveStop()
{
  // Motor - L
  OCR1B = 0; 
  PORTB &= ~(1 << in1);
  PORTB &= ~(1 << in2);

  // Motor - R
  OCR1A = 0; 
  PORTB &= ~(1 << in3);
  PORTD &= ~(1 << in4);
}
//--------[End: Functions Definition]-------