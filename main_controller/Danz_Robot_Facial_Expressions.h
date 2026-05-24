#ifndef Danz_Robot_Facial_Expressions_H
#define Danz_Robot_Facial_Expressions_H

#include <Arduino.h>
#include <avr/io.h>

class Danz_Robot_Facial_Expressions
{
  public:
	//--------[Start: constructor]-----
    Danz_Robot_Facial_Expressions();
	//--------[End: constructor]-------
	
	//--------[Start: Function Declarations]----------
	void drawSingleEye(int cx, int cy, int pupilY_Offset);
	void drawEyes(int state);
	void drawMouth(int state);
	void drawFace(int mood);
	//--------[End: Function Declarations]------------
};

#endif