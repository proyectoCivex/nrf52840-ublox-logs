#include "mpu5060.h"
#include "i2c.h"


void mpu5060_init ()
{
    i2cInit(MPU6050_IMU_ADDR);

    if (mpu5060_read(MPU6050_WHO_AM_I)!=MPU6050_IMU_ADDR)
    {
      return;   //imu not found
    }

    //Configurate imu
    mpu5060_write(MPU6050_PWR_MGMT_1,3);

}


uint8_t mpu5060_read(uint8_t regist)
{
  return i2cRead (MPU6050_IMU_ADDR, regist);
}

void mpu5060_write (uint8_t regist, uint8_t value)
{
   i2cWrite(MPU6050_IMU_ADDR, regist, value);
}


void mpu5060_getData(SENSORS* myinfo){

  int16_t rawAccX, rawAccY, rawAccZ, rawTemp, rawGyroX, rawGyroY, rawGyroZ;


  rawTemp = mpu5060_read(MPU6050_TEMP_OUT_H) << 8 | mpu5060_read(MPU6050_TEMP_OUT_L);
  myinfo->temp_amb= (float)(rawTemp/340) + 36.53;


  rawAccX = mpu5060_read(MPU6050_ACCEL_X_H) << 8 | mpu5060_read(MPU6050_ACCEL_X_L);
  myinfo->imu_acc_x = ((float)rawAccX)/16384.0;         //range 2g default scale


  rawAccY = mpu5060_read(MPU6050_ACCEL_Y_H) << 8 | mpu5060_read(MPU6050_ACCEL_Y_L);
  myinfo->imu_acc_y= ((float)rawAccY)/16384.0;         //range 2g default scale

  rawAccZ = mpu5060_read(MPU6050_ACCEL_Z_H) << 8 | mpu5060_read(MPU6050_ACCEL_Z_L);
  myinfo->imu_acc_z = ((float)rawAccZ)/16384.0;         //range 2g default scale

  rawGyroX = mpu5060_read(MPU6050_GYRO_X_H) << 8 | mpu5060_read(MPU6050_GYRO_X_L);
  myinfo->imu_giro_x = ((float)rawGyroX)/131.0;         //range 250 default scale

 rawGyroY = mpu5060_read(MPU6050_GYRO_Y_H) << 8 | mpu5060_read(MPU6050_GYRO_Y_L);
  myinfo->imu_giro_y = ((float)rawGyroY)/131.0;         //range 250 default scale


 rawGyroZ = mpu5060_read(MPU6050_GYRO_Z_H) << 8 | mpu5060_read(MPU6050_GYRO_Z_L);
 myinfo->imu_giro_z = ((float)rawGyroX)/131.0;         //range 250 default scale

}

