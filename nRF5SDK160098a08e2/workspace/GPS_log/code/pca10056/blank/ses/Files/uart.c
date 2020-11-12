#include "uart.h"



uint8_t rx[RXD_UART]; //Buffer RX UART0
uint8_t tx[TXD_UART];//Buffer Rx UART0

//Inicializa el puerto serie  
//115200 baud, sin paridad, sin control de flujo y 1 bit de parada
void uarte_init(void){

  //disable for configuration
  NRF_UARTE0->ENABLE=UARTE_ENABLE_ENABLE_Disabled<<UARTE_ENABLE_ENABLE_Pos;

  //Select pins
  NRF_UARTE0->PSEL.TXD=0x7FFFFFC6;
  NRF_UARTE0->PSEL.CTS=0xFFFFFFC7;
  NRF_UARTE0->PSEL.RXD=0x7FFFFFC8;
  NRF_UARTE0->PSEL.RTS=0xFFFFFFC5;

  //point to buffers
  NRF_UARTE0->RXD.MAXCNT=RXD_UART;
  NRF_UARTE0->RXD.PTR=(uint32_t)&rx[0];
  NRF_UARTE0->TXD.MAXCNT=TXD_UART;
  NRF_UARTE0->TXD.PTR=(uint32_t)&tx[0];


  NRF_UARTE0->BAUDRATE=UARTE_BAUDRATE_BAUDRATE_Baud115200<<UARTE_BAUDRATE_BAUDRATE_Pos;   //115200 bps
  NRF_UARTE0->CONFIG=(UARTE_CONFIG_HWFC_Disabled<<UARTE_CONFIG_HWFC_Pos)                   // no flow control
                    |(UARTE_CONFIG_PARITY_Excluded<<UARTE_CONFIG_PARITY_Pos)              //no parity
                    |(UARTE_CONFIG_STOP_One<<UARTE_CONFIG_STOP_Pos);                      //one stop bit

  
  #ifdef uart_interrupt
  /*  Interrupcion de lectura->Prioridad:6, filtrado a traves de caracter '\n\r'
  Funcion handler: UARTE0_UART0_IRQHandler(void)
  Ejemplo:

  Hola mundo\n\r (Escribir frase y pulsar intro en interfaz puerto serie)

  Receive: Hola mundo*/

 
  NRF_UARTE0->SHORTS=UARTE_SHORTS_ENDRX_STARTRX_Enabled<<UARTE_SHORTS_ENDRX_STARTRX_Pos;
  NRF_UARTE0->INTEN=UARTE_INTEN_RXDRDY_Enabled<<UARTE_INTEN_RXDRDY_Pos;

  NVIC_SetPriority(UARTE0_UART0_IRQn,6);
  NVIC_EnableIRQ(UARTE0_UART0_IRQn);
  #endif

  
  //enable uarte0
  NRF_UARTE0->ENABLE=UARTE_ENABLE_ENABLE_Enabled<<UARTE_ENABLE_ENABLE_Pos;
 

}

void UART_sendSensorData (SENSORS* data)
{
  //Put SENSORS struct into tx array, to transmit it
   int i=0;
   i= putDataIntoArray(data->temp_amb, i);
   i= putDataIntoArray(data->temp_vaca, i);
   i= putDataIntoArray(data->imu_acc_x, i);
   i= putDataIntoArray(data->imu_acc_y, i);
   i= putDataIntoArray(data->imu_acc_z, i);
   i= putDataIntoArray(data->imu_giro_x, i);
   i= putDataIntoArray(data->imu_giro_y, i);
   i= putDataIntoArray(data->imu_giro_z, i);


    /* No hace falta ya esta en la inicialización del periférico
      Siempre se manda lo mismo

    NRF_UARTE0->TXD.MAXCNT=TXD_UART;      //8 variables x 4 bytes
    NRF_UARTE0->TXD.PTR=(uint32_t)tx;*/

    NRF_UARTE0->TASKS_STARTTX=1;

    while(NRF_UARTE0->EVENTS_ENDTX==0){};
    NRF_UARTE0->EVENTS_ENDTX=0;

    NRF_UARTE0->TASKS_STOPTX=1;

    //careful! EVENTS_STOPTX is not clear 
    //because i do not need it    

}


int putDataIntoArray (float data, int index)
{
  struct LIT_ENDIAN litendian;
  litendian.BE_FLOAT.all=data;


   tx[index]=litendian.BE_FLOAT.bytes.byte1;
   index++;
   tx[index]=litendian.BE_FLOAT.bytes.byte2;
   index++;
   tx[index]=litendian.BE_FLOAT.bytes.byte3;
   index++;
   tx[index]=litendian.BE_FLOAT.bytes.byte4;
   index++;

   return index;

}





uint32_t UART_recibirTiempo(){

    //NRF_UARTE0->RXD.MAXCNT=RXD_UART;    Siempre es cuatro , lo quitamos


    NRF_UARTE0->TASKS_STARTRX=1;

    while(NRF_UARTE0->EVENTS_ENDRX==0){};
    NRF_UARTE0->EVENTS_ENDRX=0;

    NRF_UARTE0->TASKS_STOPRX=1;

    //careful! EVENTS_STOPTX is not clear 
    //because i do not need it  

    uint32_t time = (uint32_t)rx[0] << 24 |
      (uint32_t)rx[1] << 16 |
      (uint32_t)rx[2] << 8  |
      (uint32_t)rx[3];


    return time;
}



////////////////***********************************////////////////////

void enviar_cadena(char data[]){

    NRF_UARTE0->TXD.MAXCNT=sizeof(data);
    NRF_UARTE0->TXD.PTR=(uint32_t)data;

    NRF_UARTE0->TASKS_STARTTX=1;

    while(NRF_UARTE0->EVENTS_ENDTX==0){};
    NRF_UARTE0->EVENTS_ENDTX=0;

    NRF_UARTE0->TASKS_STOPTX=1;

    //careful! EVENTS_STOPTX is not clear 
    //because i do not need it     
}

void enviar_int(int valor){
  char msg[8]={0};
  itoa(valor,msg,10);
  enviar_cadena(msg);
}

void enviar_hex(int valor){
  char msg[8]={0};
  itoa(valor,msg,16);
  enviar_cadena(msg);
}


void CR (void){
  char cr[2]=ENDLINE;
  enviar_cadena(cr);
}
void LF(void){
  char lf[2]=RETURN;
  enviar_cadena(lf);
}




/*
void UARTE0_UART0_IRQHandler(void){
  if(NRF_UARTE0->EVENTS_RXDRDY==1){
    NRF_UARTE0->EVENTS_RXDRDY=0;
    for(int j=0; j<sizeof(rx); j++){
      if(rx[j]=='\n'){
        NRF_UARTE0->TASKS_STOPRX=1;

        char data[]="Receive: ";
        enviar_cadena(data);
        enviar_cadena(rx);
        CR();
        led3_toggle;
        nrf_delay_ms(100);
        for(int i=0; i<RXD_UART; i++) rx[i]=' ';
      }
    }
  }
}
*/

