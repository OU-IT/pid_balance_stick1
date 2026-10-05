#ifndef __MPU6050_H
#define __MPU6050_H
#include "stm32f4xx.h"

typedef struct mpu6050_data{
		int16_t accel_x;
		int16_t accel_y;
		int16_t accel_z;
		int16_t temp;
		int16_t gyro_x;
		int16_t gyro_y;
		int16_t gyro_z;
}mpu6050_data;
void reg_write(uint8_t reg,uint8_t value);
uint8_t reg_read(uint8_t reg);
void my_mpu6050_init();
mpu6050_data mpu6050_get_raw_data();
float get_accel_data(int16_t raw_accel_data);
float get_angle_x(float gx,float gz);
float get_angle_y(float gy,float gz);
float gyro_raw_to_angle_p_s(int16_t raw);
float calibrate_gyro_bias(uint16_t samples);
#endif
