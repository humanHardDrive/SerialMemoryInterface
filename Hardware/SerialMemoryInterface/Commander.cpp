#include "Commander.h"

uint16_t l_CurrentMsgLen = 0;
uint8_t l_CurrentMsgType = NO_COMMAND, l_CurrentParseState = WAITING_FOR_STX;

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

  for (uint8_t i = 0; i < len; i++)
    Serial.write( ((uint8_t*)data)[i] );

  Serial.write(PACKET_ETX);
}


void Commander_Background(uint8_t c)
{
  static uint8_t bytecount = 0;
  
  switch (l_CurrentParseState)
  {
    case WAITING_FOR_STX:
      if (c == PACKET_STX)
      {
        l_CurrentParseState = WAITING_FOR_TYPE;
        bytecount = 0;
      }
      break;

    case WAITING_FOR_TYPE:
      l_CurrentMsgType = c;
      l_CurrentParseState = WAITING_FOR_LEN_LSB;
      break;

    case WAITING_FOR_LEN_LSB:
      *((uint8_t*)&l_CurrentMsgLen) = c;
      l_CurrentParseState = WAITING_FOR_LEN_MSB;
      break;

    case WAITING_FOR_LEN_MSB:
      *((uint8_t*)(&l_CurrentMsgLen) + 1) = c;
      l_CurrentParseState = WAITING_FOR_DATA;
      break;

    case WAITING_FOR_DATA:
      switch(l_CurrentMsgType)
      {
        
      }
      bytecount++;
      break;

    case WAITING_FOR_ETX:
    l_CurrentParseState = WAITING_FOR_STX;
      break;
  }
}

