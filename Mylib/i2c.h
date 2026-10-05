#ifndef __I2C_H
#define __I2C_H
#endif
#include "stm32f4xx.h"

void I2C1_INIT();
int I2C_SEND_BYTES(I2C_TypeDef* I2Cx,uint8_t addr,uint8_t* Data,int length);
int I2C_RECEIVE_BYTES(I2C_TypeDef* I2Cx,uint8_t addr,uint8_t* pBuffer,int length);
int I2C_ReadBytes(I2C_TypeDef* I2Cx, uint8_t dev_addr, uint8_t reg_addr, uint8_t* pBuffer, int length);
