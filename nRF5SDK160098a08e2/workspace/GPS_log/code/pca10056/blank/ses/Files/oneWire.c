#include "oneWire.h"



void initOneWire ()    
{
  //Init GPIO for communication
  NRF_P0->PIN_CNF[PIN_ONEWIRE] = 
      (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)   //Pin sensing mechanism disable
    | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)        //Drive configuration : standar '0', standar '1'
    | (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos)        //Pull up on pin
    | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)     //Connect input buffer
    | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);         //Input for example (does not matter)


}


void resetWire(){

/*During the initialization sequence the bus master transmits (TX) the reset pulse by pulling the 1-Wire bus
low for a minimum of 480µs. The bus master then releases the bus and goes into receive mode (RX).
When the bus is released, the 5k? pullup resistor pulls the 1-Wire bus high. When the DS18B20 detects
this rising edge, it waits 15µs to 60µs and then transmits a presence pulse by pulling the 1-Wire bus low
for 60µs to 240µs.*/

    //__disable_irq();
    goOutput();
    nrf_delay_us(480);
    goInput();
    nrf_delay_us(140);
    if(NRF_P0->IN & (1<<PIN_ONEWIRE))
    {
     nrf_delay_us(350);
     return;
    }
    else
    {
    nrf_delay_us(500);
    resetWire();
    }
     
   //__enable_irq();
}


void OnewireSendConvertTempCommand(){


  //Send comand to convert the temperature
  resetWire();
  SendCMD(SKIP_ROM);    
  SendCMD(CONVER_TEMP);
 
}

float OnewireReceivedTemp ()
{
  uint16_t rawTemp;
  
  //wait until receive 1 (conversion done!)
  while(!ReadBit());  

  //Read the temperature
  resetWire();
  SendCMD(SKIP_ROM);
  SendCMD(READ_SCRATCH);

  rawTemp = ReadByte();
  rawTemp |= (ReadByte() << 8);

  return complement2_12bitsResolution(rawTemp);
}


float OnewireReadTemp(){
/*
After the Convert T command and the DS18B20 will respond by transmitting
0 while the temperature conversion is in progress and 1 when the conversion is done.
If the DS18B20 is powered with parasite power, this notification technique cannot
be used since the bus must be pulled high by a strong pullup during the entire temperature conversion.
Neecesary to wait until the 750 ms ( max temperature conversion time for 12-bits resolution)
We are powering by external supply
*/

  OnewireSendConvertTempCommand();
  return OnewirereceivedTemp();
}



float complement2_12bitsResolution (uint16_t rawData)
{
  float temp;
  //correct the sign (complemento a 2 - 12 bits the resolución)
  if ((rawData & 0x8000)>0)    //negative
  {
    temp = ((rawData ^ 0xffff) + 1) * -1;
  }
  else    //positive
  {
    temp=(float)rawData;
  }
    
  return (temp / 16.0);
}

void goInput ()
{
 NRF_P0->DIRCLR=(1<<PIN_ONEWIRE);//Entrada
}


void goOutput(){    //cambiar nombre de la función
  NRF_P0->DIRSET=(1<<PIN_ONEWIRE);//Salida
}


/*Note for writting and reding data:
All write/read time slots must be a minimum of 60µs in duration with a minimum of a 1µs recovery
time between individual write slots.*/

void SendCMD (uint8_t comand)
{
  for (int i = 0; i < 8; i++) {
    goOutput();
    if (comand & 0x01) {
      nrf_delay_us(10);
      goInput();
      nrf_delay_us(60);
    } else {
      nrf_delay_us(60);
      goInput();
      nrf_delay_us(10);
    }
    comand >>= 1;
  }
}

int ReadBit (){
  int inputState;

  goOutput();
  nrf_delay_us(6);
  goInput();
  nrf_delay_us(9);
  inputState=  ((NRF_P0->IN>>PIN_ONEWIRE) & 0x01);
    //Another way:
  //Get the value set in the 13th bit and shift it to get 1 or 0
  //input = (NRF_P0->IN>>PIN_ONEWIRE) & 1UL;
  nrf_delay_us(55);
  return inputState;
}

int ReadByte (){
  int byte = 0;
  int bit = 1;

  do {
    if (ReadBit()) {
      byte |= bit;
    }
    bit <<= 1;
  } while (0x100 != bit);
  return byte;
}




//no chequeado
void ReadSerialNumber(){
  uint8_t serialNumber[8];
  uint8_t id[6];
  int rv = -1;

  resetWire();
  SendCMD(READ_ROM);
   do {
    resetWire();
    SendCMD(READ_ROM);
    for (int i = 0; i < sizeof(serialNumber); ++i) {
      serialNumber[i] = ReadByte();
    }
    rv = 0;
  } while (0);

  if (rv==0) {
    if (0 != iBSPACMonewireComputeCRC(serialNumber, sizeof(serialNumber))) {
      rv = -1;
    } else {
      for (int i = 0; i < sizeof(id); ++i) {
        id[i] = serialNumber[sizeof(serialNumber) - 2 - i];
      }
    }
  }
  nrf_delay_ms(100);
}


//no chequeado
int iBSPACMonewireComputeCRC (const unsigned char * romp,int len){
  static const unsigned char OW_CRC_POLY = 0x8c;
  unsigned char crc = 0;

  while (0 < len--) {
    int bi;
    crc ^= *romp++;
    for (bi = 0; bi < 8; ++bi) {
      if (crc & 1) {
        crc = (crc >> 1) ^ OW_CRC_POLY;
      }
      else {
        crc >>= 1;
      }
    }
  }

  return (int)crc;
}