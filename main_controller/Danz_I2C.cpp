#define F_CPU 16000000UL   // CPU frequency (Arduino Uno = 16 MHz)
#include "Danz_I2C.h"
#include <util/delay.h>


//--------[Start: constructor]-----
Danz_I2C::Danz_I2C()
{
}
//--------[End: constructor]-------


//--------[Start: Functions Definition]-------
//TWI (Two wire communication (i2c))
void Danz_I2C::TWI_Init()
{
    TWBR = 72;         // TWBR = ((16MHz/100KHZ) - 16)/2 = 72
    TWSR = 0x00;       // Prescaler = 1
    TWCR = (1<<TWEN);  // Enable TWI
}

void Danz_I2C::TWI_Start()
{   
    // Enable TWI and generate a START condition
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    // Waits until the START operation finishes
    while (!(TWCR & (1<<TWINT)));
}

void Danz_I2C::TWI_Stop()
{
    // Enable TWI and generate a STOP condition
    TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
}

void Danz_I2C::TWI_Write(uint8_t data)
{
    TWDR = data;
    // Enable TWI and do TRANSMIT operation
    TWCR = (1<<TWINT)|(1<<TWEN);
    // Waits until the TRANSMIT operation finishes
    while (!(TWCR & (1<<TWINT)));
}

uint8_t Danz_I2C::TWI_Read_Ack()
{
    // Enable TWI reception and acknowledge (ACK) received byte
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
    // Waits until the RECEIVE operation finishes
    while (!(TWCR & (1<<TWINT)));
    return TWDR;
}

uint8_t Danz_I2C::TWI_Read_Nack()
{
    // Enable TWI reception and return NACK after receiving byte
    TWCR = (1<<TWINT)|(1<<TWEN);
    // Waits until the RECEIVE operation finishes
    while (!(TWCR & (1<<TWINT)));
    return TWDR;
}
//--------[End: Functions Definition]-------