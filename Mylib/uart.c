#include <string.h>
#include "stm32f4xx.h"
#include <stdarg.h>

// 发送一个字符串
void USART_SendString(const char *str , USART_TypeDef* USARTx) {
    while (*str) {
        // 等待发送数据寄存器空
        while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
        USART_SendData(USART1, *str++);
    }
}

void USART_Printf(USART_TypeDef* USARTx, const char *format, ...) {
    char buffer[128];      // 缓冲区大小可根据需求调整
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // 发送缓冲区内容
    char *p = buffer;
    while (*p) {
        while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
        USART_SendData(USARTx, *p++);
    }
    // 可选：等待发送完成
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
}


void USART1_Init(){
    // ========== 1. 开启时钟 ==========
    // USART1 挂接在 APB2 总线上，GPIOA 挂接在 AHB1 总线上
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    // ========== 2. 配置引脚为复用功能 ==========
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // PA9 (TX) 和 PA10 (RX) 一起配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;      // 复用模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    // 推挽输出（TX需要）
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      // 上拉，保证空闲状态为高
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 将 PA9 和 PA10 连接到 USART1 的硬件通道上
    // 对于 F4，复用功能编号：USART1 对应 AF7
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

    // ========== 3. 配置 UART 参数 ==========
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;              // 波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 8位数据
    USART_InitStructure.USART_StopBits = USART_StopBits_1;      // 1个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;         // 无校验
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发模式
    USART_Init(USART1, &USART_InitStructure);

    // 使能 USART1
    USART_Cmd(USART1, ENABLE);

}

uint16_t MY_USART_ReceiveData(USART_TypeDef* USARTx){
		if(USART_GetFlagStatus(USARTx,USART_FLAG_RXNE)){
				return USART_ReceiveData(USARTx);
		}
		else{
				return 0;
		}
}
