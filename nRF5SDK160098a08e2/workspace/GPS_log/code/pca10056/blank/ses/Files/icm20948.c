#include "icm20948.h"
#include "i2c.h"


struct ICM_20948 icm20948regs;


void icm20948_init ()
{

    i2cInit(ICM20948_DEV_ADD);

    if (icm20948_read(ICM20948_WHO_AM_I)!=ICM20948_VALUE_WHO_AM_I)
    {
      icm20948_init();    //se queda en bucle.. no va a mandar consigna
      //RF_LOG_INFO("IMU NOT FOUND");
    }

    //Reset IMU to set default configuration
    icm20948_reset();
    //Configuration imu for low power mode and minimium current consumption
    icm20948_lowPower();
    icm20948_configGyro();
    icm20948_configAccel();

    icm20948_enableFIFO();

}


uint8_t icm20948_read(uint8_t regist)
{
  return i2cRead (ICM20948_DEV_ADD, regist);
}

void icm20948_write (uint8_t regist, uint8_t value)
{
   i2cWrite(ICM20948_DEV_ADD, regist, value);
}



void icm20948_reset ()
{
  icm20948_setBank(0);
  icm20948regs.PWR_MGMT1_REG.bit.CLKSEL= 1;   //Automatic election of the clock to optimize the consumption
  icm20948regs.PWR_MGMT1_REG.bit.TEMP_DIS=0;  //enable temperature sensor
  icm20948regs.PWR_MGMT1_REG.bit.LP_EN= 1;    //turn on low power feature
  icm20948regs.PWR_MGMT1_REG.bit.SLEEP= 0;    //no sleep
  icm20948regs.PWR_MGMT1_REG.bit.DEVICE_RESET= 1;   // reset
  icm20948_write(ICM20948_PWR_MGMT_1, icm20948regs.PWR_MGMT1_REG.all);
  nrf_delay_ms(500);  
}

void icm20948_sleep ()
{
  icm20948regs.PWR_MGMT1_REG.bit.CLKSEL= 1;   //Automatic election of the clock to optimize the consumption
  icm20948regs.PWR_MGMT1_REG.bit.TEMP_DIS=0;  //enable temperature sensor
  icm20948regs.PWR_MGMT1_REG.bit.LP_EN= 1;    //turn on low power feature
  icm20948regs.PWR_MGMT1_REG.bit.SLEEP= 1;    //yes sleep
  icm20948regs.PWR_MGMT1_REG.bit.DEVICE_RESET= 0;   // no reset
  icm20948_write(ICM20948_PWR_MGMT_1, icm20948regs.PWR_MGMT1_REG.all);
}



/* NO SE HA METIDO EL SELF-TEST DEL BANK1*/


void icm20948_lowPower ()
{
  icm20948_setBank(0);

  icm20948regs.LP_CONFIG_REG.bit.I2C_MST_CYCLE = 0;   //Disable i2c master (external sensors)
  icm20948regs.LP_CONFIG_REG.bit.ACCEL_CYCLE = 1;   //accel duty cycled mode enabled (low power)
  icm20948regs.LP_CONFIG_REG.bit.GYRO_CYCLE = 1;   //gyro duty cycled mode enabled (low power)
  icm20948_write(ICM20948_LP_CONFIG, icm20948regs.LP_CONFIG_REG.all);

  icm20948regs.PWR_MGMT1_REG.bit.CLKSEL= 1;   //Automatic election of the clock to optimize the consumption
  icm20948regs.PWR_MGMT1_REG.bit.TEMP_DIS=0;  //enable temperature sensor
  icm20948regs.PWR_MGMT1_REG.bit.LP_EN= 1;    //turn on low power feature
  icm20948regs.PWR_MGMT1_REG.bit.SLEEP= 0;    //no sleep
  icm20948regs.PWR_MGMT1_REG.bit.DEVICE_RESET= 0;   //no reset
  icm20948_write(ICM20948_PWR_MGMT_1, icm20948regs.PWR_MGMT1_REG.all); 

}




void icm20948_configGyro()
{ 
  
  icm20948_setBank(2);
  //i2cWrite(ICM20948_GYRO_SMPLRT_DIV, 0xFF);     // frequency sample = 4.4 Hz  (maximum value for minimum current consumption)  
  icm20948_write(ICM20948_GYRO_SMPLRT_DIV, 0x3F);     // frequency sample = 17.6 Hz   


  //Enable gyro LPFilter; scale to 250 dpbs;
  icm20948regs.GYRO_CONFIG_1_REG.bit.GYRO_FCHOICE= 1;       //enable gyro DLPF (necessary for effectiveness of GYRO_SMPLRT_DIV register)
  icm20948regs.GYRO_CONFIG_1_REG.bit.GYRO_FS_SEL= ICM20948_GYRO_FS_SEL_250_DPS;  //scale +-250 dps
  icm20948regs.GYRO_CONFIG_1_REG.bit.GYRO_DLPCFG= 3;        //must be >0 and <7; choose 3 for 51.2 cut off frequency of low pass filter
  icm20948_write(ICM20948_GYRO_CONFIG_1, icm20948regs.GYRO_CONFIG_1_REG.all);  


}


void icm20948_configAccel()
{
  //ACCEL_SMPLRT_DIV = 4095 (value for minimum current consumption)
  icm20948_setBank(2);
  
  /*icm20948_write(ICM20948_ACCEL_SMPLRT_DIV_1, 0x00);
  icm20948_write(ICM20948_ACCEL_SMPLRT_DIV_2, 0xFF);*/    //SMPLRT_DIV = 255 (f=4.4 Hz)

  icm20948_write(ICM20948_ACCEL_SMPLRT_DIV_1, 0x00);
  icm20948_write(ICM20948_ACCEL_SMPLRT_DIV_2, 0x3F);    //SMPLRT_DIV = 63 (f=17.6 Hz)

  //Disable DLPFilter -> DLPCFG do not care; fix scale to 2+-g;
  icm20948regs.ACCEL_CONFIG_REG.bit.ACCEL_FCHOICE= 0;     //For minimum current consumption
  icm20948regs.ACCEL_CONFIG_REG.bit.ACCEL_FS_SEL=ICM20948_ACCEL_FS_SEL_2_G;      //scale +-2g
  icm20948regs.ACCEL_CONFIG_REG.bit.ACCEL_DLPCFG= 3;        //do not care when FCHOICE = 0
  icm20948_write(ICM20948_ACCEL_CONFIG, icm20948regs.ACCEL_CONFIG_REG.all);

   
}

void icm20948_enableFIFO ()
{

  icm20948_setBank(0);
  //Disable the FIFO
  icm20948regs.USER_CTRL_REG.bit.DMP_EN= 0;
  icm20948regs.USER_CTRL_REG.bit.FIFO_EN=0;
  icm20948regs.USER_CTRL_REG.bit.I2C_MST_EN=0;
  icm20948regs.USER_CTRL_REG.bit.I2C_IF_DIS=0;
  icm20948regs.USER_CTRL_REG.bit.DMP_RST=0;
  icm20948regs.USER_CTRL_REG.bit.SRAM_RST=0;
  icm20948regs.USER_CTRL_REG.bit.I2C_MST_RST=0;
  icm20948_write(ICM20948_USER_CTRL, icm20948regs.USER_CTRL_REG.all); 
  //when the FIFO is full, additional writes will be written to the FIFO, replacing the oldest data
  icm20948_write(ICM20948_FIFO_MODE, 0X00);   

  
   //Enable accelerometer and gyro to store the data in FIFO
   icm20948regs.FIFO_EN_2_BITS_REGS.bit.TEMP_FIFO_EN= 1;
   icm20948regs.FIFO_EN_2_BITS_REGS.bit.GYRO_X_FIFO_EN=1;
   icm20948regs.FIFO_EN_2_BITS_REGS.bit.GYRO_Y_FIFO_EN=1;
   icm20948regs.FIFO_EN_2_BITS_REGS.bit.GYRO_Z_FIFO_EN=1;
   icm20948regs.FIFO_EN_2_BITS_REGS.bit.ACCEL_FIFO_EN=1;
   icm20948_write(ICM20948_FIFO_EN_2, icm20948regs.FIFO_EN_2_BITS_REGS.all); 

   //Reset the FIFO  
   //Assert and hold to set FIFO size to 0. Assert and de-assert to reset FIFO
   icm20948_write(ICM20948_FIFO_RST, 0x0F); 
   icm20948_write(ICM20948_FIFO_RST, 0x00); 

    // Enable the FIFO 
   icm20948regs.USER_CTRL_REG.bit.FIFO_EN=1;
   icm20948_write(ICM20948_USER_CTRL, icm20948regs.USER_CTRL_REG.all); 
 
}





void icm20948_getAllDataInFIFO (M_SENSORS* data)
{
  
  uint8_t i=0;
  int16_t bytes = icm20948_bytesWrittenInFIFO ();
  //One set of measurements takes 14 bytes (7 variables: 3 accels, 3 gyros, 1 temp)
  int16_t readings= bytes/14;
  //frequency 17.6 Hz --> 17 or 18 samples per seconds
  
  while (readings>18)   //more than the number it should be (in initialization)
  {
    icm20948_readOldDummyData();
    readings--;
  }

  for (i=0; i<readings; i++)
  {
    icm20948_getLastDataInFIFO(data, i);
  }
  if (readings == 17)
  {
    sensorData.imu_acc_x [17]= 0;
    sensorData.imu_acc_y [17]= 0;
    sensorData.imu_acc_z [17]= 0;
    sensorData.imu_giro_x [17]= 0;
    sensorData.imu_giro_y [17]= 0;
    sensorData.imu_giro_z [17]= 0;
  }
  
}


int16_t icm20948_bytesWrittenInFIFO ()
{
  int16_t bytes = icm20948_read(ICM20948_FIFO_COUNT_H) << 8 | icm20948_read(ICM20948_FIFO_COUNT_L);
}



void icm20948_getLastDataInFIFO(M_SENSORS* myinfo, uint8_t index){


  //CUIDADIN CON LAS SENSIBILIDADES QUE ESTÁN MARCADAS AHORA PARA LOS PARAMETROS QUE HEMOS DEFINIDO 
  // NO SE HA IMPLEMENTADO BIEN, POR NO AUMENTAR EL PROCESAMIENTO Y PQ SABEMOS
  // DADA LA APLICACIÓN NO SE VA A CAMBIAR LA SENSIBLIDAD EN NINGUN MOMENT..!
  int16_t rawAccX, rawAccY, rawAccZ, rawTemp, rawGyroX, rawGyroY, rawGyroZ;
  icm20948_setBank(0);
 
 
  rawAccX = icm20948_read(ICM20948_FIFO_R_W) << 8 | icm20948_read(ICM20948_FIFO_R_W);
  myinfo->imu_acc_x[index]= ((float)rawAccX)/16384.0;         //range 2g 

  rawAccY = icm20948_read(ICM20948_FIFO_R_W) << 8 | icm20948_read(ICM20948_FIFO_R_W);
  myinfo->imu_acc_y[index]= ((float)rawAccY)/16384.0;         //range 2g 

  rawAccZ = icm20948_read(ICM20948_FIFO_R_W) << 8 | icm20948_read(ICM20948_FIFO_R_W);
  myinfo->imu_acc_z[index] = ((float)rawAccZ)/16384.0;         //range 2g

  rawGyroX = icm20948_read(ICM20948_FIFO_R_W) << 8 | icm20948_read(ICM20948_FIFO_R_W);
  myinfo->imu_giro_x[index] = ((float)rawGyroX)/131.0;         //range 250

 rawGyroY = icm20948_read(ICM20948_FIFO_R_W) << 8 | icm20948_read(ICM20948_FIFO_R_W);
  myinfo->imu_giro_y[index] = ((float)rawGyroY)/131.0;         //range 250

  rawGyroZ = icm20948_read(ICM20948_FIFO_R_W) << 8 | icm20948_read(ICM20948_FIFO_R_W);
  myinfo->imu_giro_z[index] = ((float)rawGyroX)/131.0;         //range 250 


  rawTemp = icm20948_read(ICM20948_FIFO_R_W) << 8 | icm20948_read(ICM20948_FIFO_R_W);
  if (index==0)
  {
    myinfo->temp_imu= (float)(rawTemp/333.87) +21;
  }


}

void icm20948_readOldDummyData(){

  int16_t rawAccX, rawAccY, rawAccZ, rawTemp, rawGyroX, rawGyroY, rawGyroZ;
  icm20948_setBank(0);
 
  rawAccX = icm20948_read(ICM20948_FIFO_R_W) << 8 | icm20948_read(ICM20948_FIFO_R_W);
  rawAccY = icm20948_read(ICM20948_FIFO_R_W) << 8 | icm20948_read(ICM20948_FIFO_R_W);
  rawAccZ = icm20948_read(ICM20948_FIFO_R_W) << 8 | icm20948_read(ICM20948_FIFO_R_W);
  rawGyroX = icm20948_read(ICM20948_FIFO_R_W) << 8 | icm20948_read(ICM20948_FIFO_R_W);
  rawGyroY = icm20948_read(ICM20948_FIFO_R_W) << 8 | icm20948_read(ICM20948_FIFO_R_W);
  rawGyroZ = icm20948_read(ICM20948_FIFO_R_W) << 8 | icm20948_read(ICM20948_FIFO_R_W);
  rawTemp = icm20948_read(ICM20948_FIFO_R_W) << 8 | icm20948_read(ICM20948_FIFO_R_W);

}


void icm20948_setBank (uint8_t bank)
{
  if (bank<4)
  {
    bank = (bank << 4) & 0x30;
    icm20948_write(ICM20948_REG_BANK_SEL, bank);
  }
  
}


  //get instantaneous data
void icm20948_getData(SENSORS* myinfo){
  //CUIDADIN CON LAS SENSIBILIDADES QUE ESTÁN MARCADAS AHORA PARA LOS PARAMETROS QUE HEMOS DEFINIDO 
  // NO SE HA IMPLEMENTADO BIEN, POR NO AUMENTAR EL PROCESAMIENTO Y PQ SABEMOS
  // DADA LA APLICACIÓN NO SE VA A CAMBIAR LA SENSIBLIDAD EN NINGUN MOMENT..!
  int16_t rawAccX, rawAccY, rawAccZ, rawTemp, rawGyroX, rawGyroY, rawGyroZ;
  icm20948_setBank(0);
 
  rawTemp = icm20948_read(ICM20948_TEMP_OUT_H) << 8 | icm20948_read(ICM20948_TEMP_OUT_L);
  myinfo->temp_amb= (float)(rawTemp/333.87) +21;

  rawAccX = icm20948_read(ICM20948_ACCEL_XOUT_H) << 8 | icm20948_read(ICM20948_ACCEL_XOUT_L);
  myinfo->imu_acc_x= ((float)rawAccX)/16384.0;         //range 2g 

  rawAccY = icm20948_read(ICM20948_ACCEL_YOUT_H) << 8 | icm20948_read(ICM20948_ACCEL_YOUT_L);
  myinfo->imu_acc_y= ((float)rawAccY)/16384.0;         //range 2g 

  rawAccZ = icm20948_read(ICM20948_ACCEL_ZOUT_H) << 8 | icm20948_read(ICM20948_ACCEL_ZOUT_L);
  myinfo->imu_acc_z = ((float)rawAccZ)/16384.0;         //range 2g

  rawGyroX = icm20948_read(ICM20948_GYRO_XOUT_H) << 8 | icm20948_read(ICM20948_GYRO_XOUT_L);
  myinfo->imu_giro_x= ((float)rawGyroX)/131.0;         //range 250

 rawGyroY = icm20948_read(ICM20948_GYRO_YOUT_H) << 8 | icm20948_read(ICM20948_GYRO_YOUT_L);
  myinfo->imu_giro_y= ((float)rawGyroY)/131.0;         //range 250

 rawGyroZ = icm20948_read(ICM20948_GYRO_ZOUT_H) << 8 | icm20948_read(ICM20948_GYRO_ZOUT_L);
 myinfo->imu_giro_z = ((float)rawGyroX)/131.0;         //range 250 

}
