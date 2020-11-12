#include "RTC.h"
#include "definitions.h"


uint32_t secondsOfWritting=0;
//uint32_t timeforWrittingInSeconds= TIME_FOR_WRITTING_SD_IN_HOURS*60*60;
uint32_t timeforWrittingInSeconds= TIME_FOR_WRITTING_SD_IN_SECONDS;
uint32_t secondsOfKAMessage=0;




void initRTC(void){

  NRF_CLOCK->CTIV = 20; //5s of calibration

  NRF_CLOCK->LFCLKSRC = CLOCK_LFCLKSTAT_SRC_RC<<CLOCK_LFCLKSRC_SRC_Pos;
  NRF_CLOCK->LFRCMODE = (CLOCK_LFRCMODE_MODE_Normal<<CLOCK_LFRCMODE_MODE_Pos);

  NRF_CLOCK->TASKS_LFCLKSTART = 1;
  while(NRF_CLOCK->LFCLKRUN==0){};
  while(NRF_CLOCK->LFCLKSTAT == 0x10000){};


  // fRTC [kHz] = 32.768 / (PRESCALER + 1 )
  NRF_RTC0->PRESCALER = 4095; //Counter each 125ms OVERFLOW = 582.542h
  NRF_RTC0->EVTEN = RTC_EVTEN_COMPARE0_Enabled<<RTC_EVTENCLR_COMPARE0_Pos; //Enable Compare 0 Event
  NRF_RTC0->CC[0] = 8;//Each 8 pulses 1 second

  NRF_RTC0->INTENSET = RTC_INTENSET_COMPARE0_Enabled<<RTC_INTENSET_COMPARE0_Pos;

  NVIC_SetPriority(RTC0_IRQn,1);
  NVIC_EnableIRQ(RTC0_IRQn);

  NRF_RTC0->TASKS_START = 1;

}

void RTC0_IRQHandler(void)
{
  NRF_RTC0->TASKS_CLEAR = 1;
  if(NRF_RTC0->EVENTS_COMPARE[0]){
   
    NRF_LOG_INFO(" RTC TIME %d ",  sensorData.time);
    secondsOfWritting++;
    //Add a second to the timeSTamp
    sensorData.time++;
    secondsOfKAMessage++;

     if (secondsOfWritting > timeforWrittingInSeconds)
    {
      newFile=true;   
      secondsOfWritting=1;
    }


   if (secondsOfKAMessage==60)
    {
      secondsOfKAMessage=1;
      sendKA=true;
    }
    

    // RF_sendSensorData(&sensorData);
    timer= true;
  
    NRF_RTC0->CC[0]=NRF_RTC0->COUNTER+8;
    NRF_RTC0->EVENTS_COMPARE[0] = 0;
  }
}