#include "gps.h"
#include "defines.h"

bool gps_ready=false;

uint8_t latitude;
uint8_t longuitude;


void uarte1_init(void){
  NRF_UARTE1->ENABLE=UARTE_ENABLE_ENABLE_Disabled<<UARTE_ENABLE_ENABLE_Pos;
    
  NRF_P1->PIN_CNF[2]=GPIO_PIN_CNF_DIR_Input<<GPIO_PIN_CNF_DIR_Pos;//RXD
  NRF_P1->PIN_CNF[1]=GPIO_PIN_CNF_DIR_Output<<GPIO_PIN_CNF_DIR_Pos;//TXD
  NRF_P1->OUTSET=(1<<1);

  NRF_UARTE1->PSEL.TXD=0x7FFFFFE1;//P1.01
  NRF_UARTE1->PSEL.RXD=0x7FFFFFE2;//P1.02


  //Point to rx and tx buffers
  NRF_UARTE1->RXD.MAXCNT=RXD_GPS;
  NRF_UARTE1->RXD.PTR=(uint32_t)&rx_gps[0];

  NRF_UARTE1->RXD.MAXCNT=TXD_GPS;
  NRF_UARTE1->RXD.PTR=(uint32_t)&tx_gps[0];

  NRF_UARTE1->BAUDRATE=UARTE_BAUDRATE_BAUDRATE_Baud9600<<UARTE_BAUDRATE_BAUDRATE_Pos;
  NRF_UARTE1->CONFIG=(UARTE_CONFIG_HWFC_Disabled<<UARTE_CONFIG_HWFC_Pos)
                    |(UARTE_CONFIG_PARITY_Excluded<<UARTE_CONFIG_PARITY_Pos)
                    |(UARTE_CONFIG_STOP_One<<UARTE_CONFIG_STOP_Pos);

#ifdef uart_interrupt
  NRF_UARTE1->INTEN=UARTE_INTEN_ENDRX_Enabled<<UARTE_INTEN_ENDRX_Pos;

  NVIC_SetPriority(UARTE1_IRQn,6);
  NVIC_EnableIRQ(UARTE1_IRQn);
#endif
  NRF_UARTE1->ENABLE=UARTE_ENABLE_ENABLE_Enabled<<UARTE_ENABLE_ENABLE_Pos;
#ifdef uart_interrupt
  NRF_UARTE1->TASKS_STARTRX=1;
  while(NRF_UARTE0->EVENTS_RXSTARTED!=1){};
  NRF_UARTE1->EVENTS_RXSTARTED=0;
#endif
}


void findAndfill(){
    int i=0;
    bool find=false;
    while(rx_gps[i]!='$' && i<RXD_GPS){
     i++;
    }
    find=true;
    int pos=i;
    int j=0;
    char frame[RXD_GPS];
    bool flag=false;
    while(rx_gps[pos+j]!='\n' && flag==false){
      frame[j]=rx_gps[pos+j];
      j++;
      if((pos+j)>RXD_GPS){
       flag=true;
      }
    }
    if(find && flag==0){
      if(frame[0]=='$' && frame[1]=='G' && frame[2]=='P' && frame[3]=='G' && frame[4]=='L' && frame[5]=='L'){
          enviar_cadena(frame,j);
          CR();
          gps_ready=true;
        }
    }
    for(int i=0; i<RXD_GPS; i++) frame[i]=0;
}




#ifdef uart_interrupt
void UARTE1_IRQHandler(void){
  
  findAndfill();
  NRF_UARTE1->TASKS_STARTRX=1;
  while(NRF_UARTE1->EVENTS_RXSTARTED!=1){};
  NRF_UARTE1->EVENTS_RXSTARTED=0;
  led3_toggle;
  nrf_delay_ms(100);
}
#else
void readUART1(){
    NRF_UARTE1->TASKS_STARTRX=1;
    while(NRF_UARTE1->EVENTS_RXSTARTED!=1){};
    NRF_UARTE1->EVENTS_RXSTARTED=0;
    while(NRF_UARTE1->EVENTS_ENDRX!=1){};
    NRF_UARTE1->EVENTS_ENDRX=0;
    findAndfill();
    led3_toggle;
}
#endif

void GPIOTE_IRQHandler(void){
  if(NRF_GPIOTE->EVENTS_IN[0]){
    NRF_GPIOTE->EVENTS_IN[0]=0;
    led1_toggle;
    nrf_delay_ms(100);
  }
  else if(NRF_GPIOTE->EVENTS_IN[1]){
    NRF_GPIOTE->EVENTS_IN[1]=0;
    led2_toggle;
    nrf_delay_ms(100);
  }
  else if(NRF_GPIOTE->EVENTS_IN[2]){
    NRF_GPIOTE->EVENTS_IN[2]=0;
    led3_toggle;
    nrf_delay_ms(100);
  }
  else if(NRF_GPIOTE->EVENTS_IN[3]){
    NRF_GPIOTE->EVENTS_IN[3]=0;
    //led4_toggle;
    nrf_delay_ms(100);
  }
}

