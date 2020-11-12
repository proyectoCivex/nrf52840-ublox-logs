#ifndef _ICM20948_H
#define _ICM20948_H

#include "CommonHeaders.h"
#include "definitions.h"
#include "i2c.h"
#include "icm20948_definitions.h"
#include "icm20948_registers.h"




uint8_t icm20948_read(uint8_t regist);



void icm20948_init();
uint8_t icm20948_read(uint8_t );
void icm20948_write (uint8_t , uint8_t );




void icm20948_reset();
void icm20948_sleep();
void icm20948_lowPower ();
void icm20948_powerOffMagnetometer();
void icm20948_setBank(uint8_t );
void icm20948_configGyro ();
void icm20948_configAccel ();
void icm20948_enableFIFO();
int16_t icm20948_bytesWrittenInFIFO ();
//get the last data in FIFO and save in M_SENSORS 
void icm20948_getLastDataInFIFO(M_SENSORS*, uint8_t );  
//get all the available data in FIFO and save in M_SENSORS (except the old dummy data )
void icm20948_getAllDataInFIFO (M_SENSORS* );
//read old dummy data (used for trash while waiting the RF initialization)
void icm20948_readOldDummyData();

void icm20948_getData(SENSORS* );
#endif
