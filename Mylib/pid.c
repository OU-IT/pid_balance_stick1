#include "stm32f4xx.h"
float pid(volatile pid_struct* p_s){
		p_s->err = p_s->expect - p_s->now_value;
		float p_t = p_s->K_P * p_s->err;
		float i_t = p_s->K_I * p_s->err + p_s->i;
		float d_t = -p_s->K_D * p_s->cur_speed;
		float out = p_t + i_t + d_t;
	  if (out > p_s->out_max) out = p_s->out_max;
    if (out < p_s->out_min) out = p_s->out_min;
		return out;
}
