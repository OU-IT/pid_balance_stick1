#include "stm32f4xx.h"

void reg_write(uint8_t reg,uint8_t value){
		uint8_t data[2] = {reg,value};
		I2C_SEND_BYTES(I2C1,0xd0,data,2);
}

uint8_t reg_read(uint8_t reg){
		I2C_SEND_BYTES(I2C1,0xd0,&reg,1);
	  uint8_t buffer;
		I2C_RECEIVE_BYTES(I2C1,0xd0,&buffer,1);
		return buffer;
}
void my_mpu6050_init(){
		//delay_init();
		I2C1_INIT();   //pb6->SCL    pb7->SDA
		reg_write(0x6b,0x80);  //复位
		delay(100000);
		reg_write(0x6b,0x00);  //关闭睡眠模式
		//reg_write(0x1A, 0x03);  // DLPF_CFG=3，带宽约 44Hz，可抑制部分高频振动
		//设置量程
		reg_write(0x1b,0x18);  //陀螺仪量程+-2000°/s
		reg_write(0x1c,0x00);  //加速度计量程+-2g				
}

mpu6050_data mpu6050_get_raw_data(){
		mpu6050_data data;
		uint8_t accel_x_h = reg_read(0x3b);
		uint8_t accel_x_l = reg_read(0x3c);
		uint8_t accel_y_h = reg_read(0x3d);
		uint8_t accel_y_l = reg_read(0x3e);
		uint8_t accel_z_h = reg_read(0x3f);
		uint8_t accel_z_l = reg_read(0x40);
		uint8_t temp_h = reg_read(0x41);
		uint8_t temp_l = reg_read(0x42);
		uint8_t gyro_x_h = reg_read(0x43);
		uint8_t gyro_x_l = reg_read(0x44);
		uint8_t gyro_y_h = reg_read(0x45);
		uint8_t gyro_y_l = reg_read(0x46);
		uint8_t gyro_z_h = reg_read(0x47);
		uint8_t gyro_z_l = reg_read(0x48);
		data.accel_x = (accel_x_h<<8) + accel_x_l;
	  data.accel_y = (accel_y_h<<8) + accel_y_l;
		data.accel_z = (accel_z_h<<8) + accel_z_l;
		data.gyro_x = (gyro_x_h<<8) + gyro_x_l;
		data.gyro_y = (gyro_y_h<<8) + gyro_y_l;
		data.gyro_z = (gyro_z_h<<8) + gyro_z_l;
		data.temp = (temp_h<<8) + temp_l;
		return data;
}

//mpu6050_data mpu6050_get_raw_data() {
//    static mpu6050_data last_valid = {0};  // 静态变量保存上次成功数据
//    mpu6050_data data;
//    uint8_t buf[14];

//    if (I2C_ReadBytes(I2C1, 0xD0, 0x3B, buf, 14) != 0) {
//        // 读取出错，直接返回上次有效的值，绝不让垃圾值污染系统！
//        return last_valid; 
//    }

//    data.accel_x = (int16_t)((buf[0] << 8) | buf[1]);
//    data.accel_y = (int16_t)((buf[2] << 8) | buf[3]);
//    data.accel_z = (int16_t)((buf[4] << 8) | buf[5]);
//    data.temp    = (int16_t)((buf[6] << 8) | buf[7]);
//    data.gyro_x  = (int16_t)((buf[8] << 8) | buf[9]);
//    data.gyro_y  = (int16_t)((buf[10] << 8) | buf[11]);
//    data.gyro_z  = (int16_t)((buf[12] << 8) | buf[13]);

//    last_valid = data;  // 更新缓存
//    return data;
//}

float get_accel_data(int16_t raw_accel_data){
		float data = (float)raw_accel_data * 6.1036e-5f;
		return data;
}
float get_angle_x(float gy,float gz){
	   if (fabsf(gy) < 1e-6f && fabsf(gz) < 1e-6f) {
        return 0.0f;
    }
		float angle = (float)atan2(gy, gz) * 180 / 3.141592653589793f;
		return angle;
}
float grt_angle_y(float gx,float gz){
	   if (fabsf(gx) < 1e-6f && fabsf(gz) < 1e-6f) {
        return 0.0f;
    }
		float angle = (float)atan2(gx, gz) * 180 / 3.141592653589793f;
		return angle;
}

float gyro_raw_to_angle_p_s(int16_t raw) { 
		return (float)raw * 6.1036e-2f; 
}

float calibrate_gyro_bias(uint16_t samples)
{
    int32_t sum = 0;
    for (uint16_t i = 0; i < samples; i++) {
        mpu6050_data raw = mpu6050_get_raw_data();
        sum += raw.gyro_x;
        delay(7000);
    }
    return gyro_raw_to_angle_p_s((int16_t)(sum / samples));
}
