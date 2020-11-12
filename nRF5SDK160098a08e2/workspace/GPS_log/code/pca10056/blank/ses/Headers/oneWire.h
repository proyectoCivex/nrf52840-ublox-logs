#ifndef _oneWire_H
#define _onwWire_H

#include <nrfx.h>
#include "defines.h"
#include "nrf_delay.h"


/**********************************************************************/
//ONEWIRE --> TEMPERATURE SENSOR DS18B20
//Note: 
//The code is implemented to:
//   -Power the DS18B20 with an External supply
//   -Only one sensor
/**********************************************************************/


/**********************************************************************/  
//DS18B20 TEMPERATURE SENSOR - COMMAND DESCRIPTION
/**********************************************************************/
#define CONVER_TEMP 0x44
#define READ_SCRATCH 0xBE
#define SKIP_ROM 0xCC
#define READ_ROM 0x33



/**********************************************************************/
//PIN DEFINITION
/**********************************************************************/
#define PIN_ONEWIRE 31 //P0.31 (A7)



/**********************************************************************/
//      DECLARATION OF FUNCTIONS
/**********************************************************************/

//Function to initialize GPIO (pin P0.31)
void initOneWire ();


//This function is required for starting all the communications
/*All communication with the DS18B20 begins with an initialization sequence that consists of a reset pulse
from the master followed by a presence pulse from the DS18B20. 
When the DS18B20 sends the presence pulse in response to the reset, it is indicating to the master that it is on
the bus and ready to operate.*/
void resetWire();


//Send command to convert temp and read temperature data in celsius
float OnewireReadTemp();
void OnewireSendConvertTempCommand();     //only send comand to convert temp
float OnewireReceivedTemp ();           //only received temp (necessary to send comand previously)


//This funciton is internally used by OneWireReadTemp() function.
//The temperature data is stored as a 16-bit sign-extended two’s complement number in the temperature register. If the
//DS18B20 is configured for 12-bit resolution (by default), all bits in the temperature register will contain valid data
float complement2_12bitsResolution (uint16_t);

//put gpio pin as an input
void goInput();

//put gpio pin as an out
void goOutput();

//send comand
void SendCMD (uint8_t);

//read data
int ReadBit();
int ReadByte();


//this functions are not used
//not sure if they work properly
void ReadSerialNumber();
int iBSPACMonewireComputeCRC (const unsigned char * romp, int len);

#endif