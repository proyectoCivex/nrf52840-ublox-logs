#ifndef _MPU5060_H
#define _MPU5060_H

#include "definitions.h"
#include "i2c.h"
#include "mpu5060_definitions.h"




void mpu5060_init();
/*Function for initialization the mpu5060 device*/

uint8_t mpu5060_read(uint8_t regist);
void mpu5060_write (uint8_t regist, uint8_t value);




void mpu5060_getData(SENSORS* );
/*Function for read the registers of temperature, gyroscope and acceleration measurements
of the IMU and save it in SENSORES struct  */












#endif

