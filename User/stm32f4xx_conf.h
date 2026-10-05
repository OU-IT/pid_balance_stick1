#ifndef __STM32F4xx_CONF_H
#define __STM32F4xx_CONF_H
/* 包含需要的头文件*/
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include <stdio.h>
#include <math.h>
#include "i2c.h"
#include "stm32f4xx_usart.h"
#include "delay.h"
#include "mpu6050.h"
#include "uart.h"
#include "kalman.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "motor.h"
#include "pid.h"


/* 以下是一些必要的空定义，防止编译报错 */
#define assert_param(expr) ((void)0)

#endif /* __STM32F4xx_CONF_H */