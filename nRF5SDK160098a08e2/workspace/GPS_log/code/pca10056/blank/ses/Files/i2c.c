#include "i2c.h"

//to work simulatenously with TWIM and SD (programming SD using fatfs library, which uses SPI0)
//is necessary to allocate NRF_TWIMi to NRF_TWIM1 
//because NRF_TWM0 and SPI0 shares the same location registers

uint8_t tx_buffer[TXD_TWI];
uint8_t rx_buffer[RXD_TWI];


void i2cInit(uint8_t address){

    //Disable to configure GPIO
    NRF_TWIM1->ENABLE=TWIM_ENABLE_ENABLE_Disabled<<TWI_ENABLE_ENABLE_Pos;
    
    //Prueba deshabilitar los pull-up para la comunicación con GPS
    // puede que sea un problema de i2c timings que se agraba
    // al aumentar la resistencia de pull-up
    //ublox ya cuenta con resistencias pull-up integradas

 //he internal pull-up is set in the nrf_drv_twi_init() function in the nrf_drv_twi.c file:
 // NRF_GPIO->PIN_CNF[p_config->scl] = SCL_PIN_INIT_CONF;
 // NRF_GPIO->PIN_CNF[p_config->sda] = SDA_PIN_INIT_CONF;

  //and SCL_PIN_INIT_CONF is defined as: 
  /*#define SCL_PIN_INIT_CONF     ( (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos) \
                              | (GPIO_PIN_CNF_DRIVE_S0D1     << GPIO_PIN_CNF_DRIVE_Pos) \
                              | (GPIO_PIN_CNF_PULL_Pullup    << GPIO_PIN_CNF_PULL_Pos)  \
                              | (GPIO_PIN_CNF_INPUT_Connect  << GPIO_PIN_CNF_INPUT_Pos) \
                              | (GPIO_PIN_CNF_DIR_Input      << GPIO_PIN_CNF_DIR_Pos))*/

                               //Init GPIO for communication


    #ifdef DK       //DK pins
    NRF_TWIM1->PSEL.SCL= NRF_GPIO_PIN_MAP(1, 8);
    NRF_TWIM1->PSEL.SDA= NRF_GPIO_PIN_MAP(1, 7);

    
    NRF_P0->PIN_CNF[7] |=  (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos);       //Pull up on pin
    NRF_P0->PIN_CNF[8] |=  (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos);       //Pull up on pin

    #else         //Dongle pins
     NRF_TWIM1->PSEL.SCL= NRF_GPIO_PIN_MAP(0, 22);
    NRF_TWIM1->PSEL.SDA= NRF_GPIO_PIN_MAP(0, 24);
    #endif


    //400 bps
    NRF_TWIM1->FREQUENCY = TWIM_FREQUENCY_FREQUENCY_K400 << TWIM_FREQUENCY_FREQUENCY_Pos;

    //point to tx and rx register
    NRF_TWIM1->TXD.PTR = (uint32_t)&tx_buffer[0];
    NRF_TWIM1->RXD.PTR = (uint32_t)&rx_buffer[0];


    NRF_TWIM1->ADDRESS = address;//DIR of device

    NRF_TWIM1->ENABLE = TWIM_ENABLE_ENABLE_Enabled << TWIM_ENABLE_ENABLE_Pos;
}


uint8_t i2cRead(uint8_t address, uint8_t regist){

  NRF_TWIM1->ADDRESS = address;//DIR of device 
  NRF_TWIM1->TXD.MAXCNT = 1;
  NRF_TWIM1->RXD.MAXCNT = 1;
  //shorcut --> LASTTX event with STARTRX task and  LASTRX event with STOP task
  NRF_TWIM1->SHORTS = TWIM_SHORTS_LASTTX_STARTRX_Msk | TWIM_SHORTS_LASTRX_STOP_Msk;

  //write transmit register
  tx_buffer[0] = regist;

 
  //start tranmission
  NRF_TWIM1->TASKS_STARTTX=1;
  while (NRF_TWIM1->EVENTS_STOPPED == 0){};
  NRF_TWIM1->EVENTS_STOPPED = 0;

  return rx_buffer[0];
}


void i2cWrite(uint8_t address, uint8_t regist, uint8_t value){

  NRF_TWIM1->ADDRESS = address;//DIR of device 
  NRF_TWIM1->SHORTS = TWIM_SHORTS_LASTTX_STOP_Msk;
  NRF_TWIM1->TXD.MAXCNT = 2;

  tx_buffer[0] = regist;
  tx_buffer[1] = value;

  
  //start tranmission
  NRF_TWIM1->TASKS_STARTTX = 1;
  while (NRF_TWIM1->EVENTS_STOPPED == 0){};
  NRF_TWIM1->EVENTS_STOPPED = 0; //need to be clear by firmware
}