#include "CommonHeaders.h"
#include "definitions.h"

uint8_t POFCON_threshold;


void initPower ()
{
 POFCON_threshold= 28;
 NRF_POWER->POFCON = POWER_POFCON_POF_Enabled<<POWER_POFCON_POF_Pos |      //Enable power failure comparator
                    POWER_POFCON_THRESHOLD_V28<<POWER_POFCON_THRESHOLD_Pos;    //Set power failure comprator threshold to 2.8V





  NRF_POWER->EVENTS_POFWARN = 0;
 
  NVIC_SetPriority(POWER_CLOCK_IRQn,2);
  NVIC_EnableIRQ(POWER_CLOCK_IRQn);

   NRF_LOG_INFO("Inicializando power ");

     //When the supply voltage falls below the defined threshold,
  //the power-fail comparator will generate an event (POFWARN) 
  NRF_POWER->INTENSET = POWER_INTENSET_POFWARN_Msk;       //Enable POFWARN event
                      
}



void POWER_CLOCK_IRQHandler(void)
{
   //Update variable
  POFCON_threshold--;
   //Save batery level in sensorData struct

   //ESTA COMENTADO ESTO PQ AHORA SE HA QUITADO DE LA ESTRUCTURA
  //sensorData.batLevel = POFCON_threshold;


  //change threshold 
  setPOFCONThreshold(POFCON_threshold);
    //clear event
  NRF_POWER->EVENTS_POFWARN = 0;
  //wait until event is clear
  while (NRF_POWER->EVENTS_POFWARN==1){};

  //NRF_LOG_INFO("Dentro del evento ");
  //NRF_LOG_INFO("Valor de la bateria %d ", sensorData.batLevel);
   NRF_POWER->POFCON |= POWER_POFCON_POF_Enabled<<POWER_POFCON_POF_Pos;
 
}


void setPOFCONThreshold (uint8_t threshold)
{
  switch (threshold)
  {
    case 28:
    NRF_POWER->POFCON = POWER_POFCON_POF_Disabled<<POWER_POFCON_POF_Pos|
                        POWER_POFCON_THRESHOLD_V28<<POWER_POFCON_THRESHOLD_Pos;    //Set power failure comprator threshold to 2.8V
    break;
    case 27: 
    NRF_POWER->POFCON = POWER_POFCON_POF_Disabled<<POWER_POFCON_POF_Pos|
    POWER_POFCON_THRESHOLD_V27<<POWER_POFCON_THRESHOLD_Pos;    //Set power failure comprator threshold to 2.7V
    break;
    case 26:
    NRF_POWER->POFCON = 
    POWER_POFCON_POF_Disabled<<POWER_POFCON_POF_Pos|
    POWER_POFCON_THRESHOLD_V26<<POWER_POFCON_THRESHOLD_Pos;    //Set power failure comprator threshold to 2.6V
    break;
    case 25: 
    NRF_POWER->POFCON = 
    POWER_POFCON_POF_Disabled<<POWER_POFCON_POF_Pos|
    POWER_POFCON_THRESHOLD_V25<<POWER_POFCON_THRESHOLD_Pos;    //Set power failure comprator threshold to 2.5V
    break;
    case 24:
    NRF_POWER->POFCON = POWER_POFCON_POF_Disabled<<POWER_POFCON_POF_Pos|
    POWER_POFCON_THRESHOLD_V24<<POWER_POFCON_THRESHOLD_Pos;    //Set power failure comprator threshold to 2.4V
    break;
    case 23:
    NRF_POWER->POFCON = POWER_POFCON_POF_Disabled<<POWER_POFCON_POF_Pos|
    POWER_POFCON_THRESHOLD_V23<<POWER_POFCON_THRESHOLD_Pos;    //Set power failure comprator threshold to 2.3V
    break;
    case 22:
    NRF_POWER->POFCON = POWER_POFCON_POF_Disabled<<POWER_POFCON_POF_Pos|
    POWER_POFCON_THRESHOLD_V22<<POWER_POFCON_THRESHOLD_Pos;    //Set power failure comprator threshold to 2.2V
    break;
    case 21:
    NRF_POWER->POFCON = POWER_POFCON_POF_Disabled<<POWER_POFCON_POF_Pos|
    POWER_POFCON_THRESHOLD_V21<<POWER_POFCON_THRESHOLD_Pos;    //Set power failure comprator threshold to 2.1V
    break;
    case 20:
    NRF_POWER->POFCON = POWER_POFCON_POF_Disabled<<POWER_POFCON_POF_Pos|
    POWER_POFCON_THRESHOLD_V20<<POWER_POFCON_THRESHOLD_Pos;    //Set power failure comprator threshold to 2.0V
    break;
    case 19:
    NRF_POWER->POFCON = POWER_POFCON_POF_Disabled<<POWER_POFCON_POF_Pos|
    POWER_POFCON_THRESHOLD_V19<<POWER_POFCON_THRESHOLD_Pos;    //Set power failure comprator threshold to 1.9V
    break;
    case 18: 
    NRF_POWER->POFCON = POWER_POFCON_POF_Disabled<<POWER_POFCON_POF_Pos|
    POWER_POFCON_THRESHOLD_V18<<POWER_POFCON_THRESHOLD_Pos;    //Set power failure comprator threshold to 1.8V
    break;
    case 17:
    NRF_POWER->POFCON = POWER_POFCON_POF_Disabled<<POWER_POFCON_POF_Pos|
    POWER_POFCON_THRESHOLD_V17<<POWER_POFCON_THRESHOLD_Pos;    //Set power failure comprator threshold to 1.7V
    break;
  }
  
}

