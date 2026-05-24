#define F_CPU 16000000UL   // CPU frequency (Arduino Uno = 16 MHz)
#include "Danz_Timer.h"

//--------[Start: constructor]-----
Danz_Timer::Danz_Timer()
{
}
//--------[End: constructor]-------


//--------[Start: Functions Definition]-------
// Timer : generate a periodic interrupt
void Danz_Timer::Timer2_Init(void)
{
    // CTC (Clear Timer on Compare Match) mode
    TCCR2A = (1 << WGM21);

    // Prescaler 1024
    TCCR2B = (1 << CS22) |(1 << CS21) |(1 << CS20);

    OCR2A = 77; // ( 16 MHz/( (1/5 ms) * 1024 ) ) - 1 

    // Enable interrupt (interrupt occurs every 5 ms)
    TIMSK2 = (1 << OCIE2A);
}
//--------[End: Functions Definition]-------