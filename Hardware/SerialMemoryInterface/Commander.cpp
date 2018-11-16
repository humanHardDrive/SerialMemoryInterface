#include "Commander.h"

void Commander_RequestMemory(ADDRESS_BUS_TYPE address, uint8_t len)
{
  Serial.write(PACKET_STX);
  Serial.write(MEMORY_REQUEST);
  Serial.write(address);
  Serial.write(len);
  Serial.write(PACKET_ETX);
}

void Commander_WriteMemory(ADDRESS_BUS_TYPE address, void* data, uint8_t len)
{
  Serial.write(PACKET_STX);
  Serial.write(MEMORY_WRITE);
  Serial.write(address);
  Serial.write(len);

  for(uint8_t i = 0; i < len; i++)
    Serial.write( ((uint8_t*)data)[i] );

  Serial.write(PACKET_ETX);
}


void Commander_Background(uint8_t c)
{
}

