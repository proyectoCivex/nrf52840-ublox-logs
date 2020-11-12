#ifndef _GPS_H
#define _GPS_H

#include "definitions.h"


/**********************************/
// SIN CHEQUEAR
/**********************************/

#define RXD_GPS 128 //Buffer maximo de lectura 50
#define TXD_GPS 128 //Buffer maximo de lectura 50

extern uint8_t rx_gps[RXD_GPS]; //Buffer RX UART0
extern uint8_t tx_gps[TXD_GPS];//Buffer Rx UART1


void uarte1_init(void);
/*
Funcion que iniciliza el puerto UART1 con las mismas caracteristas que el UART0
Recomendado su uso para GPS
Configuracion:
  TXD->P1.01
  RXD->P1.02

  Configurado a 9600 baud
  Sin paridad
  Sin control de flujo
  1 bit de parada

  Sin interrupcion de lecutra --> Recurrir a funcion readUART1()

  NO SE HA REALIZADO FUNCION DE ESCRITURA PARA ESTE PUERTO!!!!!!!
*/


void findAndfill(void);


#endif