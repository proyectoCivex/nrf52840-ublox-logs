#ifndef _SPI_H
#define _SPI_H

#include "CommonHeaders.h"

uint8_t spi_send(char data);
/*
Funcion para enviar comando por SPI
Tipo: Non-void function
Return MISO data
*/

void spi_init(void);
/*
Funcion que inicializa puerto SPI.
Configuracion:

  CS -> P1.12
  MOSI -> P1.13
  MISO -> P1.14
  SCK -> P1.15

  SPI Mode: 0
  -> Leading: 0
  -> Active High: 0
  
  MSB First

  CS idle state: LOW
*/

#endif