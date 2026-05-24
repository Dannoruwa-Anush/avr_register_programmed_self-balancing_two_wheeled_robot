#ifndef Danz_I2C_H
#define Danz_I2C_H

#include <Arduino.h>
#include <avr/io.h>

class Danz_I2C
{
  public:
	//--------[Start: constructor]-----
    Danz_I2C();
	//--------[End: constructor]-------
	
	//--------[Start: Function Declarations]----------
	//TWI (Two wire communication (i2c))
	void TWI_Init();
	void TWI_Start();
	void TWI_Stop();
	void TWI_Write(uint8_t data);
	uint8_t TWI_Read_Ack();
	uint8_t TWI_Read_Nack();
	//--------[End: Function Declarations]------------
};

#endif