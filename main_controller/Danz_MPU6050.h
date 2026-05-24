#ifndef Danz_MPU6050_H
#define Danz_MPU6050_H

#include <Arduino.h>
#include <avr/io.h>

class Danz_MPU6050
{
  public:
	//--------[Start: constructor]-----
    Danz_MPU6050();
	//--------[End: constructor]-------
	
	//--------[Start: Function Declarations]----------
	void MPU_Write(uint8_t reg, uint8_t data);
	void MPU_Read_Accel(int16_t *ax, int16_t *ay, int16_t *az);
	void MPU_Read_Gyro(int16_t *gx, int16_t *gy, int16_t *gz);
	void MPU_Init();
	void MPU_Calibrate(int16_t *ax, int16_t *ay, int16_t *az, int16_t *gx, int16_t *gy, int16_t *gz);
	float Get_Tilt_Angle(float ax, float az);
	float Get_Gyro_Rate(float gx);
	//--------[End: Function Declarations]------------
};

#endif