#ifndef SDCARD_H
#define SCCARD_H

#include "definitions.h"
#include "string.h"


/**********************************************************************/  
//PIN DEFINITIONS
/**********************************************************************/
#ifdef DK       //DK pins
#define SDC_CS_PIN      ARDUINO_10_PIN  ///< SDC chip select (CS) pin.      ((P1.12)
#define SDC_MOSI_PIN    ARDUINO_11_PIN  ///< SDC serial data in (DI) pin. (P1.13)
#define SDC_MISO_PIN    ARDUINO_12_PIN  ///< SDC serial data out (DO) pin.  (P1.14)
#define SDC_SCK_PIN     ARDUINO_13_PIN  ///< SDC serial clock (CLK) pin.  (P1.15)
#else     //Dongle pins
#define SDC_SCK_PIN NRF_GPIO_PIN_MAP(0, 13)  ///< SDC serial clock (SCK) pin.
#define SDC_MOSI_PIN NRF_GPIO_PIN_MAP(0, 17) ///< SDC serial data in (DI) pin.
#define SDC_MISO_PIN NRF_GPIO_PIN_MAP(0, 15) ///< SDC serial data out (DO) pin.
#define SDC_CS_PIN NRF_GPIO_PIN_MAP(0, 20)   ///< SDC chip select (CS) pin.
#endif


/**********************************************************************/  
//OTHER DEFINITIONS 
/**********************************************************************/
#define SD_READING_OK  1
#define SD_READING_END_FILE 0
#define SD_READING_ERROR -1



/**********************************************************************/
//      DECLARATION OF FUNCTIONS
/**********************************************************************/

// It is intended to make multiple writes in the SD, to later read all at once
//you don't want to be opening and closing the file continuously, so it will go through parameters
// All the data is saved in binary for computational optimization


// Initialize FATFS disk I/O interface by providing the block device and mounting volume
void initializeSD ();

//Open file with argument FIL file (File object)
//The file is name as "data"+"xxxx"+".bin"
void openSDFile (FIL*, uint16_t);
//Write SENSORS structure
void writeSDBinary (FIL*, M_SENSORS*);    //change to SENSORS* if necessary
//Set file point to the header's file
// Call this function when you want to start reading from file
void setFilePointatHeader (FIL* file);
//Read SENSORs structure
int8_t readSDBinary (FIL*, SENSORS*); 
//close file
void closeSDFile (FIL*);
//Print directories and capacity of SD card
void infoSD();


            /****USAGE***/

/*
    initializeSD();
    openSDFile (&file);
    writeSDBinary(&file, &sensor);
    ...
    writeSDBinary(&file, &sensor);

    setFilePointatHeader(&file);

    readSDBinary(&file, &readSensor);
    ....
    readSDBinary(&file, &readSensor);

    closeSDfile(&file);
    */

    



/**********************************************************************/
//      DECLARATION OF OTHER FUNCTIONS (NOT IN USED)
/**********************************************************************/

//These functions are not used in this implementation, but they work
// The data is saved in human-redeable format (not binary)
#define FILE_NAME   "prueba.txt"
#define ENDLINE     "\n"
void writeVariableSD(int, int);
void readSD(int);
void readLineSD ();
//Initialize SD, open file, write SENSORS structure and close 
void openWriteANDCloseBinary (SENSORS*); 
//Initialize SD, open file, read SENSORS structure (the first one) and close  
void OpenReadANDCloseBinary (SENSORS*);




#endif //SDK_CONFIG_H
