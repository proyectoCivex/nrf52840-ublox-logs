#ifndef _NTC3950_H
#define _NTC3950_H


#define NTCPIN AIN0 //PIN 0.02 -- 0


//Get the voltage of read by the ADC in NTCPIN
float getVoltage ();


//Get tempearture in kelvin
float getTempKelvin ();

//Get tempearture in celsius degrees
float getTempCelsius ();

//init ADC
void initNTC3950();




#endif