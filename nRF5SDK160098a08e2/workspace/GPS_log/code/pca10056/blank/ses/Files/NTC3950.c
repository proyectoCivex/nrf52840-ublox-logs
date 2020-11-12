#include "ADC.h"
#include "NTC3950.h"
#include "CommonHeaders.h"

void initNTC3950()
{
  adcInitOneChannel(NTCPIN);
}

float getVoltage ()
{
  int adcResult = readAnalog(NTCPIN);

    //RESULT = V*GAIN/REFERENCE*(2^resolution-m))
        //gain 1/6
        //reference internal = 0.6 V
        //resolution 14
        //m =0 because single-ended
  //RESULT = V/6/0.6*2^4

  float voltage= adcResult*6.0*0.6/16384; //(in volts)*/
  return voltage;
}

float getTempKelvin ()
{
  double invTemp;
  double invTemp2;
  float temperature;
  //Rt= Rbajo (Vin/Vout -1)
  // Vin=3.3, Rbajo= 10 Kohms
  double resistance = 100*(3.3/getVoltage()-1);
 
  double lnR= log (resistance);
  invTemp = -5.16*pow(10,-7)*pow (lnR,3)+1.01*pow(10,-5)*pow(lnR,2)+2*pow(10,-4)*lnR+2.27*pow(10,-3);
  invTemp2 = 0.0003*log(resistance)+ 0.0022;
  temperature = 1/invTemp;
  return temperature;
}

float getTempCelsius ()
{
  return getTempKelvin()-273;
}


