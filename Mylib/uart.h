#ifndef __UART_H
#define __UART_H
#include "stm32f4xx.h"
#include <stdarg.h>

void USART_Printf(USART_TypeDef* USARTx, const char *format, ...);
void USART_SendString(const char *str , USART_TypeDef* USARTx);
void USART1_Init();
uint16_t MY_USART_ReceiveData(USART_TypeDef* USARTx);

#endif

