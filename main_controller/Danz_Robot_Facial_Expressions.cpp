#define F_CPU 16000000UL   // CPU frequency (Arduino Uno = 16 MHz)
#include "Danz_Robot_Facial_Expressions.h"
#include "Danz_OLED.h"    // Expressions: based on OLED
#include <util/delay.h>


//--------[Start: Objects]-----
Danz_OLED display;
//--------[End: Objects]-------


//--------[Start: Constants]-----
// Face emotion definitions
#define FACE_SURPRISED 0   // surprised: pupils at top, mouth as filled circle
#define FACE_HAPPY     1   // happy: pupils centered, mouth as horizontal line
#define FACE_SAD       2   // sad: pupils at bottom, mouth as bottom line
//--------[End: Constants]-------


//--------[Start: constructor]-----
Danz_Robot_Facial_Expressions::Danz_Robot_Facial_Expressions()
{
}
//--------[End: constructor]-------


//--------[Start: Functions Definition]-------
void Danz_Robot_Facial_Expressions::drawSingleEye(int cx, int cy, int pupilY_Offset)
{
    // Eye outline
    display.OLED_draw_circle(cx, cy, 14);

    // safety clamp (prevents pupil leaving eye)
    if (pupilY_Offset > 5) 
    {
        pupilY_Offset = 5;
    }
    if (pupilY_Offset < -5) 
    {
        pupilY_Offset = -5;
    }

    // Pupil
    display.OLED_draw_filled_circle(cx, cy + pupilY_Offset, 4);
}

void Danz_Robot_Facial_Expressions::drawEyes(int mood)
{
    int leftEyeX = 40;
    int rightEyeX = 88;
    int eyeY = 24;

    int pupilOffset_Y = 0; // default: center

    // Mood-based pupil vertical position
    switch(mood)
    {
        case FACE_HAPPY:      
            pupilOffset_Y = 0;  
            break; // center
        case FACE_SURPRISED:  
            pupilOffset_Y = -6; 
            break; // top
        case FACE_SAD:        
            pupilOffset_Y = 6;  
            break; // bottom
        default:              
            pupilOffset_Y = 0;  
            break;
    }

    drawSingleEye(leftEyeX, eyeY, pupilOffset_Y);
    drawSingleEye(rightEyeX, eyeY, pupilOffset_Y);
}

void Danz_Robot_Facial_Expressions::drawMouth(int mood)
{
    int mouthX = 64;
    int mouthY = 48;

    switch(mood)
    {
        case FACE_HAPPY:
            display.OLED_draw_line(mouthX - 12, mouthY, mouthX + 12, mouthY); // horizontal line
            break;
        case FACE_SURPRISED:
            display.OLED_draw_filled_circle(mouthX, mouthY, 8); // filled circle
            break;
        case FACE_SAD:
            display.OLED_draw_line(mouthX - 12, mouthY + 4, mouthX + 12, mouthY + 4); // bottom line
            break;
        default:
            display.OLED_draw_line(mouthX - 12, mouthY, mouthX + 12, mouthY);
            break;
    }
}

void Danz_Robot_Facial_Expressions::drawFace(int mood)
{
    display.OLED_clearBuffer();   // clear screen

    drawEyes(mood);       // draw both eyes
    drawMouth(mood);      // draw mouth

    display.OLED_update();        // send buffer to OLED
}
//--------[End: Functions Definition]-------