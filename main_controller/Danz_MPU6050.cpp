#define F_CPU 16000000UL   // CPU frequency (Arduino Uno = 16 MHz)
#include "Danz_MPU6050.h"
#include "Danz_I2C.h"	   // MPU6050: based on i2c
#include <util/delay.h>

//--------[Start: Objects]-----
Danz_I2C twi_MPU6050;
//--------[End: Objects]-------


//--------[Start: Constants]-----
#define PI 3.14159265

#define MPU_ADDR 0x68   // 7-bit address (assigned by the hardware design of the device's controller chip)
//--------[End: Constants]-----


//--------[Start: Global Variables]-------------------        
// Calibration offsets
int16_t ax_offset = 0, ay_offset = 0, az_offset = 0;
int16_t gx_offset = 0, gy_offset = 0, gz_offset = 0;
//--------[End: Global Variables]---------------------


//--------[Start: constructor]-----
Danz_MPU6050::Danz_MPU6050()
{
}
//--------[End: constructor]-------


//--------[Start: Functions Definition]-------
void Danz_MPU6050::MPU_Write(uint8_t reg, uint8_t data)
{
    twi_MPU6050.TWI_Start();               // Start I2C transaction

    twi_MPU6050.TWI_Write(MPU_ADDR << 1); // I2C 7-bit slave (MPU6050) address | I2C R/W  bit (R/W = 0 : write)
    twi_MPU6050.TWI_Write(reg);           // I2C data frame 1 : MPU6050 register address
    twi_MPU6050.TWI_Write(data);          // I2C data frame 2 : MPU6050 data

    twi_MPU6050.TWI_Stop();                // End I2C transaction
}

void Danz_MPU6050::MPU_Read_Accel(int16_t *ax, int16_t *ay, int16_t *az)
{
    // The MPU6050 sends 6 bytes continuously in register order
    // |ACCEL_XOUT_H  | ACCEL_XOUT_L  | ACCEL_YOUT_H  | ACCEL_YOUT_L  | ACCEL_ZOUT_H  | ACCEL_ZOUT_L  |
    uint8_t data[6];

    twi_MPU6050.TWI_Start();                      // Start I2C transaction (write)
    
    twi_MPU6050.TWI_Write(MPU_ADDR << 1);         // I2C 7-bit slave (MPU6050) address | I2C R/W  bit (R/W = 0 : write)
    twi_MPU6050.TWI_Write(0x3B);                  // I2C data frame 1 : MPU6050 accelerometer register address

    twi_MPU6050.TWI_Start();                     // Start I2C transaction (read)
    twi_MPU6050.TWI_Write((MPU_ADDR << 1) | 1);  // I2C 7-bit slave (MPU6050) address | I2C R/W  bit (R/W = 1 : Read)

    for (uint8_t i = 0; i < 5; i++)
        data[i] = twi_MPU6050.TWI_Read_Ack();

    data[5] = twi_MPU6050.TWI_Read_Nack();
    
    twi_MPU6050.TWI_Stop();                      // End I2C transaction

    // Each axis (X, Y, Z) is stored as two bytes: H byte and L byte
    // Combine each two 8-bit bytes into one 16-bit value
    *ax = (int16_t)(data[0] << 8 | data[1]);
    *ay = (int16_t)(data[2] << 8 | data[3]);
    *az = (int16_t)(data[4] << 8 | data[5]);

    // Apply calibration offsets
    *ax -= ax_offset;
    *ay -= ay_offset;
    *az -= az_offset;
}

void Danz_MPU6050::MPU_Read_Gyro(int16_t *gx, int16_t *gy, int16_t *gz)
{
    // The MPU6050 sends 6 bytes continuously in register order
    // |GYRO_XOUT_H  | GYRO_XOUT_L  | GYRO_YOUT_H  | GYRO_YOUT_L  | GYRO_ZOUT_H  | GYRO_ZOUT_L  |
    uint8_t data[6];

    twi_MPU6050.TWI_Start();                        // Start I2C transaction (write)
    
    twi_MPU6050.TWI_Write(MPU_ADDR << 1);           // I2C 7-bit slave (MPU6050) address | I2C R/W  bit (R/W = 0 : write)
    twi_MPU6050.TWI_Write(0x43);                    // I2C data frame 1 : MPU6050 gyroscope register address

    twi_MPU6050.TWI_Start();                        // Start I2C transaction (read)
    twi_MPU6050.TWI_Write((MPU_ADDR << 1) | 1);     // I2C 7-bit slave (MPU6050) address | I2C R/W  bit (R/W = 1 : Read)

    for (uint8_t i = 0; i < 5; i++)
        data[i] = twi_MPU6050.TWI_Read_Ack();

    data[5] = twi_MPU6050.TWI_Read_Nack();

    twi_MPU6050.TWI_Stop();                         // End I2C transaction

    // Each axis (X, Y, Z) is stored as two bytes: H byte and L byte
    // Combine each two 8-bit bytes into one 16-bit value
    *gx = (int16_t)(data[0] << 8 | data[1]);
    *gy = (int16_t)(data[2] << 8 | data[3]);
    *gz = (int16_t)(data[4] << 8 | data[5]);

    // Apply calibration offsets
    *gx -= gx_offset;
    *gy -= gy_offset;
    *gz -= gz_offset;
}

void Danz_MPU6050::MPU_Init(){
  // Wake up MPU6050 (exit sleep mode)
  MPU_Write(0x6B, 0x00);  // 0x00: config. Power Mangement 1 register of MPU6050
  _delay_ms(100);

  // Sample Rate Divider
  MPU_Write(0x19, 0x07);
  _delay_ms(100);

  // Digital Low Pass Filter
  MPU_Write(0x1A, 0x03);
  _delay_ms(100);

  // Gyroscope Full Scale Range
  MPU_Write(0x1B, 0x00);
  _delay_ms(100);

  // Accelerometer Full Scale Range
  MPU_Write(0x1C, 0x00);  
  _delay_ms(100);
}

void Danz_MPU6050::MPU_Calibrate(int16_t *ax, int16_t *ay, int16_t *az, int16_t *gx, int16_t *gy, int16_t *gz){
  int32_t ax_sum = 0, ay_sum = 0, az_sum = 0;
  int32_t gx_sum = 0, gy_sum = 0, gz_sum = 0;

  const uint16_t samples = 1000;

  for (uint16_t i = 0; i < samples; i++) {
    MPU_Read_Accel(ax, ay, az);
    MPU_Read_Gyro(gx, gy, gz);

    ax_sum += *ax;
    ay_sum += *ay;
    az_sum += *az;

    gx_sum += *gx;
    gy_sum += *gy;
    gz_sum += *gz;

    _delay_ms(2);
  }

  // Average offsets
  ax_offset = ax_sum / samples;
  ay_offset = ay_sum / samples;

  // Remove gravity from Z axis
  az_offset = (az_sum / samples) - 16384;

  gx_offset = gx_sum / samples;
  gy_offset = gy_sum / samples;
  gz_offset = gz_sum / samples;
}

float Danz_MPU6050::Get_Tilt_Angle(float ax, float az){
    return atan2(ax, az) * 180.0 / PI;
}

float Danz_MPU6050::Get_Gyro_Rate(float gx){
    return gx / 131.0; // 131: sensitivity scale factor for the default range
}
//--------[End: Functions Definition]-------