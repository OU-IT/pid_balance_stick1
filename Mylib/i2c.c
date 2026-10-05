#include "stm32f4xx_i2c.h"

void I2C1_INIT(void) {
    // 时钟使能
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    // GPIO 配置（复用开漏，上拉）
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 复用功能映射（AF4）
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);

    // I2C 配置
    I2C_InitTypeDef I2C_InitStruct;
    I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;       // 占空比
    I2C_InitStruct.I2C_ClockSpeed = 400000;               // 400kHz
    I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;              // 使能应答（接收时自动应答）
    I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // 7位地址
    I2C_Init(I2C1, &I2C_InitStruct);

    I2C_Cmd(I2C1, ENABLE);   // 使能外设
}
int I2C_SEND_BYTES(I2C_TypeDef* I2Cx,uint8_t addr,uint8_t* Data,int length){
		//1、等待总线空闲
		while(I2C_GetFlagStatus(I2Cx,I2C_FLAG_BUSY) == SET);
	
		//2、发送起始位
		I2C_GenerateSTART(I2Cx,ENABLE);
		while(I2C_GetFlagStatus(I2Cx,I2C_FLAG_SB) == RESET);    //等待起始位发完
		
		//3、发送地址
		I2C_ClearFlag(I2Cx,I2C_FLAG_AF);//清除（AF）答应失败标志位
		I2C_SendData(I2Cx,addr & 0xfe);
		while(1){
				//寻址成功
				if(I2C_GetFlagStatus(I2Cx,I2C_FLAG_ADDR) == SET){
						break;
				}
				//寻址失败
				if(I2C_GetFlagStatus(I2Cx,I2C_FLAG_AF) == SET){
						I2C_GenerateSTOP(I2Cx,ENABLE);
						return -1;
				}
		}
		//清除addr
		I2C_ReadRegister(I2Cx,I2C_Register_SR1);
		I2C_ReadRegister(I2Cx,I2C_Register_SR2);
		
		//4、发送数据
		for(int i = 0;i < length; i++){
				//等待发送数据寄存器清空
				while(1){
						if(I2C_GetFlagStatus(I2Cx,I2C_FLAG_AF) == SET){
								I2C_GenerateSTOP(I2Cx,ENABLE);
								return -2; //数据被拒收
						}
						if(I2C_GetFlagStatus(I2Cx,I2C_FLAG_TXE) == SET){
								break;
						}
				}
				I2C_SendData(I2Cx,Data[i]);
				//等待数据发送完成
				while(1){
						if(I2C_GetFlagStatus(I2Cx,I2C_FLAG_AF) == SET){
								I2C_GenerateSTOP(I2Cx,ENABLE);
								return -2; //数据被拒收
						}
						if(I2C_GetFlagStatus(I2Cx,I2C_FLAG_BTF) == SET){
								break;
						}
				}
		}
		
		//5、发送停止位
		I2C_GenerateSTOP(I2Cx,ENABLE);
		return 0;
}

int I2C_RECEIVE_BYTES(I2C_TypeDef* I2Cx,uint8_t addr,uint8_t* pBuffer,int length){
		//1、发送起始位数据
		I2C_GenerateSTART(I2Cx,ENABLE);
		while(I2C_GetFlagStatus(I2Cx,I2C_FLAG_SB) == RESET);//等待起始位数据发送完成
	
		//2、发送寻址
		I2C_ClearFlag(I2Cx,I2C_FLAG_AF);//清除AF寄存器
		I2C_SendData(I2Cx,addr | 0x01);
		while(1){
				if(I2C_GetFlagStatus(I2Cx,I2C_FLAG_AF) == SET){
					I2C_GenerateSTOP(I2Cx,ENABLE);
						return -1; //寻址失败
				}
				if(I2C_GetFlagStatus(I2Cx,I2C_FLAG_ADDR) == SET){
						break;
				}
		}
		
		//3、接收数据
		//清除ADDR
		I2C_ReadRegister(I2Cx,I2C_Register_SR1);
		I2C_ReadRegister(I2Cx,I2C_Register_SR2);
		//ACK=1
		I2C_AcknowledgeConfig(I2Cx,ENABLE);
		for(int i = 0;i < length - 1;i++){
				while(I2C_GetFlagStatus(I2Cx,I2C_FLAG_RXNE) == RESET);
				pBuffer[i] = I2C_ReceiveData(I2Cx);
		}
		//ACK=0  STOP = 1
		I2C_AcknowledgeConfig(I2Cx,DISABLE);
		I2C_GenerateSTOP(I2Cx,ENABLE);
			
		//等待接收完成
		while(I2C_GetFlagStatus(I2Cx,I2C_FLAG_RXNE) == RESET);
		//读取数据
		pBuffer[length - 1] = I2C_ReceiveData(I2Cx);
		return 0;

}

/**
  * @brief  连续读取I2C设备多个字节（带寄存器地址）
  * @param  I2Cx      I2C外设，如I2C1
  * @param  dev_addr  设备地址（7位左移一位，如MPU6050写地址0xD0）
  * @param  reg_addr  起始寄存器地址（如0x3B）
  * @param  pBuffer   存放读取数据的缓冲区
  * @param  length    读取字节数
  * @retval 0成功，负值错误
  */
int I2C_ReadBytes(I2C_TypeDef* I2Cx, uint8_t dev_addr, uint8_t reg_addr, uint8_t* pBuffer, int length) {
    // 1. 等待总线空闲
    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) == SET);

    // 2. 发送起始位
    I2C_GenerateSTART(I2Cx, ENABLE);
    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_SB) == RESET);

    // 3. 发送设备写地址（dev_addr最低位为0）
    I2C_SendData(I2Cx, dev_addr & 0xFE);
    while(1) {
        if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR) == SET) break;
        if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_AF) == SET) {
            I2C_GenerateSTOP(I2Cx, ENABLE);
            return -1; // 寻址失败
        }
    }
    // 清除ADDR
    I2C_ReadRegister(I2Cx, I2C_Register_SR1);
    I2C_ReadRegister(I2Cx, I2C_Register_SR2);

    // 4. 发送寄存器地址
    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXE) == RESET);
    I2C_SendData(I2Cx, reg_addr);
    while(1) {
        if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF) == SET) break;
        if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_AF) == SET) {
            I2C_GenerateSTOP(I2Cx, ENABLE);
            return -2; // 寄存器地址被拒
        }
    }

    // 5. 发送重复起始位
    I2C_GenerateSTART(I2Cx, ENABLE);
    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_SB) == RESET);

    // 6. 发送设备读地址（最低位为1）
    I2C_SendData(I2Cx, dev_addr | 0x01);
    while(1) {
        if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR) == SET) break;
        if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_AF) == SET) {
            I2C_GenerateSTOP(I2Cx, ENABLE);
            return -3; // 寻址失败
        }
    }
    // 清除ADDR
    I2C_ReadRegister(I2Cx, I2C_Register_SR1);
    I2C_ReadRegister(I2Cx, I2C_Register_SR2);

    // 7. 连续读取数据（前 length-1 个带ACK）
    I2C_AcknowledgeConfig(I2Cx, ENABLE);
    for(int i = 0; i < length - 1; i++) {
        while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET);
        pBuffer[i] = I2C_ReceiveData(I2Cx);
    }
    // 最后一个字节前关闭ACK，并发送停止
    I2C_AcknowledgeConfig(I2Cx, DISABLE);
    I2C_GenerateSTOP(I2Cx, ENABLE);
    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET);
    pBuffer[length - 1] = I2C_ReceiveData(I2Cx);

    return 0;
}