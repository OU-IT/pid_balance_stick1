#ifndef __KALMAN_H
#define __KALMAN_H
#include "stm32f4xx.h"
typedef struct kalman_filter_struct{
		float P_prev;
		float Q;
		float R;
		float X_prev;
		float gyro_data;
		float gyro_bias;
		float accel_data;
		float dt;
}kalman_filter_struct;
float predict(float prev_angle, float gyro_data, float gyro_bias, float dt);
float kalman_filter(kalman_filter_struct* pStruct);

#endif