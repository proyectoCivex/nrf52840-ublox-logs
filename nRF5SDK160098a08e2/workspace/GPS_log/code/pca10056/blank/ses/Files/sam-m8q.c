#include "sam_m8q.h"
#include "sam_m8q_definitions.h"
#include "i2c.h"

#define MAX_PAYLOAD_SIZE 256
//#define DEBUG_PRINT 

//Init the packet structures and init them with pointers to the  payloadCfg arrays
uint8_t payloadCfg[MAX_PAYLOAD_SIZE]; // Holds the requested data packet
ubxPacket packetCfg = {0, 0, 0, payloadCfg, 0, 0, SFE_UBLOX_PACKET_VALIDITY_NOT_DEFINED, SFE_UBLOX_PACKET_NOT_DEFINED};



void gpsInit ()
{
  //init i2c port
  i2cInit(GPS_ADDRESS);

  //configuration
  setPortOutput(COM_PORT_I2C, COM_TYPE_UBX);  //Set the I2C port to output UBX only (turn off NMEA noise)
  

  /*
  * Función para solo obtener datos de Galileo (no funciona)
  */
  //set only one GNSS (needed for PSMCT low power mode)
  setGNSSConfig(GNSSID_GALILEO);      

 
 /*
 *  Funciones para la configuración en bajo consumo  (no funciona)
 */
  powerSaveMode();       //power save mode
  setPSMCTmode();    //select Power Save Mode Cyclic Training (PSMCT)
  
  //creating log
  createLogFile();
  ConfigLogFile();

  //Save the current settings to flash and BBR
  saveConfiguration(); 

  //poll a PVT request
   getNAV_PVT();

}




//The function is the other way around
// You should poll the NAV_PVT request, and then check the answer (checkpollrequest)
// However, the GPS last one seconds between the polling and the answer
// For this reason, in each cycle (period of 1 seconds), we get the answer and then poll the new request
// so there is no need of waiting
void samM8Q_getPosition()
{  
  //get teh position
  checkPollRequest();
  //poll request
  getNAV_PVT();

}


//   FUNCIONA EN CONFIG
sfe_ublox_packet_buffer_type checkPollRequest()
{
  uint16_t num =0;
  uint16_t tries=0;
  while (num==0 && tries<2 )
  {
      num=readBytesAvailable();


      if (num != 0)
      {
          NRF_LOG_INFO("Getting poll data");
          getPollData(num,  &packetCfg, packetCfg.cls,  packetCfg.id);
      }
      else
      {
          #ifdef DEBUG_PRINT
          if (num==-1)
          {
            NRF_LOG_INFO("Invalid answer");
          } 
          else if (num==0)
          {
            NRF_LOG_INFO("No bytes available");
          }  
          #endif

          NRF_LOG_INFO("delay");
          nrf_delay_ms (100);
          tries++;
      }     
  }

  return packetCfg.type;
}




//Configure a given port to output UBX, NMEA, RTCM3 or a combination thereof
//Port 0=I2c, 1=UART1, 2=UART2, 3=USB, 4=SPI
//Bit:0 = UBX, :1=NMEA, :5=RTCM3
void setPortOutput(uint8_t portID, uint8_t outProtoMask)
{
  //First, get the PortOutput configuration
  getPortSettings(portID);

  if (packetCfg.valid == SFE_UBLOX_PACKET_VALIDITY_VALID)
  {
    //Then, set new port configurations  
    packetCfg.len = 20;   //change the lenght
    //payloadCfg is now loaded with current bytes. Change only the ones we need to
    packetCfg.payload[14] = outProtoMask; //OutProtocolMask LSB - Set outStream bits
    packetCfg.payload[16]= COM_BITFIELD_FLAGS;

    //send message
    sendi2cUBXMessage(&packetCfg);

    //Get and process answer when new data is available
    if (checkPollRequest()== SFE_UBLOX_PACKET_PACKETACKACK)  // We are only expecting an ACK
    {
       NRF_LOG_INFO("PORT OUTPUT CONFIGURATION SET");
    }
    else
    {
        NRF_LOG_INFO("ERROR WHEN SETTING PORT OUTPUT CONFIGURATION");
        if (packetCfg.type == SFE_UBLOX_PACKET_PACKETACKNAK)
        {
          NRF_LOG_INFO("Received an ACK-NACK answer");
        }
    }
  }
  else
  {
    NRF_LOG_INFO("Set port output can not be done");
  }

}



void getPortSettings (uint8_t portID)
{  
  //Poll request

  //Creating message
  packetCfg.cls = UBX_CLASS_CFG;
  packetCfg.id = UBX_CFG_PRT;
  packetCfg.len = 1;
  payloadCfg[0] = portID;
  
  //Sending message 
  sendi2cUBXMessage(&packetCfg);

  //Get and process answer when new data is available
  if (checkPollRequest()==SFE_UBLOX_PACKET_PACKETCFG)
  {
    NRF_LOG_INFO("GET PORT SETTINGS - CORRECT");
  }
  else
  {
    NRF_LOG_INFO("ERROR WHEN GETTING PORT SETTINGS");
  }

}



//Save current configuration to flash and BBR (battery backed RAM)
void saveConfiguration()
{
  //Creating message
  packetCfg.cls = UBX_CLASS_CFG;
  packetCfg.id = UBX_CFG_CFG;
  packetCfg.len = 12;

  //Clear packet payload
  for (uint8_t x = 0; x < packetCfg.len; x++)
    packetCfg.payload[x] = 0;

  packetCfg.payload[4] = 0xFF; //Set any bit in the saveMask field to save current config to Flash and BBR
  packetCfg.payload[5] = 0xFF;

  //Send message
  sendi2cUBXMessage(&packetCfg); 

  //Get and process answer when new data is available
  if (checkPollRequest()== SFE_UBLOX_PACKET_PACKETACKACK)  // We are only expecting an ACK
  {
     NRF_LOG_INFO(" CONFIGURATION SAVED");
  }
  else
  {
      NRF_LOG_INFO("ERROR WHEN SAVING  CONFIGURATION");
              if (packetCfg.type == SFE_UBLOX_PACKET_PACKETACKNAK)
        {
          NRF_LOG_INFO("Received an ACK-NACK answer");
        }
  }


}

//Power Save Mode (Enables/Disables Low Power Mode using UBX-CFG-RXM)
void powerSaveMode()
{
   //get power mode
   getPowerMode();

  //Create new message
  payloadCfg[1] = 1;  // Power Save Mode
 //payloadCfg[1] =0;    //Continuos  mode
  packetCfg.len = 2;


  sendi2cUBXMessage(&packetCfg); 
  
  //Get and process answer when new data is available
  if (checkPollRequest()== SFE_UBLOX_PACKET_PACKETACKACK)  // We are only expecting an ACK
  {
     NRF_LOG_INFO("POWER SAVED MODE SET");
  }
  else
  {
    NRF_LOG_INFO("ERROR WHEN SETTING POWER MODE CONFIGURATION");
            if (packetCfg.type == SFE_UBLOX_PACKET_PACKETACKNAK)
        {
          NRF_LOG_INFO("Received an ACK-NACK answer");
        }
  }

}


// Get Power Save Mode - Returns the current Low Power Mode using UBX-CFG-RXM
void getPowerMode()
{
  // Read the power setting using UBX-CFG-RXM
  packetCfg.cls = UBX_CLASS_CFG;
  packetCfg.id = UBX_CFG_RXM;
  packetCfg.len = 0;

  //Ask module for the current power management settings. Loads into payloadCfg.
  sendi2cUBXMessage(&packetCfg);

  //Get and process answer when new data is available
 if (checkPollRequest()==SFE_UBLOX_PACKET_PACKETCFG)
 {
    if (payloadCfg[1] == 0 )
    {
     NRF_LOG_INFO("Current configuration: Continuos  mode" );
    }
    else if(payloadCfg[1] == 1 )
    {
     NRF_LOG_INFO("Current configuration: Power save mode" );
    }
    else
    {
     NRF_LOG_INFO("Current configuration: Other power mode %d", payloadCfg[1] );
    }
 }
 else
 {
    NRF_LOG_INFO("ERROR when getting the current power save mode" );
 }



 
}




//Power Save Mode (Enables/Disables Low Power Mode using UBX-CFG-RXM)
void setPSMCTmode()
{
   //get power mode
   getSaveMode();

  //Create new message
  payloadCfg[18] = 1;  // PSMCT
 //payloadCfg[1] =0;    //PSMOO

  //used the previous len (datasheet explicit that is 44, but received 48)
  //packetCfg.len = 44;   


  sendi2cUBXMessage(&packetCfg); 
  
  //Get and process answer when new data is available
  if (checkPollRequest()== SFE_UBLOX_PACKET_PACKETACKACK)  // We are only expecting an ACK
  {
     NRF_LOG_INFO("PSMCT SAVED MODE SET");
  }
  else
  {
    NRF_LOG_INFO("ERROR WHEN SETTING POWER MODE CONFIGURATION");
            if (packetCfg.type == SFE_UBLOX_PACKET_PACKETACKNAK)
        {
          NRF_LOG_INFO("Received an ACK-NACK answer");
        }
  }


 
}


// Get Power Save Mode - Returns the current Low Power Mode using UBX-CFG-RXM
void getSaveMode()
{
  // Read the power setting using UBX-CFG-PM2
  packetCfg.cls = UBX_CLASS_CFG;
  packetCfg.id = UBX_CFG_PM2;   //Extended power management configuration
  packetCfg.len = 0;

  //Ask module for the current power management settings. Loads into payloadCfg.
  sendi2cUBXMessage(&packetCfg);


  //Get and process answer when new data is available
   if (checkPollRequest()==SFE_UBLOX_PACKET_PACKETCFG)
   {
      if (payloadCfg[18] == 0 )
      {
       NRF_LOG_INFO("Current configuration: ON/OFF (PSMOO)" );
      }
      else if(payloadCfg[18] == 1 )
      {
       NRF_LOG_INFO("Current configuration: PSMCT" );
      }
      else
      {
       NRF_LOG_INFO("Current configuration: Other power mode %d", payloadCfg[1] );
      }
   }
   else
   {
      NRF_LOG_INFO("ERROR when getting the current power management settings" );
   }



}



/***********************************/


//Power Save Mode (Enables/Disables Low Power Mode using UBX-CFG-RXM)
void setGNSSConfig(uint8_t gnnsId)
{

   getGNSSConfig();

   
   //disable all GNSS except one
   //because PSMCT only support one main GNSS (GPS, GLONASS, DeiBou, Galileo or QZSS). Moreover, it is recommeded to disable both SBAS and IMES 
  
   for (int n=0; n<=6; n++)
   {
      if (packetCfg.payload [4+8*n]==gnnsId)    
      {
          packetCfg.payload [8+8*n]= 1;   //Enable GNSS with gnnsID 
       }
       else
       {
        packetCfg.payload [8+8*n]= 0;   //disable the GNSS 
       }
   } 

   
  sendi2cUBXMessage(&packetCfg); 
  
  //Get and process answer when new data is available
  if (checkPollRequest()== SFE_UBLOX_PACKET_PACKETACKACK)  // We are only expecting an ACK
  {
     NRF_LOG_INFO("ONLY USING ONE GNSS SET");
  }
  else
  {
    NRF_LOG_INFO("ERROR WHEN SETTING ONLY ONE GNSS");
            if (packetCfg.type == SFE_UBLOX_PACKET_PACKETACKNAK)
        {
          NRF_LOG_INFO("Received an ACK-NACK answer");
        }
  }


 
}


// Get Power Save Mode - Returns the current Low Power Mode using UBX-CFG-RXM
void getGNSSConfig()
{
  // Read the power setting using UBX-CFG-PM2
  packetCfg.cls = UBX_CLASS_CFG;
  packetCfg.id = UBX_CFG_GNSS;   ////GNSS system configuration
  packetCfg.len = 0;

  //Ask module for the current power management settings. Loads into payloadCfg.
  sendi2cUBXMessage(&packetCfg);


  //Get and process answer when new data is available
   if (checkPollRequest()==SFE_UBLOX_PACKET_PACKETCFG)
   {
    
       NRF_LOG_INFO("getting current gnss config" );
   }
   else
   {
      NRF_LOG_INFO("ERROR when getting the current gnss config" );
   }

}


void factoryReset()
{
  // Copy default settings to permanent
  // Note: this does not load the permanent configuration into the current configuration. Calling factoryDefault() will do that.
  packetCfg.cls = UBX_CLASS_CFG;
  packetCfg.id = UBX_CFG_CFG;
  packetCfg.len = 13;

  for (uint8_t i = 0; i < 4; i++)
  {
    payloadCfg[0 + i] = 0xff; // clear mask: copy default config to permanent config
    payloadCfg[4 + i] = 0x00; // save mask: don't save current to permanent
    payloadCfg[8 + i] = 0x00; // load mask: don't copy permanent config to current
  }
  payloadCfg[12] = 0xff;     // all forms of permanent memory
  sendi2cUBXMessage(&packetCfg); // don't expect ACK
  hardReset();               // cause factory default config to actually be loaded and used cleanly
}

void hardReset()
{
  // Issue hard reset
  packetCfg.cls = UBX_CLASS_CFG;
  packetCfg.id = UBX_CFG_RST;
  packetCfg.len = 4;
  payloadCfg[0] = 0xff;      // cold start
  payloadCfg[1] = 0xff;      // cold start
  payloadCfg[2] = 0;         // 0=HW reset
  payloadCfg[3] = 0;         // reserved
  sendi2cUBXMessage(&packetCfg); // don't expect ACK
}


//Reset module to factory defaults
//This still works but it is the old way of configuring ublox modules. See getVal and setVal for the new methods
void factoryDefault()
{
  packetCfg.cls = UBX_CLASS_CFG;
  packetCfg.id = UBX_CFG_CFG;
  packetCfg.len = 12;

  //Clear packet payload
  for (uint8_t x = 0; x < packetCfg.len; x++)
    packetCfg.payload[x] = 0;

  packetCfg.payload[0] = 0xFF; //Set any bit in the clearMask field to clear saved config
  packetCfg.payload[1] = 0xFF;
  packetCfg.payload[8] = 0xFF; //Set any bit in the loadMask field to discard current config and rebuild from lower non-volatile memory layers
  packetCfg.payload[9] = 0xFF;

  sendi2cUBXMessage(&packetCfg);    // We are only expecting an ACK
 
   //Get and process answer when new data is available
  if (checkPollRequest()== SFE_UBLOX_PACKET_PACKETACKACK)  // We are only expecting an ACK
  {
     NRF_LOG_INFO("FACTORY DEFAULT DONE");
  }
  else
  {
    NRF_LOG_INFO("ERROR WHEN SETTING FACTORY DEFAULT SETTINGS");
            if (packetCfg.type == SFE_UBLOX_PACKET_PACKETACKNAK)
        {
          NRF_LOG_INFO("Received an ACK-NACK answer");
        }
  }

}

void createLogFile ()
{
   // Issue hard reset
  packetCfg.cls = UBX_CLASS_LOG;
  packetCfg.id = UBX_LOG_CREATE;
  packetCfg.len = 8;
  payloadCfg[0] = 0;          // message version (0x00 for this version)
  payloadCfg[1] = 0;          // config flags (==1 log is circular; ==0 log not circular)
  payloadCfg[2] = 0;         // reserved
  payloadCfg[3] = 0;         // logSize (0 for maximum safe size)
  
  //bytes 7-4: set the maximum amount of space (only applicable if logSize==2 - user defined)
  payloadCfg[4] = 0;         // does not care
  payloadCfg[5] = 0;         // does not care
  payloadCfg[6] = 0;         // does not care
  payloadCfg[7] = 0;         // does not care
  sendi2cUBXMessage(&packetCfg); 


  //Get and process answer when new data is available
   if (checkPollRequest()==SFE_UBLOX_PACKET_PACKETACKACK)
   {
    
       NRF_LOG_INFO("Create log file successfully" );
   }
   else
   {
      NRF_LOG_INFO("ERROR when creating log file" );
   }

}


void ConfigLogFile ()
{
   // Issue hard reset
  packetCfg.cls = UBX_CLASS_CFG;
  packetCfg.id = UBX_CFG_LOGFILTER;
  packetCfg.len = 12;

  payloadCfg[0] = 0x01;        // message version (0x01 for this version)
  payloadCfg[1] = 0;          //  flags (only apply recordEnabled)
 
  payloadCfg[2] = 1;         // minInterval (sec)
  payloadCfg[3] = 0;         // minInterval (sec)
  
  payloadCfg[4] = 1;         // timeThreshold (sec)
  payloadCfg[5] = 0;         // timeThreshold (sec)

  payloadCfg[6] = 0;         // speedThreshold -not set
  payloadCfg[7] = 0;         // speedThreshold -not set

  payloadCfg[8] = 0;         // positionThreshold -not set
  payloadCfg[9] = 0;         // positionThreshold -not set
  payloadCfg[10] = 0;        // positionThreshold -not set
  payloadCfg[11] = 0;        // positionThreshold -not set

  sendi2cUBXMessage(&packetCfg); 


  //Get and process answer when new data is available
   if (checkPollRequest()==SFE_UBLOX_PACKET_PACKETACKACK)
   {
    
       NRF_LOG_INFO("Configuration log file successfully" );
   }
   else
   {
      NRF_LOG_INFO("ERROR when creating log file" );
   }

}



//Given a packet and payload, send everything including CRC bytes via I2C port
//Returns false if sensor fails to respond to I2C traffic
void sendi2cUBXMessage(ubxPacket *outgoingUBX)   //i2cWrite_UBXmessage
{ 
  //Write UBXMessage in tx_buffer
  tx_buffer[0]= UBX_SYNCH_1;
  tx_buffer[1]= UBX_SYNCH_2;
  tx_buffer[2]= outgoingUBX->cls;
  tx_buffer[3]= outgoingUBX->id;
  tx_buffer[4]= outgoingUBX->len & 0xFF;
  tx_buffer[5]= outgoingUBX->len >> 8;   //MSB

  uint16_t counter=6;
  for (int i=0; i<outgoingUBX->len; i++)
  {
    tx_buffer[6+i]= outgoingUBX->payload[i];
    counter++;
  }

  calcChecksum(outgoingUBX); //Sets checksum A and B bytes of the packet
  tx_buffer[counter]= outgoingUBX->checksumA;
  counter++;
  tx_buffer[counter]= outgoingUBX->checksumB;
  counter++;


  #ifdef DEBUG_PRINT
  NRF_LOG_INFO("sending i2c UBX message......" );
  NRF_LOG_INFO("CLASS: %X   ID: %x", outgoingUBX->cls,  outgoingUBX->id );
  NRF_LOG_INFO("Length: %d ",outgoingUBX->len);
  NRF_LOG_INFO("PAYLOAD: %X %X %X %X %X ", outgoingUBX->payload[0],outgoingUBX->payload[1],outgoingUBX->payload[2],outgoingUBX->payload[3],outgoingUBX->payload[4]);
  NRF_LOG_INFO("PAYLOAD: %X %X %X %X %X ", outgoingUBX->payload[5],outgoingUBX->payload[6],outgoingUBX->payload[7],outgoingUBX->payload[8],outgoingUBX->payload[9]);
  NRF_LOG_INFO("PAYLOAD: %X %X %X %X %X ", outgoingUBX->payload[10],outgoingUBX->payload[11],outgoingUBX->payload[12],outgoingUBX->payload[13],outgoingUBX->payload[14]);
  NRF_LOG_INFO("PAYLOAD: %X %X %X %X %X ", outgoingUBX->payload[15],outgoingUBX->payload[16],outgoingUBX->payload[17],outgoingUBX->payload[18],outgoingUBX->payload[19]);
  NRF_LOG_INFO("CHEKSUM: %X %X ",outgoingUBX->checksumA, outgoingUBX->checksumB);
  #endif

  //Configuration of i2c communication
  NRF_TWIM1->ADDRESS = GPS_ADDRESS;//DIR of device
  NRF_TWIM1->TXD.MAXCNT =counter;    
  NRF_TWIM1->SHORTS = TWIM_SHORTS_LASTTX_STOP_Msk;

 //start tranmission
  NRF_TWIM1->TASKS_STARTTX = 1;
  while (NRF_TWIM1->EVENTS_STOPPED == 0){};
    NRF_TWIM1->EVENTS_STOPPED = 0; //need to be clear by firmware

}



//Polls I2C for data, passing any new bytes to process()
//Random read access, returns true if new bytes are available
bool getPollData(uint16_t bytesToRead, ubxPacket *incomingUBX, uint8_t requestedClass, uint8_t requestedID)
{

  //Configuration of i2c communication
  NRF_TWIM1->TXD.MAXCNT = 1;
  NRF_TWIM1->RXD.MAXCNT = bytesToRead;
  NRF_TWIM1->ADDRESS = GPS_ADDRESS;//DIR of device
  //shorcut --> LASTTX event with STARTRX task and  LASTRX event with STOP task
  NRF_TWIM1->SHORTS = TWIM_SHORTS_LASTTX_STARTRX_Msk | TWIM_SHORTS_LASTRX_STOP_Msk;

  //write transmit register
  tx_buffer[0] = 0xFF;    //read all available bytes

  //start tranmission
  NRF_TWIM1->TASKS_STARTTX=1;
  while (NRF_TWIM1->EVENTS_STOPPED == 0){};
    NRF_TWIM1->EVENTS_STOPPED = 0;

  process(rx_buffer, incomingUBX, requestedClass, requestedID); //Process this valid character

  return (true);

} //end checkUbloxI2C()





//Process received UBX binary message one byte at a time
void process(uint8_t incoming[] , ubxPacket *incomingUBX, uint8_t requestedClass, uint8_t requestedID)
{
  //save the received data save in incoming buffer into the 'incomingUBX' ubxPacket structure and check the incoming message in the meanwhile

  //validity is not defined
  incomingUBX->valid = SFE_UBLOX_PACKET_VALIDITY_NOT_DEFINED;

  //check if the headers are correct
  if (incoming[0] == UBX_SYNCH_1 && incoming[1]== UBX_SYNCH_2)
  { 
    incomingUBX->cls = incoming[2];
    incomingUBX->id =  incoming[3]; 
    incomingUBX->len = incoming[4];          //Length LSB
    incomingUBX->len |= incoming[5]<< 8;    //Length MSB


    //now, it is possible to identifly the type of response
    if (incomingUBX->cls == UBX_CLASS_ACK)
    {
       //The packet is an ACK. //Packet lenght of an ACK message should be 2
       if (incomingUBX->len==2)
       {
          //if lenght is 2 --> save all the data into the incomingUBX structure
          incomingUBX->payload[0] = incoming[6];
          incomingUBX->payload[1] = incoming[7];
          incomingUBX->checksumA = incoming[8];
          incomingUBX->checksumB = incoming[9];
          if (incomingUBX->id == UBX_ACK_ACK)
          {
            incomingUBX->type = SFE_UBLOX_PACKET_PACKETACKACK;
          }
          else if (incomingUBX->id == UBX_ACK_NAK)
          {
            incomingUBX->type = SFE_UBLOX_PACKET_PACKETACKNAK;
          }
          else
          {
           incomingUBX->type = SFE_UBLOX_PACKET_NOT_VALID;
          }

          //check the checksum
          verifiedCheckSum(incomingUBX);
       }
       else
       {
        //lenght should be 2 for ACK message
        NRF_LOG_INFO("Message not valid. ACK message with lenght different to 2");
        incomingUBX->valid= SFE_UBLOX_PACKET_VALIDITY_NOT_VALID;            //message not valid
       }
    }
    else if ((incomingUBX->cls == requestedClass) && (incomingUBX->id == requestedID))
    {
      if (incomingUBX->len!=0)
      {
         //The packet is not an ACK and we have a class and ID match --> (usually packetCfg)
        int payLoadLenCounter=0;
        while (payLoadLenCounter<=incomingUBX->len)
        {
           incomingUBX->payload[payLoadLenCounter] = incoming[6+payLoadLenCounter];
           payLoadLenCounter++;
        }
        //then save the chekcsum
         incomingUBX->checksumA = incoming[6+incomingUBX->len];
         incomingUBX->checksumB = incoming[7+incomingUBX->len];

         if (incomingUBX->cls == UBX_CLASS_CFG)       
         {
          incomingUBX->type = SFE_UBLOX_PACKET_PACKETCFG;
          //NRF_LOG_INFO("Configuration packet");
         }
         else if(incomingUBX->cls == UBX_CLASS_NAV)
         {
           //NRF_LOG_INFO("Navigation packet");
           incomingUBX->type = SFE_UBLOX_PACKET_PACKETNAV;
         }  
          //check the checksum
          verifiedCheckSum(incomingUBX);
      }
      else
      {
        //imposible that lenght is 0
        NRF_LOG_INFO("Message not valid. Lenght is 0");
        incomingUBX->valid= SFE_UBLOX_PACKET_VALIDITY_NOT_VALID;            //message not valid
      }
    }
    else
    {
      NRF_LOG_INFO("Message not valid. The message is not an ACK and the CLASS and/or ID does not match");
      //The packet is not an ACK and we do not have a class and ID match
      incomingUBX->valid= SFE_UBLOX_PACKET_VALIDITY_NOT_VALID;            //message not valid
    }
  }
  else
  {
     NRF_LOG_INFO("Message not valid. Headers does not match");
  }

            
  #ifdef DEBUG_PRINT
  NRF_LOG_INFO("Processing the received data... function Process-- incomingUBX packet" );
  NRF_LOG_INFO("CLASS: %X   ID: %x",incomingUBX->cls, incomingUBX->id  );
  NRF_LOG_INFO("Length: %d ",incomingUBX->len);
  NRF_LOG_INFO("PAYLOAD: %X %X %X %X %X ", incomingUBX->payload[0],incomingUBX->payload[1],incomingUBX->payload[2],incomingUBX->payload[3],incomingUBX->payload[4]);
  NRF_LOG_INFO("PAYLOAD: %X %X %X %X %X ", incomingUBX->payload[5],incomingUBX->payload[6],incomingUBX->payload[7],incomingUBX->payload[8],incomingUBX->payload[9]);
  NRF_LOG_INFO("PAYLOAD: %X %X %X %X %X ", incomingUBX->payload[10],incomingUBX->payload[11],incomingUBX->payload[12],incomingUBX->payload[13],incomingUBX->payload[14]);
  NRF_LOG_INFO("PAYLOAD: %X %X %X %X %X ", incomingUBX->payload[15],incomingUBX->payload[16],incomingUBX->payload[17],incomingUBX->payload[18],incomingUBX->payload[19]);
  NRF_LOG_INFO("CHEKSUM: %X %X ",incomingUBX->checksumA, incomingUBX->checksumB); 
  #endif



   
    // if the checksum is valid and the data is a packet navigation
    if (incomingUBX->valid ==SFE_UBLOX_PACKET_VALIDITY_VALID && incomingUBX->type == SFE_UBLOX_PACKET_PACKETNAV)
    {
        NRF_LOG_INFO("Message valid. Processing navigation packet");
        //process the navigation packet
        processUBXpacket(incomingUBX);     
     }
     else if (incomingUBX->valid ==SFE_UBLOX_PACKET_VALIDITY_VALID && incomingUBX->type ==SFE_UBLOX_PACKET_PACKETCFG)
     {    
        NRF_LOG_INFO("Message valid. Config packet");   
        //FALTARÍA DE PROCESAR EL SIGUIENTE MENSAJE DE ACK PERO ME DAPEREZA
        
     }
     else if (incomingUBX->valid ==SFE_UBLOX_PACKET_VALIDITY_NOT_VALID){
        NRF_LOG_INFO("Message invalid. Checksum does not match");
     }
     else if (incomingUBX->valid == SFE_UBLOX_PACKET_VALIDITY_NOT_DEFINED)
     {
        NRF_LOG_INFO("Message validity not defined");
     }


}



//Once a packet has been received and validated, identify this packet's class/id and update internal flags
//Note: if the user requests a PVT or a HPPOSLLH message using a custom packet, the data extraction will
//      not work as expected beacuse extractLong etc are hardwired to packetCfg payloadCfg. Ideally
//      extractLong etc should be updated so they receive a pointer to the packet buffer.
void processUBXpacket(ubxPacket* msg)
{

    if (msg->id == UBX_NAV_PVT && msg->len == 92)
    {
    /*
      const int startingSpot=0;
      uint32_t timeOfWeek = extractLong(0);
      uint32_t gpsMillisecond = extractLong(0) % 1000; //Get last three digits of iTOW
      uint16_t gpsYear = extractInt(4);
      uint8_t gpsMonth = extractByte(6);
      uint8_t gpsDay = extractByte(7);
      uint8_t gpsHour = extractByte(8);
      uint8_t gpsMinute = extractByte(9);
      uint8_t gpsSecond = extractByte(10);
      uint32_t gpsNanosecond = extractLong(16); //Includes milliseconds

      uint8_t fixType = extractByte(20 - startingSpot);
      uint8_t carrierSolution = extractByte(21 - startingSpot) >> 6; //Get 6th&7th bits of this byte
      uint8_t SIV = extractByte(23 - startingSpot);
      uint32_t longitude = extractLong(24 - startingSpot);
      uint32_t latitude = extractLong(28 - startingSpot);
      uint32_t altitude = extractLong(32 - startingSpot);
      uint32_t altitudeMSL = extractLong(36 - startingSpot);
      uint32_t groundSpeed = extractLong(60 - startingSpot);
      uint32_t headingOfMotion = extractLong(64 - startingSpot);
      uint16_t pDOP = extractInt(76 - startingSpot);*/
  


     sensorData.longitude = (float)((int32_t)extractLong(24)/1e7);    //Longitude (degress
     sensorData.latitude = (float)((int32_t)extractLong(28)/1e7);   //Latitute (degress
     sensorData.altitude = (float) ((int32_t)extractLong(36)/1e3);   //Height above medium see level (meters)  
     sensorData.speed = (float) ((int32_t)extractLong(60)/1e3);   //Ground speed (m/s)

/*
      sensorData.longitude = (int32_t)extractLong(24);    //Longitude  in degress x 1^(-7)
     sensorData.latitude = (int32_t)extractLong(28);   //Latitute  in degress x 1^(-7)
     sensorData.altitude =(int32_t)extractLong(36);   //Height above medium see level (mm)  
     sensorData.speed = (int32_t)extractLong(60);   //Ground speed (mm/s)*/


    }
    else if (msg->id == UBX_NAV_HPPOSLLH && msg->len == 36)
    {
      uint32_t timeOfWeek = extractLong(4);
      uint32_t highResLongitude = extractLong(8);
      uint32_t  highResLatitude = extractLong(12);
      uint32_t elipsoid = extractLong(16);
      uint32_t meanSeaLevel = extractLong(20);
      int8_t highResLongitudeHp = extractSignedChar(24);
      int8_t highResLatitudeHp = extractSignedChar(25);
      int8_t elipsoidHp = extractSignedChar(26);
      int8_t meanSeaLevelHp = extractSignedChar(27);
      uint32_t horizontalAccuracy = extractLong(28);
      uint32_t verticalAccuracy = extractLong(32);
    }
}




//Given a spot in the payload array, extract four bytes and build a long
uint32_t extractLong(uint8_t spotToStart)
{
  uint32_t val = 0;
  val |= (uint32_t)payloadCfg[spotToStart + 0] << 8 * 0;
  val |= (uint32_t)payloadCfg[spotToStart + 1] << 8 * 1;
  val |= (uint32_t)payloadCfg[spotToStart + 2] << 8 * 2;
  val |= (uint32_t)payloadCfg[spotToStart + 3] << 8 * 3;
  return (val);
}

//Given a spot in the payload array, extract two bytes and build an int
uint16_t extractInt(uint8_t spotToStart)
{
  uint16_t val = 0;
  val |= (uint16_t)payloadCfg[spotToStart + 0] << 8 * 0;
  val |= (uint16_t)payloadCfg[spotToStart + 1] << 8 * 1;
  return (val);
}

//Given a spot, extract a byte from the payload
uint8_t extractByte(uint8_t spotToStart)
{
  return (payloadCfg[spotToStart]);
}

//Given a spot, extract a signed 8-bit value from the payload
int8_t extractSignedChar(uint8_t spotToStart)
{
  return ((int8_t)payloadCfg[spotToStart]);
}


//0xFD and 0xFE are the address of the registers to know the available bytes (MSB and LSB, respectevely).
//Method: Read current access 
//Set register address 0xFD and then, it auto-increments the address pointer so the value of the register 0xFE is also read
uint16_t readBytesAvailable ()
{
  //write transmit register
  tx_buffer[0] = 0xFD;

  //Configuration i2c
  NRF_TWIM1->TXD.MAXCNT = 1;
  NRF_TWIM1->RXD.MAXCNT = 2;
  NRF_TWIM1->ADDRESS = GPS_ADDRESS;//DIR of device
  //shorcut --> LASTTX event with STARTRX task and  LASTRX event with STOP task
  NRF_TWIM1->SHORTS = TWIM_SHORTS_LASTTX_STARTRX_Msk | TWIM_SHORTS_LASTRX_STOP_Msk;

  //start tranmission
  NRF_TWIM1->TASKS_STARTTX=1;
  while (NRF_TWIM1->EVENTS_STOPPED == 0){};
  NRF_TWIM1->EVENTS_STOPPED = 0;

  //rx_buffer[0] -> MSB available bytes
  //rx_buffer[1] -> LSB available bytes

  uint16_t bytesAvailable = (uint16_t)(rx_buffer[0]<<8 | rx_buffer[1]);

 if (rx_buffer[0]==0xFF) //message no valid
 {
      NRF_LOG_INFO("0xFF - messsage not valid ");
    return -1;
 }

  //Check for undocumented bit error. We found this doing logic scans.
 //This error is rare but if we incorrectly interpret the first bit of the two 'data available' bytes as 1
  //then we have far too many bytes to check. May be related to I2C setup time violations: https://github.com/sparkfun/SparkFun_Ublox_Arduino_Library/issues/40
 else if (bytesAvailable & ((uint16_t)1 << 15))
  {
      NRF_LOG_INFO("I2C FAILURE TIMING ");
      //Clear the MSbit
      bytesAvailable &= ~((uint16_t)1 << 15);
      return bytesAvailable;
  }
  else
    return bytesAvailable;
}




//Get the latest Position/Velocity/Time solution and fill all global variables
void getNAV_PVT()
{
  //The GPS is not automatically reporting navigation position so we have to poll explicitly
  //Creating the message
  packetCfg.cls = UBX_CLASS_NAV;
  packetCfg.id = UBX_NAV_PVT;
  packetCfg.len = 0;

  //Sending the message
   sendi2cUBXMessage(&packetCfg);
  
}


void verifiedCheckSum (ubxPacket *incoming)
{

  //copy the checksum of the incoming packet
  uint8_t copyChecksumA=incoming->checksumA;
  uint8_t copyChecksumB = incoming->checksumB;
  //calcualte the checksum of the incoming packet
  calcChecksum(incoming);


  if ( (copyChecksumA==incoming->checksumA)  && (copyChecksumB == incoming->checksumB))
  {
    incoming->valid=SFE_UBLOX_PACKET_VALIDITY_VALID;
  }
  else
  {
    NRF_LOG_INFO("not valid checksum");
    incoming->valid= SFE_UBLOX_PACKET_VALIDITY_NOT_VALID;            //message not vali
  }  

  //Restore the real value of checksum to the pointer
  incoming->checksumA = copyChecksumA;
  incoming->checksumB = copyChecksumB;

}

void calcChecksum(ubxPacket *msg)
{
  msg->checksumA = 0;
  msg->checksumB = 0;

  msg->checksumA += msg->cls;
  msg->checksumB += msg->checksumA;

  msg->checksumA += msg->id;
  msg->checksumB += msg->checksumA;

  msg->checksumA += (msg->len & 0xFF);
  msg->checksumB += msg->checksumA;

  msg->checksumA += (msg->len >> 8);
  msg->checksumB += msg->checksumA;

  for (uint16_t i = 0; i < msg->len; i++)
  {
    msg->checksumA += msg->payload[i];
    msg->checksumB += msg->checksumA;
  }
}
