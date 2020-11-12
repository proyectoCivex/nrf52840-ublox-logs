#include "RF.h"


uint8_t data_packet[PAYLOAD];   //for transmit and receive



void initRF(void){

  //need high frequency clock for running
  start_clock();
  
  NRF_RADIO->FREQUENCY = 20;//2440MHz
  NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Pos4dBm<<RADIO_TXPOWER_TXPOWER_Pos);

  // Configure radio with 1Mbit Nordic proprietary mode
  NRF_RADIO->MODE = (RADIO_MODE_MODE_Nrf_1Mbit<<RADIO_MODE_MODE_Pos);

  // Configure address Prefix0 + Base0
  NRF_RADIO->PREFIX0 = 0x00000024;
  NRF_RADIO->BASE0 = 0x35f29301;

 // Use logical address 0 (BASE0 + PREFIX0 byte 0)
  NRF_RADIO->TXADDRESS=0 << RADIO_TXADDRESS_TXADDRESS_Pos;
  NRF_RADIO->RXADDRESSES=RADIO_RXADDRESSES_ADDR0_Enabled<<RADIO_RXADDRESSES_ADDR0_Pos;


 // Configure packet with  S0,S1 and Length fields and 16-bit preamble.
  NRF_RADIO->PCNF0 = (LENGHT<<RADIO_PCNF0_LFLEN_Pos)
                     |(S0<<RADIO_PCNF0_S0LEN_Pos)
                     |(S1<<RADIO_PCNF0_S1LEN_Pos)
                     |(RADIO_PCNF0_S1INCL_Automatic<<RADIO_PCNF0_S1INCL_Pos)
                     |(C1<<RADIO_PCNF0_CILEN_Pos)
                     |(RADIO_PCNF0_PLEN_16bit<<RADIO_PCNF0_PLEN_Pos)
                     |(RADIO_PCNF0_CRCINC_Exclude<<RADIO_PCNF0_CRCINC_Pos)
                     |(TERM1<<RADIO_PCNF0_TERMLEN_Pos);

 // Configure static payload length of PAYLOAD bytes, 2 bytes address, little endian with whitening enabled.
  NRF_RADIO->PCNF1 = (PAYLOAD<<RADIO_PCNF1_MAXLEN_Pos)
                     |(PAYLOAD<<RADIO_PCNF1_STATLEN_Pos)
                     |(3<<RADIO_PCNF1_BALEN_Pos)
                     |(RADIO_PCNF1_ENDIAN_Little<<RADIO_PCNF1_ENDIAN_Pos)
                     |(RADIO_PCNF1_WHITEEN_Enabled<<RADIO_PCNF1_WHITEEN_Pos);
  


  NRF_RADIO->MODECNF0 = (RADIO_MODECNF0_RU_Default<<RADIO_MODECNF0_RU_Pos);

  // Initialize CRC (two bytes)
  NRF_RADIO->CRCCNF = (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos);

  if ((NRF_RADIO->CRCCNF & RADIO_CRCCNF_LEN_Msk) == (RADIO_CRCCNF_LEN_Three << RADIO_CRCCNF_LEN_Pos)){
      NRF_RADIO->CRCINIT = 0xFFFFFFUL;   // Initial value
      NRF_RADIO->CRCPOLY = 0x912A1UL;  // CRC poly: x^16 + x^12^x^5 + 1
  }
  else if ((NRF_RADIO->CRCCNF & RADIO_CRCCNF_LEN_Msk) == (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos))
  {
      NRF_RADIO->CRCINIT = 0xFFFFUL;   // Initial value
      NRF_RADIO->CRCPOLY = 0x11021UL;  // CRC poly: x^16 + x^12^x^5 + 1
  }
  else if ((NRF_RADIO->CRCCNF & RADIO_CRCCNF_LEN_Msk) == (RADIO_CRCCNF_LEN_One << RADIO_CRCCNF_LEN_Pos))
  {
      NRF_RADIO->CRCINIT = 0xFFUL;   // Initial value
      NRF_RADIO->CRCPOLY = 0x107UL;  // CRC poly: x^8 + x^2^x^1 + 1
  }

  //packet pointer (packet address to be used for the next tranmission or reception)
  NRF_RADIO->PACKETPTR=(uint32_t)&data_packet[0];   

  //Radio on
  NRF_RADIO->POWER=1;


}

void start_clock()
{
  /* Start 16 MHz crystal oscillator */
  NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
  NRF_CLOCK->TASKS_HFCLKSTART=1;

   /* Wait for the external oscillator to start up */
  while(NRF_CLOCK->EVENTS_HFCLKSTARTED==0){};
  NRF_CLOCK->EVENTS_HFCLKSTARTED=0;
}

void disableRF(void){
  //stop 16 MHz cyrstal oscillator
  NRF_CLOCK->TASKS_HFCLKSTOP=1;
  //power off RF
  NRF_RADIO->POWER=0;
}







////////////////////////////////////////////////////////////////////
////////////////////////////////////SEND//////////////////////////////////
////////////////////////////////////////////////////////////////////


void RF_sendUint32(int idData, uint32_t timeData)
{
   uint8_t frameToSend[5];
    //Convert TIMESTAMP time (uint32) to 4 uint8
    frameToSend[0]= (uint8_t)idData;
    frameToSend[1]= (uint8_t)((timeData&0xFF000000)>>24);
    frameToSend[2]= (uint8_t)((timeData&0x00FF0000)>>16);
    frameToSend[3]= (uint8_t)((timeData&0x0000FF00)>>8);
    frameToSend[4]= (uint8_t)((timeData&0x000000FF));
     
    RF_sendFrame(frameToSend);
}

void RF_sendFloat (int idData, float floatData)
{
    struct LIT_ENDIAN litendian;
    uint8_t frameToSend[PAYLOAD]={0};

    litendian.BE_FLOAT.all=floatData;

    frameToSend[0]= (uint8_t)(idData);
    frameToSend[1]= litendian.BE_FLOAT.bytes.byte1;
    frameToSend[2]= litendian.BE_FLOAT.bytes.byte2;
    frameToSend[3]= litendian.BE_FLOAT.bytes.byte3;
    frameToSend[4]= litendian.BE_FLOAT.bytes.byte4;
    RF_sendFrame(frameToSend);

  
}



void RF_sendFrame(uint8_t send_data[]){

    for(int i=0; i<PAYLOAD; i++){
      data_packet[i]=send_data[i];
    }


  // Configure shortcuts to start as soon as READY event is received, and disable radio as soon as packet is sent.
  NRF_RADIO->SHORTS = (RADIO_SHORTS_READY_START_Enabled << RADIO_SHORTS_READY_START_Pos) |
                      (RADIO_SHORTS_END_DISABLE_Enabled << RADIO_SHORTS_END_DISABLE_Pos);
  
  // send packet
    NRF_RADIO->TASKS_TXEN = 1;
    while (NRF_RADIO->EVENTS_DISABLED == 0);
    NRF_RADIO->EVENTS_DISABLED = 0;
   
}


void RF_sendSensorData (SENSORS* data)
{
  RF_sendFloat(ID_TEMPAMB, data->temp_amb);
  RF_sendFloat(ID_TEMPVACA, data->temp_vaca);
  RF_sendFloat(ID_IMUACCX, data->imu_acc_x);
  RF_sendFloat(ID_IMUACCY, data->imu_acc_y);
  RF_sendFloat(ID_IMUACCZ, data->imu_acc_z);
  RF_sendFloat(ID_IMUGIROX, data->imu_giro_x);
  RF_sendFloat(ID_IMUGIROY, data->imu_giro_y);
  RF_sendFloat(ID_IMUGIROZ, data->imu_giro_z);

}




void RF_SendKeepAlive ()
{
    RF_sendUint32(ID_KA, KEEPALIVE);
}



////////////////////////////////////////////////////////////////////
////////////////////////////////////RECEIVE//////////////////////////////////
////////////////////////////////////////////////////////////////////

uint32_t RF_receiveTime(void){

  // Configure shortcuts to start as soon as READY event is received, 
  //and disable radio as soon as packet is received.
  uint32_t time;


                       //Shortcut between event READY and task START
  NRF_RADIO->SHORTS = (RADIO_SHORTS_READY_START_Enabled << RADIO_SHORTS_READY_START_Pos) |
                      // Shortcut between event END and task DISABLE
                      (RADIO_SHORTS_END_DISABLE_Enabled << RADIO_SHORTS_END_DISABLE_Pos);
  
  // Continually receive packet
    NRF_RADIO->TASKS_RXEN = 1;
    while (NRF_RADIO->EVENTS_DISABLED == 0);
    NRF_RADIO->EVENTS_DISABLED = 0;
    


    if (data_packet[0]==ID_TIMESTAMP)
    {
     time = (uint32_t)data_packet[1] << 24 |
      (uint32_t)data_packet[2] << 16 |
      (uint32_t)data_packet[3] << 8  |
      (uint32_t)data_packet[4];
    }
    else
      time= RF_receiveTime();



    return time;
}

bool RF_receiveKeepAlive(void){

  // Configure shortcuts to start as soon as READY event is received, 
  //and disable radio as soon as packet is received.



                       //Shortcut between event READY and task START
  NRF_RADIO->SHORTS = (RADIO_SHORTS_READY_START_Enabled << RADIO_SHORTS_READY_START_Pos) |
                      // Shortcut between event END and task DISABLE
                      (RADIO_SHORTS_END_DISABLE_Enabled << RADIO_SHORTS_END_DISABLE_Pos);
  
  // Continually receive packet
    NRF_RADIO->TASKS_RXEN = 1;
    while (NRF_RADIO->EVENTS_DISABLED == 0);
    NRF_RADIO->EVENTS_DISABLED = 0;
    

  uint32_t data = (uint32_t)data_packet[1] << 24 |
      (uint32_t)data_packet[2] << 16 |
      (uint32_t)data_packet[3] << 8  |
      (uint32_t)data_packet[4];

    if (data_packet[0]==ID_KA)
    {
     if (data==KEEPALIVE)
      return true;
    }
    else
      return false;

}





//receive dataSensors
SENSORS RF_receiveDataSensor (void)
{
  SENSORS receivedData;

  for (int i=0; i<8; i++)
  {
    RF_receive(&receivedData);
  }
  return receivedData;

}



void RF_receive(SENSORS* data){

  struct LIT_ENDIAN litendian;

  // Configure shortcuts to start as soon as READY event is received, 
  //and disable radio as soon as packet is received.


                       //Shortcut between event READY and task START
   NRF_RADIO->SHORTS = (RADIO_SHORTS_READY_START_Enabled << RADIO_SHORTS_READY_START_Pos) |
                      // Shortcut between event END and task DISABLE
                      (RADIO_SHORTS_END_DISABLE_Enabled << RADIO_SHORTS_END_DISABLE_Pos);
  
  // Continually receive packet
    NRF_RADIO->TASKS_RXEN = 1;
    while (NRF_RADIO->EVENTS_DISABLED == 0);
    NRF_RADIO->EVENTS_DISABLED = 0;



    litendian.BE_FLOAT.bytes.byte1=data_packet[1];
    litendian.BE_FLOAT.bytes.byte2=data_packet[2];
    litendian.BE_FLOAT.bytes.byte3=data_packet[3];
    litendian.BE_FLOAT.bytes.byte4=data_packet[4];


    switch (data_packet[0])
    {
      case ID_TEMPAMB:
        data->temp_amb = litendian.BE_FLOAT.all;
        break;
      case ID_TEMPVACA:
        data->temp_vaca = litendian.BE_FLOAT.all;
        break;
      case ID_IMUACCX:
        data->imu_acc_x = litendian.BE_FLOAT.all;
        break;
      case ID_IMUACCY:
        data->imu_acc_y = litendian.BE_FLOAT.all;
        break;
      case ID_IMUACCZ:
        data->imu_acc_z = litendian.BE_FLOAT.all;
        break;
      case ID_IMUGIROX:
        data->imu_giro_x = litendian.BE_FLOAT.all;
        break;
      case ID_IMUGIROY:
        data->imu_giro_y = litendian.BE_FLOAT.all;
        break;
      case ID_IMUGIROZ:
        data->imu_giro_z = litendian.BE_FLOAT.all;
        break;
    }


  /*
    for(int i=0; i<sizeof(data_packet); ++i) data_packet[i]=0x00;

    NRF_RADIO->EVENTS_READY = 0;
    // Enable radio and wait for ready
    NRF_RADIO->TASKS_RXEN = 1;

    while (NRF_RADIO->EVENTS_READY == 0){};   //WAIT
    NRF_RADIO->EVENTS_END = 0;
     // Start listening and wait for address received event
    NRF_RADIO->TASKS_START = 1;

   // Wait for end of packet 
    while (NRF_RADIO->EVENTS_END == 0){};

    NRF_RADIO->EVENTS_RSSIEND = 0;
    NRF_RADIO->TASKS_RSSISTART = 1;
    while(NRF_RADIO->EVENTS_RSSIEND == 0){};

   
    if (NRF_RADIO->CRCSTATUS == 1U)   //ok
    {
      for (int i=0; i<PAYLOAD-1; i++)
        result[i] = data_packet[i];
    }

    NRF_RADIO->EVENTS_DISABLED = 0;
      // Disable radio
    NRF_RADIO->TASKS_DISABLE = 1;
    while (NRF_RADIO->EVENTS_DISABLED == 0){};// wait
    */

}




