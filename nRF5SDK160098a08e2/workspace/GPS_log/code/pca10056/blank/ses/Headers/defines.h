#ifndef _DEFINES_H
#define _DEFINES_H

#include <nrfx.h>


#define DK

#ifdef DK

  #define led1_toggle NRF_P0->OUT^=(1<<13)
  #define led2_toggle NRF_P0->OUT^=(1<<14)
  #define led3_toggle NRF_P0->OUT^=(1<<15)
  #define led4_toggle NRF_P0->OUT^=(1<<16)

  #define led1_on NRF_P0->OUTCLR=(1<<13)
  #define led2_on NRF_P0->OUTCLR=(1<<14)
  #define led3_on NRF_P0->OUTCLR=(1<<15)
  #define led4_on NRF_P0->OUTCLR=(1<<16)

  #define led1_off NRF_P0->OUTSET=(1<<13)
  #define led2_off NRF_P0->OUTSET=(1<<14)
  #define led3_off NRF_P0->OUTSET=(1<<15)
  #define led4_off NRF_P0->OUTSET=(1<<16)

  #define btn1_state ((NRF_P0->IN>>11) & 0x01)
  #define btn2_state ((NRF_P0->IN>>12) & 0x01)
  #define btn3_state ((NRF_P0->IN>>24) & 0x01)
  #define btn4_state ((NRF_P0->IN>>25) & 0x01)

#else
  #define led1_toggle NRF_P0->OUT^=(1<<6)
  #define led2_toggle NRF_P0->OUT^=(1<<8)
  #define led3_toggle NRF_P1->OUT^=(1<<9)
//  #define led4_toggle NRF_P0->OUT^=(1<<12)

  #define led1_on NRF_P0->OUTCLR=(1<<6)
  #define led2_on NRF_P0->OUTCLR=(1<<8)
  #define led3_on NRF_P1->OUTCLR=(1<<9)
//  #define led4_on NRF_P0->OUTCLR=(1<<12)

  #define led1_off NRF_P0->OUTSET=(1<<6)
  #define led2_off NRF_P0->OUTSET=(1<<8)
  #define led3_off NRF_P1->OUTSET=(1<<9)
//  #define led4_off NRF_P0->OUTSET=(1<<12)

  #define btn1_state ((NRF_P1->IN>>6) & 0x01)
#endif
//------------------------------------------//


/*
__STATIC_INLINE void start_clock()
{
  NRF_CLOCK->TASKS_HFCLKSTART=1;
  while(NRF_CLOCK->EVENTS_HFCLKSTARTED==0){};
  NRF_CLOCK->EVENTS_HFCLKSTARTED=0;
}*/
/*
Funcion que incia el reloj externo-> 64Mhz
*/
__STATIC_INLINE void led_init(void){
#ifdef DK
    NRF_P0->DIRSET=(1<<13);//Led 1
    NRF_P0->OUTSET=(1<<13);
    NRF_P0->DIRSET=(1<<14);//Led 2
    NRF_P0->OUTSET=(1<<14);
    NRF_P0->DIRSET=(1<<15);//Led 3
    NRF_P0->OUTSET=(1<<15);
    NRF_P0->DIRSET=(1<<16);//Led 4
    NRF_P0->OUTSET=(1<<16);
 #else
    NRF_P0->DIRSET=(1<<6);
    NRF_P0->OUTSET=(1<<6);
    NRF_P0->DIRSET=(1<<8);
    NRF_P0->OUTSET=(1<<8);
    NRF_P1->DIRSET=(1<<9);
    NRF_P1->OUTSET=(1<<9);
//    NRF_P0->DIRSET=(1<<12);
//    NRF_P0->OUTSET=(1<<12);
 #endif
}
/*
Inicializacion de perifericos led para DK y para DONGLE
*/
__STATIC_INLINE void button_init(void){
  #ifdef DK
  NRF_P0->PIN_CNF[11]=(GPIO_PIN_CNF_DIR_Input<<GPIO_PIN_CNF_DIR_Pos)|(GPIO_PIN_CNF_PULL_Pullup<<GPIO_PIN_CNF_PULL_Pos);//BTN1
  NRF_P0->PIN_CNF[12]=(GPIO_PIN_CNF_DIR_Input<<GPIO_PIN_CNF_DIR_Pos)|(GPIO_PIN_CNF_PULL_Pullup<<GPIO_PIN_CNF_PULL_Pos);//BTN2
  NRF_P0->PIN_CNF[24]=(GPIO_PIN_CNF_DIR_Input<<GPIO_PIN_CNF_DIR_Pos)|(GPIO_PIN_CNF_PULL_Pullup<<GPIO_PIN_CNF_PULL_Pos);//BTN3
  NRF_P0->PIN_CNF[25]=(GPIO_PIN_CNF_DIR_Input<<GPIO_PIN_CNF_DIR_Pos)|(GPIO_PIN_CNF_PULL_Pullup<<GPIO_PIN_CNF_PULL_Pos);//BTN4
  NRF_GPIOTE->CONFIG[0]=(GPIOTE_CONFIG_MODE_Event<<GPIOTE_CONFIG_MODE_Pos)
                        |(GPIOTE_CONFIG_POLARITY_HiToLo<<GPIOTE_CONFIG_POLARITY_Pos)
                        |(0<<GPIOTE_CONFIG_PORT_Pos)
                        |(11<<GPIOTE_CONFIG_PSEL_Pos);
  NRF_GPIOTE->CONFIG[1]=(GPIOTE_CONFIG_MODE_Event<<GPIOTE_CONFIG_MODE_Pos)
                        |(GPIOTE_CONFIG_POLARITY_HiToLo<<GPIOTE_CONFIG_POLARITY_Pos)
                        |(0<<GPIOTE_CONFIG_PORT_Pos)
                        |(12<<GPIOTE_CONFIG_PSEL_Pos);
  NRF_GPIOTE->CONFIG[2]=(GPIOTE_CONFIG_MODE_Event<<GPIOTE_CONFIG_MODE_Pos)
                        |(GPIOTE_CONFIG_POLARITY_HiToLo<<GPIOTE_CONFIG_POLARITY_Pos)
                        |(0<<GPIOTE_CONFIG_PORT_Pos)
                        |(24<<GPIOTE_CONFIG_PSEL_Pos);
  NRF_GPIOTE->CONFIG[3]=(GPIOTE_CONFIG_MODE_Event<<GPIOTE_CONFIG_MODE_Pos)
                        |(GPIOTE_CONFIG_POLARITY_HiToLo<<GPIOTE_CONFIG_POLARITY_Pos)
                        |(0<<GPIOTE_CONFIG_PORT_Pos)
                        |(25<<GPIOTE_CONFIG_PSEL_Pos);

  NRF_GPIOTE->INTENSET=(GPIOTE_INTENSET_IN0_Set<<GPIOTE_INTENSET_IN0_Pos)
                      |(GPIOTE_INTENSET_IN1_Set<<GPIOTE_INTENSET_IN1_Pos)
                      |(GPIOTE_INTENSET_IN2_Set<<GPIOTE_INTENSET_IN2_Pos)
                      |(GPIOTE_INTENSET_IN3_Set<<GPIOTE_INTENSET_IN3_Pos);

  NRF_GPIOTE->EVENTS_IN[0]=0;
  NRF_GPIOTE->EVENTS_IN[1]=0;
  NRF_GPIOTE->EVENTS_IN[2]=0;
  NRF_GPIOTE->EVENTS_IN[3]=0;

  NVIC_SetPriority(GPIOTE_IRQn,2);
  NVIC_EnableIRQ(GPIOTE_IRQn);

#else
   NRF_P1->PIN_CNF[6]=(GPIO_PIN_CNF_DIR_Input<<GPIO_PIN_CNF_DIR_Pos)|(GPIO_PIN_CNF_PULL_Pullup<<GPIO_PIN_CNF_PULL_Pos);//BTN1
   NRF_GPIOTE->CONFIG[0]=(GPIOTE_CONFIG_MODE_Event<<GPIOTE_CONFIG_MODE_Pos)
                      |(GPIOTE_CONFIG_POLARITY_HiToLo<<GPIOTE_CONFIG_POLARITY_Pos)
                      |(1<<GPIOTE_CONFIG_PORT_Pos)
                      |(6<<GPIOTE_CONFIG_PSEL_Pos);
   NRF_GPIOTE->INTENSET=(GPIOTE_INTENSET_IN0_Set<<GPIOTE_INTENSET_IN0_Pos);
   NRF_GPIOTE->EVENTS_IN[0]=0;
   NVIC_SetPriority(GPIOTE_IRQn,1);
   NVIC_EnableIRQ(GPIOTE_IRQn);
#endif
}
/*
Iniciación de perifericos boton para DK y DONGLE
Habilitado su uso con interrupcion externa
Prioridad 1
*/
#endif