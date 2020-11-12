#ifndef _I2C_H
#define _I2C_H

#include "CommonHeaders.h"
#include "icm20948.h"
#include "mpu5060.h"


#define TXD_TWI 255 //Buffer de escritura 
#define RXD_TWI 255 //Buffer de lectura 

extern uint8_t tx_buffer[];
extern uint8_t rx_buffer[];


void i2cInit(uint8_t);
/*Function for initialization of i2c peripheral
 Connections:
    SDA -> P1.07
    SCL -> P1.08
Argument: uint8_t address (device address)*/



// Function for reading a register (used for both icm20948 and mpu5060)
uint8_t i2cRead(uint8_t, uint8_t) ;


// Function for reading a register (used for both icm20948 and mpu5060)
void i2cWrite(uint8_t , uint8_t , uint8_t );





#endif