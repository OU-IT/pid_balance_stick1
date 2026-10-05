#include "motor.h"

/**
  * @brief  PWM 初始化（使用 TIM2_CH2，输出至 PA1）
  * @note  配置为 50Hz 频率，周期 20ms，计数时钟 1MHz
  */
void PWM_Init(void) {
    // 1. 使能 GPIO 和 TIM 时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(MOTOR_PWM_TIM_CLK, ENABLE);

    // 2. 配置 GPIO 为复用推挽输出
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = MOTOR_PWM_GPIO_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(MOTOR_PWM_GPIO_PORT, &GPIO_InitStruct);

    // 3. 引脚复用映射到 TIM2_CH2
    GPIO_PinAFConfig(MOTOR_PWM_GPIO_PORT, GPIO_PinSource1, MOTOR_PWM_GPIO_AF);

    // 4. 定时器时基配置
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
    TIM_TimeBaseStruct.TIM_Period = MOTOR_PWM_PERIOD - 1;
    TIM_TimeBaseStruct.TIM_Prescaler = MOTOR_PWM_PRESCALER;
    TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(MOTOR_PWM_TIM, &TIM_TimeBaseStruct);

    // 5. PWM 通道配置（使用 TIM_OC2Init）
    TIM_OCInitTypeDef TIM_OCStruct;
    // --- 新增：为结构体所有成员赋予一个安全默认值 ---
    TIM_OCStructInit(&TIM_OCStruct);
    
    TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCStruct.TIM_Pulse = 1500;          // 1.5ms 中位停止
    TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_High;

    // 调用通道2的初始化函数
    TIM_OC2Init(MOTOR_PWM_TIM, &TIM_OCStruct);

    // 6. 使能预装载和定时器
    TIM_OC2PreloadConfig(MOTOR_PWM_TIM, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(MOTOR_PWM_TIM, ENABLE);
    TIM_Cmd(MOTOR_PWM_TIM, ENABLE);
}

// motor_speed 函数保持不变
void motor_speed(int speed) {
    int duty_us;

    // 限幅
    if (speed > 100)  speed = 100;
    if (speed < -100) speed = -100;

    // 线性映射：1500 + speed * 5
    duty_us = 1500 + speed * 5;

    // 安全钳制
    if (duty_us < 1000) duty_us = 1000;
    if (duty_us > 2000) duty_us = 2000;

    // 设置比较值（TIM2_CH2）
    TIM_SetCompare2(MOTOR_PWM_TIM, duty_us);
}