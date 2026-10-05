#include "delay.h"

//volatile uint32_t uwTick = 0;

//void SysTick_Handler(void)
//{
//    uwTick++;
//}

//void delay_init(void)
//{
//    // 配置 SysTick 每 1ms 中断一次，优先级设为最低
//    SysTick_Config(SystemCoreClock / 1000);
//    NVIC_SetPriority(SysTick_IRQn, 15);
//}

//void delay_ms(uint32_t ms)
//{
//    uint32_t start = uwTick;
//    while ((uwTick - start) < ms);
//}

void delay(int t){
		while(t > 0){t--;}
}
