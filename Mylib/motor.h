#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f4xx.h"

// ---------- 用户可配置参数 ----------
// 可根据实际硬件连接修改以下宏定义
#define MOTOR_PWM_TIM          TIM2
#define MOTOR_PWM_TIM_CLK      RCC_APB1Periph_TIM2
#define MOTOR_PWM_GPIO_PORT    GPIOA
#define MOTOR_PWM_GPIO_PIN     GPIO_Pin_1
#define MOTOR_PWM_GPIO_AF      GPIO_AF_TIM2
#define MOTOR_PWM_CHANNEL      TIM_Channel_2

#define MOTOR_PWM_PERIOD       20000   // 20ms (50Hz)
#define MOTOR_PWM_PRESCALER    83      // 84MHz / (83+1) = 1MHz，每μs计数1

// ---------- 函数声明 ----------
/**
  * @brief  初始化电机控制 PWM（PA1 输出 50Hz 信号）
  */
void PWM_Init(void);

/**
  * @brief  设置电机速度（双向电调模式）
  * @param  speed: -100 ~ 100
  *         负值反转，0 停止，正值正转，绝对值越大转速越快
  */
void motor_speed(int speed);

#endif /* __MOTOR_H */