#ifndef Danz_L298NDRIVER_H
#define Danz_L298NDRIVER_H

#include <Arduino.h>
#include <avr/io.h>

class Danz_L298NDriver
{
  public:
	//--------[Start: constructor]-----
    Danz_L298NDriver();
	//--------[End: constructor]-------
	
	//--------[Start: Function Declarations]----------
    void motorDriverInit();
	void moveForward(uint8_t speed);
	void moveBackward(uint8_t speed);
	void moveLeftRightAlongCurve(uint8_t leftSpeed, uint8_t rightSpeed);
    void moveStop();
	//--------[End: Function Declarations]------------

  private:
    void pwmInit();
};

#endif