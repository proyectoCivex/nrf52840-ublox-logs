#ifndef _ICM20948_DEFINITIONS_H
#define _ICM20948_DEFINITIONS_H


      /****************************************
        // SENSOR ICM20948 REGISTERS
      ****************************************/
      
//#define ICM20948_AD0 //if ADC=1

#ifdef ICM20948_AD0       
  #define ICM20948_DEV_ADD 0x69
#else         
  #define ICM20948_DEV_ADD 0x68
#endif

#define ICM20948_VALUE_WHO_AM_I  0xEA
#define ICM20948_REG_BANK_SEL  0x7F



/************USER BANK 0 - REGISTER MAP*************************/
#define ICM20948_WHO_AM_I        0x00
#define ICM20948_USER_CTRL       0x03
#define ICM20948_LP_CONFIG       0x05  
#define ICM20948_PWR_MGMT_1      0x06
#define ICM20948_PWR_MGMT_2      0x07

#define ICM20948_INT_ENABLE      0x10

#define ICM20948_ACCEL_XOUT_H    0x2D
#define ICM20948_ACCEL_XOUT_L    0x2E
#define ICM20948_ACCEL_YOUT_H    0x2F
#define ICM20948_ACCEL_YOUT_L    0x30
#define ICM20948_ACCEL_ZOUT_H    0x31
#define ICM20948_ACCEL_ZOUT_L    0x32

#define ICM20948_GYRO_XOUT_H     0x33
#define ICM20948_GYRO_XOUT_L     0x34
#define ICM20948_GYRO_YOUT_H     0x35
#define ICM20948_GYRO_YOUT_L     0x36
#define ICM20948_GYRO_ZOUT_H     0x37
#define ICM20948_GYRO_ZOUT_L     0x38

#define ICM20948_TEMP_OUT_H      0x39
#define ICM20948_TEMP_OUT_L      0x3A

#define ICM20948_FIFO_EN_1       0x66
#define ICM20948_FIFO_EN_2       0x67
#define ICM20948_FIFO_RST        0x68
#define ICM20948_FIFO_MODE       0x69
#define ICM20948_FIFO_COUNT_H    0x70
#define ICM20948_FIFO_COUNT_L    0x71
#define ICM20948_FIFO_R_W        0x72


  

/************USER BANK 2 - REGISTER MAP*************************/

#define ICM20948_GYRO_SMPLRT_DIV   0x00
#define ICM20948_GYRO_CONFIG_1     0x01
#define ICM20948_GYRO_CONFIG_2     0x02
#define ICM20948_ACCEL_SMPLRT_DIV_1  0x10
#define ICM20948_ACCEL_SMPLRT_DIV_2  0x11
#define ICM20948_ACCEL_INTEL_CRTL   0x12
#define ICM20948_ACCEL_CONFIG      0x14
#define ICM20948_ACCEL_CONFIG_2     0x15

/************USER BANK 3 - REGISTER MAP*************************/

/************MAGNETOMETER*************************/

#define ICM20948_MAG_AK09916_I2C_ADDR 0x0C
#define ICM20948_MAG_AK09916_WHO_AM_I1 0x48   //Company ID (answer to WIA1)
#define ICM20948_MAG_AK09916_WHO_AM_I2 0x09   //Device ID   (answer to WAI2)


#define ICM20948_MAG_REG_WIA1     0x00
#define ICM20948_MAG_REG_WIA2     0x01
#define ICM20948_MAG_REG_ST1      0x10
#define ICM20948_MAG_REG_HXL      0x11
#define ICM20948_MAG_REG_HXH      0x12
#define ICM20948_MAG_REG_HYL      0x13
#define ICM20948_MAG_REG_HYH      0x14
#define ICM20948_MAG_REG_HZL      0x15
#define ICM20948_MAG_REG_HZH      0x16
#define ICM20948_MAG_REG_ST2      0x18
#define ICM20948_MAG_REG_CNTL2    0x31
#define ICM20948_MAG_REG_CNTL3    0x32
#define ICM20948_MAG_REG_TS1      0x33
#define ICM20948_MAG_REG_TS2      0x34


/***********************CONSTANTS FOR CONFIGURATION*****************/
#define ICM20948_GYRO_FS_SEL_250_DPS 0b00             //Sensibility: 131 LSB/dps
#define ICM20948_GYRO_FS_SEL_500_DPS 0b01             //Sensibility: 65.5 LSB/dps
#define ICM20948_GYRO_FS_SEL_1000_DPS 0b10            //Sensibility: 32.8 LSB/dps
#define ICM20948_GYRO_FS_SEL_2000_DPS 0b11            //Sensibility: 16.4 LSB/dps

#define ICM20948_ACCEL_FS_SEL_2_G 0b00                 //Sensibility: 16384 LSB/g
#define ICM20948_ACCEL_FS_SEL_4_G 0b01                 //Sensibility: 8192 LSB/g
#define ICM20948_ACCEL_FS_SEL_8_G 0b10                 //Sensibility: 4096 LSB/g
#define ICM20948_ACCEL_FS_SEL_16_G 0b11                //Sensibility: 2048 LSB/g

#define ICM20948_AVERAGING_x1   0b000
#define ICM20948_AVERAGING_x2   0b001
#define ICM20948_AVERAGING_x4   0b010
#define ICM20948_AVERAGING_x8   0b011
#define ICM20948_AVERAGING_x16  0b100
#define ICM20948_AVERAGING_x32  0b101
#define ICM20948_AVERAGING_x64  0b110
#define ICM20948_AVERAGING_x128 0b111



 

/****************END OF CONSTANTS FOR CONFIGURATION*****************/



#endif