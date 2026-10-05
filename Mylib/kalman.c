#include "kalman.h"

float predict(float prev_angle, float gyro_data, float gyro_bias, float dt)
{
    return prev_angle + (gyro_data - gyro_bias) * dt;
}

float kalman_filter(kalman_filter_struct* pStruct){
		float X_pred;
		float P_new;
		float Kalman_Gain;
		float X_new;
		float P_next;
		X_pred = predict(pStruct->X_prev, pStruct->gyro_data, pStruct->gyro_bias, pStruct->dt);
		P_new = pStruct->P_prev + pStruct->Q * pStruct->dt;
		Kalman_Gain = P_new/(P_new + pStruct->R);
		X_new = X_pred + Kalman_Gain * (pStruct->accel_data - X_pred);
		P_next = (1 - Kalman_Gain) * P_new;
		pStruct->X_prev = X_new;
		pStruct->P_prev = P_next;
		return X_new;
}