#ifndef _UART_H
#define _UART_H

#include "communicationDefinitions.h"
#include "definitions.h"



/**************************************************/
// UARTE0 is used for PC communication 
// Just connect the USB wire to your pc
// 115200 bps, no bit parity, no flow control and 1 stop bit
/**************************************************/


/****************************************************/
// It is also posible to use NRF_LOG_INFO library
// To initialize:
//  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
//  NRF_LOG_DEFAULT_BACKENDS_INIT();
// To write:
// NRF_LOG_INFO("Int value %d ", val);
//NRF_LOG_INFO("float value:" NRF_LOG_FLOAT_MARKER "\r\n", NRF_LOG_FLOAT(val));
/****************************************************/



//FALTA METER LO DE LAS INTERRUPCIONES
//PARA RECIBIR INFORMACIÓN DESDE EL PC
// DE MOMENTO SOLO MANDA INFORMACIÓN AL PC Y NO RECIBE

#define RXD_UART 4 //Buffer maximo de lectura 50
#define TXD_UART 32 //Buffer maximo de lectura 50

extern uint8_t rx[RXD_UART]; //Buffer RX UART0
extern uint8_t tx[TXD_UART];//Buffer Rx UART0

#define ENDLINE "\n"
#define TAB "\t"
#define RETURN "\r"





//Inititialization of UART peripheral
// 9600 baud, no parity, no flow control, 1 stop bit
void uarte_init(void); 
/*
  TXD->P0.06
  RXD->P0.08
  CTS->P0.07
  RTS->P0.05
*/




//send SENSORS struct using UART
void UART_sendSensorData(SENSORS*);

//Function used by sendSensorData_UART function
// the function put one float variable into the tx buffer for transmit UART
int putDataIntoArray (float , int );

uint32_t UART_recibirTiempo(void);//Funcion para introducir la hora




void enviar_cadena(char data[]); 
void enviar_int(int valor);//Para enviar valores enteros
void enviar_hex(int valor);//Para enviar valores en formato hex
void CR(void);//Retorno de carro para legibilidad
void LF(void);//Salto de linea para legitividad
void readUART1(void); //Funcion de lectura puerto UART1

//--------GPIOTE_IRQHandler(void)----------------//
/*
Handler de interrupciones externas
Revisar defines.h para conocer los puertos habilitados 
*/

//---------RTC0_IRQHandler(void)----------------//
/*
Handler de interrupcion del RTC
Revisar defines.h para conocer los puertos habilitados 
*/


#endif