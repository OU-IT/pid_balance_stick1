#ifndef __PID_H
#define __PID_H

typedef struct pid_struct{
		float K_P;
		float K_I;
		float K_D;
		float dt;
		float out_min;
		float out_max;
		float now_value;
		float expect;
		float err;
		float cur_out;
		float cur_speed;
		float i;
}pid_struct;

float pid(volatile pid_struct* p_s);

#endif
