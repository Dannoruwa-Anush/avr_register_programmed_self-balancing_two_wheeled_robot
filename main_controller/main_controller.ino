#include <Arduino.h>
#include <avr/interrupt.h>
#include <math.h>

#include "Danz_L298NDriver.h"
#include "Danz_USART.h"
#include "Danz_I2C.h"
#include "Danz_MPU6050.h"
#include "Danz_Timer.h"
#include "Danz_OLED.h"
#include "Danz_Robot_Facial_Expressions.h"


//--------[Start: Objects]-----
Danz_L298NDriver robot;
Danz_USART serial_comm;
Danz_I2C two_wire_comm;
Danz_MPU6050 accl_gyro;
Danz_Timer timer_5ms;
Danz_OLED oled;
Danz_Robot_Facial_Expressions robot_face;
//--------[End: Objects]-------


//--------[Start: Constants]-----
// PID 
// Control loop frequency = 200Hz
const float dt = 0.005f;

// OLED Face IDs
const uint8_t FACE_SURPRISED_AT_TILT_BACKWARD = 0;
const uint8_t FACE_HAPPY_AT_BALANCED          = 1;
const uint8_t FACE_SAD_AT_TILT_FORWARD        = 2;

// OLED refresh divider
// 200Hz / 20 = 10Hz OLED update
const uint8_t OLED_REFRESH_DIVIDER = 20;
//--------[End: Constants]-------


//--------[Start: Global Variables]-------------------   
// USART: Non-Blocking
volatile uint8_t receivedCommand = 0;
volatile uint8_t commandReady    = 0;

// Timer Scheduler
volatile uint8_t controlTicks = 0;

// MPU6050 Variables
int16_t ax, ay, az; // accel
int16_t gx, gy, gz; // gyro

// Filter Variables
float tilt_angle = 0.0f;
float gyro_rate  = 0.0f;
float angle      = 0.0f;

// PID
// Tuning PID Controller Parameters: Start with Ki = 0 and Kd = 0
float Kp = 15.0;  // Increase Kp until the system responds quickly (1.0 - 20.0)
float Ki = 5.0;  // Increase Ki to remove slow drift or steady-state error (0.0 - 5.0)
float Kd = 0.5; // Increase Kd to reduce vibration, overshoot, or fast oscillation (0.0 - 1.0)

float error          = 0.0f;
float prev_error     = 0.0f;
float integral       = 0.0f;
float control_output = 0.0f;

// Handling moment by serial communication
// Movement target
float target_angle = 0.0f;

// Auto return timeout
uint16_t command_timeout = 0;

// OLED Face
uint8_t current_face = FACE_HAPPY_AT_BALANCED;
uint8_t last_face    = 255;

uint8_t oledDividerCounter = 0;
//--------[End: Global Variables]-------------------  


//--------[Start: Functions Declarations]-----
void processCommand();
void runControlLoop();
void updateOLED();
//--------[End: Functions Declarations]-------


int main(void)
{
    robot.motorDriverInit();

    robot.moveStop();

    // Initialize USART with 9600 baud (16 MHz clock)
    serial_comm.USART_Init(103);  // UBRR = (16MHz/(16*9600))-1 = 103

    two_wire_comm.TWI_Init();

    accl_gyro.MPU_Init();

    serial_comm.USART_SendString("Keep robot still for MPU6050 calibration...\r\n");
    
    accl_gyro.MPU_Calibrate(&ax, &ay, &az, &gx, &gy, &gz);
    
    serial_comm.USART_SendString("MPU6050 Calibration Complete!\r\n");

    oled.OLED_init();

    robot_face.drawFace(FACE_HAPPY_AT_BALANCED);

    timer_5ms.Timer2_Init();

    // Enable global interrupts
    sei();

    while (1)
    {
        if (commandReady)
        {
            processCommand();
        }

        // Run scheduled control loops
        while (controlTicks > 0)
        {
            cli(); // Disable global interrupts
            controlTicks--;
            sei();

            runControlLoop();
        }

        updateOLED();
    }
}


//-------------[Start : ISR]-----------------------------
// Interrupt subroutine : TIMER2 Interrupt : gives a precise 5ms periodic trigger
ISR(TIMER2_COMPA_vect)
{
    // Prevent overflow
    if (controlTicks < 255)
    {
        controlTicks++;
    }
}

// Interrupt subroutine : USART RX Complete Interrupt
ISR(USART_RX_vect)
{
    receivedCommand = serial_comm.USART_Receive();

    // Convert lowercase to uppercase
    if (receivedCommand >= 'a' && receivedCommand <= 'z')
    {
        receivedCommand -= 32;
    }

    commandReady = 1;
}
//-------------[End : ISR]-------------------------------


//--------[Start: Functions Definition]-------
// Process Bluetooth / USART Commands
void processCommand(void)
{
    commandReady = 0;

    switch (receivedCommand)
    {
        case 'F':
            // Lean forward slightly
            target_angle = 6.0f;

            // Keep command active for 1 second
            command_timeout = 200;

            serial_comm.USART_SendString("Forward\r\n");

            break;

        case 'B':
            // Lean backward slightly
            target_angle = -6.0f;

            // Keep command active for 1 second
            command_timeout = 200;

            serial_comm.USART_SendString("Backward\r\n");

            break;

        case 'S':
            // Return upright
            target_angle = 0.0f;

            serial_comm.USART_SendString("Stop\r\n");

            break;
    }
}

// Main Control Loop
void runControlLoop(void)
{
    // Read MPU6050 data
    accl_gyro.MPU_Read_Accel(&ax, &ay, &az);
    accl_gyro.MPU_Read_Gyro(&gx, &gy, &gz);

    // Compute tilt angle using accelerometer
    tilt_angle = accl_gyro.Get_Tilt_Angle(ax, az);

    // Compute yyroscope angular velocity (deg/sec)
    gyro_rate = accl_gyro.Get_Gyro_Rate(gx);

    // Complementary filter
    angle = (0.98f * (angle + (gyro_rate * dt))) + (0.02f * tilt_angle);

    // Face Selection
    if (angle > -5.0f && angle < 5.0f)
    {
        current_face = FACE_HAPPY_AT_BALANCED;
    }
    else if (angle >= 5.0f)
    {
        current_face = FACE_SAD_AT_TILT_FORWARD;
    }
    else
    {
        current_face = FACE_SURPRISED_AT_TILT_BACKWARD;
    }

    // Command Timeout
    if (command_timeout > 0)
    {
        command_timeout--;
    }
    else
    {
        target_angle = 0.0f;
    }

    // Safety Shutoff
    if (fabs(angle) > 45.0f)
    {
        robot.moveStop();

        integral = 0.0f;

        return;
    }

    // PID error
    error = target_angle - angle;

    // Integral
    integral += (error * dt);

    // Anti-windup
    if (integral > 50.0f)
    {
        integral = 50.0f;
    }

    if (integral < -50.0f)
    {
        integral = -50.0f;
    }

    // Derivative
    float derivative = (error - prev_error) / dt;

    // PID output
    control_output = (Kp * error) + (Ki * integral) + (Kd * derivative);

    prev_error = error;

    // PWM
    int pwm = (int)fabs(control_output);

    // Clamp
    if (pwm > 255)
    {
        pwm = 255;
    }

    // Dead zone
    if (pwm < 20)
    {
        pwm = 0;
    }

    // Motor Control
    if (control_output > 0.0f)
    {
        robot.moveForward(pwm);
    }
    else
    {
        robot.moveBackward(pwm);
    }
}


// OLED Update
void updateOLED(void)
{
    oledDividerCounter++;

    // Update OLED at 10Hz only
    if (oledDividerCounter < OLED_REFRESH_DIVIDER)
    {
        return;
    }

    oledDividerCounter = 0;

    // Draw only if face changed
    if (current_face != last_face)
    {
        last_face = current_face;

        robot_face.drawFace(current_face);
    }
}
//--------[End: Functions Definition]---------