#ifndef _MPU6050_H
#define _MPU6050_H



      /****************************************
        // SENSOR MPU-6050  REGISTERS
      ****************************************/

#define GYRO_CONF   0x1B
#define ACCEL_CONF  0x1C

#define MPU6050_SELF_TEST_X 0x0D // Self test factory calibrated values register
#define MPU6050_SELF_TEST_Y 0x0E // Self test factory calibrated values register
#define MPU6050_SELF_TEST_Z 0x0F // Self test factory calibrated values register
#define MPU6050_SELF_TEST_A 0x10 // Self test factory calibrated values register

#define IMU_ADDR    0x68
#define WHO_AM_I    0x75
#define PWR_MGMT_1  0X6B
    //Accelerometers 16-bits 2's complements value
#define ACCEL_X_H   0x3B
#define ACCEL_X_L   0x3C
#define ACCEL_Y_H   0x3D
#define ACCEL_Y_L   0x3E
#define ACCEL_Z_H   0x3F
#define ACCEL_Z_L   0x40
  //temp (ºC) = TEMP_OUT as signed quantity)/340+36.53
#define TEMP_OUT_H  0x41
#define TEMP_OUT_L  0x42
  //GYROS 16-bits 2's complements value
#define GYRO_X_H    0X43
#define GYRO_X_L    0X44
#define GYRO_Y_H    0X45
#define GYRO_Y_L    0X46
#define GYRO_Z_H    0X47
#define GYRO_Z_L    0X48
  


      /****************************************
     // SENSOR MPU-6050  COMMANDS
      ****************************************/


#define FS_SEL_250    0x00    // +/- 250 deg/s (default value) --> LSB sensitivity = 131 LSB/º/s
#define FS_SEL_500    0x01    // +/- 500 deg/s                  --> LSB sensitivity = 65.5 LSB/º/s
#define FS_SEL_1000   0x02    // +/- 1000 deg/s             --> LSB sensitivity = 32.8 LSB/º/s
#define FS_SEL_2000   0x03    // +/- 2000 deg/s             --> LSB sensitivity = 16.4 LSB/º/s

#define AFS_SEL_2    0x00      // +/- 2g (default value)    --> LSB sensitivity = 16384 LSB/º/s
#define AFS_SEL_4    0x01      // +/- 4g                   --> LSB sensitivity = 8192 LSB/º/s
#define AFS_SEL_8    0x02      // +/- 8g                     --> LSB sensitivity = 4096 LSB/º/s
#define AFS_SEL_16   0x03      // +/- 16g                    --> LSB sensitivity = 2048 LSB/º/s



















#endif
