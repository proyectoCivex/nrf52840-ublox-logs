#ifndef _RF_H
#define _RF_H

#include "communicationDefinitions.h"
#include "definitions.h"





//A radio packet contains: PREAMBLE/ADDRESS/S0/LENGTH/S1/PAYLOAD/CRC
//In memory only saved:   S0/LENGHT/S1/PAYLOAD
//S0 + LENGHT+S1+PAYLOAD <= 258 bits (32 bytes)


#define LENGHT 8    //lenght of the LENGHT field (number of bits)
#define S0  0       //lenght of the S0 field (number of bits bits)
#define S1  0       //lenght of the S0 field (number of bits bits)
#define C1  0       //lenght of the S0 field (number of bits bits)
#define TERM1 1     //lenght of TERMD field (number of bits)
#define PAYLOAD 5    


//we are going to receive all the data one by one, because all the data 
//exceeds the maximum payload available

//PAYLOAD = ID_variables (1 bytes) + variable (4 bytes)

//ID_variables definitions
#define ID_TIMESTAMP   0x00
#define ID_TEMPAMB     0x01
#define ID_TEMPVACA    0x02
#define ID_IMUACCX     0x03
#define ID_IMUACCY     0x04
#define ID_IMUACCZ     0x05
#define ID_IMUGIROX    0x06
#define ID_IMUGIROY    0x07
#define ID_IMUGIROZ    0x08


#define ID_KA    0xAB
#define KEEPALIVE    0xFFFFFFFF




//Initialization of RF 
void initRF();
//Start high frequency clock for running RF communication
void start_clock();
//Disable RF peripheral
void disableRF(void);


//Send Uint32 plus ID
void RF_sendUint32(int idData, uint32_t);
//Send float32 plus ID
void RF_sendFloat (int idData, float floatData);
//Send frame (lenght = PAYLOAD)
void RF_sendFrame(uint8_t send_data[]);
//Send data sensors struct
void RF_sendSensorData (SENSORS* );
//Send KA message
void RF_SendKeepAlive ();


//receive one data packet and save it in SENSORS* struct
// this function check the ID_variable
void RF_receive(SENSORS*);
//receive dataSensors
SENSORS RF_receiveDataSensor (void);
//receive time
uint32_t RF_receiveTime(void);
//receive KA message
bool RF_receiveKeepAlive(void);








#endif