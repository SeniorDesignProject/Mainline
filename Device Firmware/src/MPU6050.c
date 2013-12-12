
#include "MPU6050.h"

void MPU6050_Init() {
      //Sets sample rate to 8000/1+7 = 1000Hz
    I2C_Write(MPU6050_RA_SMPLRT_DIV, 0x07);
    //Disable FSync, 256Hz DLPF
    I2C_Write(MPU6050_RA_CONFIG, 0x00);
    //Disable gyro self tests, scale of 500 degrees/s
    I2C_Write(MPU6050_RA_GYRO_CONFIG, 0x08);
    //Disable accel self tests, scale of +-2g, no DHPF
    I2C_Write(MPU6050_RA_ACCEL_CONFIG, 0x00);
    
    I2C_Write(MPU6050_RA_PWR_MGMT_1, 0x00);
}
/*FOR SOME REASON WHEN WRITING TO THE SD CARD, IT READS FROM LEFT TO RIGHT OF GIVEN INPUT OF VARIABLE AND WRITES RIGHT TO LEFT*/
uint16_t getRawAccelX() {
	int16_t upper, lower;

	upper = I2C_Read(MPU6050_RA_ACCEL_XOUT_H);
	lower = I2C_Read(MPU6050_RA_ACCEL_XOUT_L);
	return (upper<<8)|(lower);
}

uint16_t getRawAccelY() {
	int16_t upper, lower;

	upper = I2C_Read(MPU6050_RA_ACCEL_YOUT_H);
	lower = I2C_Read(MPU6050_RA_ACCEL_YOUT_L);
	return (upper<<8)|(lower);
}

uint16_t getRawAccelZ() {
	int16_t upper, lower;

	upper = I2C_Read(MPU6050_RA_ACCEL_ZOUT_H);
	lower = I2C_Read(MPU6050_RA_ACCEL_ZOUT_L);
	return (upper<<8)|(lower);
}

uint16_t getRawGyroX() {
	int16_t upper, lower;

	upper = I2C_Read(MPU6050_RA_GYRO_XOUT_H);
	lower = I2C_Read(MPU6050_RA_GYRO_XOUT_L);
	return (upper<<8)|(lower);
}

uint16_t getRawGyroY() {
	int16_t upper, lower;

	upper = I2C_Read(MPU6050_RA_GYRO_YOUT_H);
	lower = I2C_Read(MPU6050_RA_GYRO_YOUT_L);
	return (upper<<8)|(lower);
}

uint16_t getRawGyroZ() {
	int16_t upper, lower;

	upper = I2C_Read(MPU6050_RA_GYRO_ZOUT_H);
	lower = I2C_Read(MPU6050_RA_GYRO_ZOUT_L);
	return (upper<<8)|(lower);
}
