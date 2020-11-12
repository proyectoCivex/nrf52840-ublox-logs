/////////////////////////////////////////
//CÓDIGO DEL COLLAR - DK
//////////////////////////////////////

/*

Conexiones DK:                              | Conexiones Dongle:
                                            | 
- SAM-M8Q                                   | - SAM-M8Q 
  // SDA->P1.07                             |   // SDA->P0.24
  // SCL->P1.08                             |   // SCL->P0.22
                                            | 
*/

//Configure console at 115200 baud


#include "definitions.h"
#include "RF.h"
#include "rtc.h"
#include "sam_m8q.h"




//Cuando cualquier funcion de un periférico da error,
//esa función se llama a si misma, de tal forma que deja el programa atascado
//asi el keepAlive no se puede mandar

/**
 * @brief Function for main application entry.
 */
  int main(void) {
  bsp_board_init(BSP_INIT_LEDS);
  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  NRF_LOG_DEFAULT_BACKENDS_INIT();


  //Initialization of peripherals 

  NRF_LOG_INFO("starting...");
  initRF();
  
  gpsInit();
  
  //When everything is set, init RTC
  initRTC();



  while (true) {
    //each second...
    // This task should be running in RTC0_IRQHandler
    //However fatfs (writeSDBinary) uses interrupts which affect the normal behaviour of this interrupt and it is not possible

    while (timer == false) {
    };
    timer = false;

 
    samM8Q_getPosition();      

    NRF_LOG_INFO("lONG " NRF_LOG_FLOAT_MARKER "\r\n", NRF_LOG_FLOAT(sensorData.longitude));
    NRF_LOG_INFO("lAT " NRF_LOG_FLOAT_MARKER "\r\n", NRF_LOG_FLOAT(sensorData.latitude));
    NRF_LOG_INFO("alt " NRF_LOG_FLOAT_MARKER "\r\n", NRF_LOG_FLOAT(sensorData.altitude));
    NRF_LOG_INFO("speed " NRF_LOG_FLOAT_MARKER "\r\n", NRF_LOG_FLOAT(sensorData.speed));


  }


 }

