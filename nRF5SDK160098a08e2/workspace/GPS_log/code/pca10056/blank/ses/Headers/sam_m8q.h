#ifndef _SAM_M8Q_H
#define _SAM_M8Q_H

#include "CommonHeaders.h"


//ubxPacket validity
typedef enum
{
  SFE_UBLOX_PACKET_VALIDITY_NOT_VALID,
  SFE_UBLOX_PACKET_VALIDITY_VALID,
  SFE_UBLOX_PACKET_VALIDITY_NOT_DEFINED,
  SFE_UBLOX_PACKET_NOTACKNOWLEDGED // This indicates that we received a NACK
} sfe_ublox_packet_validity_e;


// Identify which packet buffer is in use:
typedef enum
{
  SFE_UBLOX_PACKET_PACKETCFG,
  SFE_UBLOX_PACKET_PACKETACKACK,
  SFE_UBLOX_PACKET_PACKETACKNAK,
  SFE_UBLOX_PACKET_PACKETBUF,
  SFE_UBLOX_PACKET_PACKETNAV,
  SFE_UBLOX_PACKET_NOT_DEFINED,   
  SFE_UBLOX_PACKET_NOT_VALID     
} sfe_ublox_packet_buffer_type;


//UBX MESSAGE STRUCTURE
typedef struct
{
  uint8_t cls;
  uint8_t id;
  uint16_t len;		   //Length of the payload. Does not include cls, id, or checksum bytes
  uint8_t *payload;
  uint8_t checksumA; //Given to us from module. Checked against the rolling calculated A/B checksums.
  uint8_t checksumB;
  sfe_ublox_packet_validity_e valid; //Goes from NOT_DEFINED to VALID or NOT_VALID 
  sfe_ublox_packet_buffer_type type;   //Save the type of the message
} ubxPacket;







//FUNCTION DEFINITIONS
    //Configuration
void gpsInit ();
void getPortSettings ( uint8_t);
void setPortOutput(uint8_t , uint8_t );
void powerSaveMode();
void getPowerMode();
void saveConfiguration();
void setPSMCTmode();
void getSaveMode();
void setGNSSConfig(uint8_t);
void getGNSSConfig();

    //Resets
void factoryDefault();
void hardReset();
void factoryReset();

    //Get data (latitude, longitude..)
void samM8Q_getPosition();
void getNAV_PVT();

  //Log files function
void createLogFile();
void ConfigLogFile();

  //Other necessary functions
void sendi2cUBXMessage(ubxPacket *);
bool getPollData(uint16_t , ubxPacket *, uint8_t , uint8_t );
void process(uint8_t [] , ubxPacket *, uint8_t , uint8_t );
void processUBXMessage(uint8_t , ubxPacket *, uint8_t , uint8_t );
void processUBXpacket(ubxPacket* );
void verifiedCheckSum (ubxPacket *);
void calcChecksum(ubxPacket *);
sfe_ublox_packet_buffer_type checkPollRequest();
uint32_t extractLong(uint8_t);
uint16_t extractInt(uint8_t);
uint8_t extractByte(uint8_t);
int8_t extractSignedChar(uint8_t);
uint16_t readBytesAvailable ();







#endif