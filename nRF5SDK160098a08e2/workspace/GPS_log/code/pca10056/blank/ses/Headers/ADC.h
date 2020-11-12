#ifndef _ADC_H
#define _ADC_H

#include <nrfx.h>

#define SAMPLES_BUFFER 8

#define AIN0 0      //pin p0.02
#define AIN1 1
#define AIN2 2
#define AIN3 3
#define AIN4 4
#define AIN5 5
#define AIN6 6
#define AIN7 7

//ALL CHANNELS
void adcInitAll(void);
/*
Funcion para inicilizar la lectura de canales analogicos
Configuracion:
  Resolucion: 2^14
  Ganancia: 1/6
  TACQ: 3uS
  Modo: Single endeded, Brust OFF
  Referencia interna de 0.6V
  Resistencias de bypass en canal postivo y negativo.
  Auto-calibracion
*/

int readAnalogs(int pin);
/*
Non-void function, realiza la lectura del canal analogico designado en el argumento.
Argumento:
  int pin   Pin analogico a leer, se recomuenda el uso de #defines 
Return:
  Devuelve el voltage del pin analógico
*/


//only one channel
void adcInitOneChannel(int);
int readAnalog(int);


#endif

