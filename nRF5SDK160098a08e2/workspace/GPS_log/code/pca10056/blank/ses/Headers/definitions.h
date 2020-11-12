#ifndef _DEFINITIONS_H
#define _DEFINITIONS_H


#include "CommonHeaders.h"


//COMMENT OR DESCOMMENT THIS LINE FOR USING DK OR DONGLE
#define DK

// Solo si nos interesa que nos llegue información relevante
//#define uart_interrupt


#define TIME_FOR_WRITTING_SD_IN_HOURS  1 //hour
#define TIME_FOR_WRITTING_SD_IN_SECONDS 10


//new structure to save multiple data from the imu
typedef struct multiple_sensors_struct { 
  uint32_t time;      
  float temp_imu;
  float temp_db18b20;
  float temp_NTC;
  float longitude;
  float latitude;
  float altitude;
  float speed;
  float imu_acc_x [18];
  float imu_acc_y [18];
  float imu_acc_z [18];
  float imu_giro_x [18];
  float imu_giro_y [18];
  float imu_giro_z [18];
  
} M_SENSORS;
extern M_SENSORS sensorData;



//old structure to save instant data
typedef struct sensors_struct { 
  uint32_t time;      
  float temp_amb;
  float temp_vaca;
  float termistor;
  float imu_acc_x;
  float imu_acc_y;
  float imu_acc_z;
  float imu_giro_x;
  float imu_giro_y;
  float imu_giro_z;
} SENSORS;

extern SENSORS old_sensorData;

extern bool timer;
extern bool sendKA;
extern bool newFile;






#endif