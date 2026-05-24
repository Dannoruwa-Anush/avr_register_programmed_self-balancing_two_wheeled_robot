#ifndef Danz_Timer_H
#define Danz_Timer_H

#include <Arduino.h>
#include <avr/io.h>

class Danz_Timer
{
  public:
	//--------[Start: constructor]-----
    Danz_Timer();
	//--------[End: constructor]-------
	
	//--------[Start: Function Declarations]----------
	// Timer : generate a periodic interrupt
	void Timer2_Init(void);
	//--------[End: Function Declarations]------------
};

#endif