#include "Commander.h"

uint16_t l_CurrentMsgLen = 0;
uint8_t l_CurrentMsgType = NO_COMMAND, l_CurrentParseState = WAITING_FOR_STX;

bool l_HostConnected = false;

void ProcessCommand(uint8_t type);
void ProcessCommand(uint8_t type, uint16_t bytecount);

void Commander_RequestMemory(ADDRESS_BUS_TYPE address, uint8_t len)
{
  Serial.write(PACKET_STX);
  Serial.write(MEMORY_REQUEST);
  Serial.write(sizeof(ADDRESS_BUS_TYPE));
  Serial.write((char*)&address, sizeof(ADDRESS_BUS_TYPE));
  Serial.write(PACKET_ETX);
}

void Commander_WriteMemory(ADDRESS_BUS_TYPE address, void* data, uint8_t len)
{
}

void CacheResponse()
{
  Serial.write(PACKET_STX);
  Serial.write(CACHE_SIZE_REQUEST);
  Serial.write(3);
  Serial.write(CACHE_SIZE);
  Serial.write(ADDRESS_BYTES);
  Serial.write(DATA_BYTES);
  Serial.write(PACKET_ETX);
}

void VersionResponse()
{
  Serial.write(PACKET_STX);
  Serial.write(VERSION_REQUEST);
  Serial.write(2);
  Serial.write(0);
  Serial.write(0);
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
      l_CurrentParseState = PROCESS_COMMAND;
      break;

    case PROCESS_COMMAND:
      ProcessCommand(l_CurrentMsgType);
      if (l_CurrentMsgLen)
        l_CurrentParseState = WAITING_FOR_DATA;
      else
        l_CurrentParseState = WAITING_FOR_ETX;
      break;

    case WAITING_FOR_DATA:
      ProcessCommand(l_CurrentMsgType, bytecount);
      bytecount++;
      break;

    case WAITING_FOR_ETX:
      if (c == PACKET_ETX)
        l_CurrentParseState = WAITING_FOR_STX;
      break;
  }
}


bool Commander_HostConnected()
{
  return l_HostConnected;
}


void ProcessCommand(uint8_t type)
{
  switch (type)
  {
    case VERSION_REQUEST:
      l_HostConnected = true;
      VersionResponse();
      break;

    case CACHE_SIZE_REQUEST:
      CacheResponse();
      break;

    case CLOCK_DISABLE:
      MemInterface_ClockDisable();
      break;
  }
}

void ProcessCommand(uint8_t type, uint16_t bytecount)
{
  switch (type)
  {
    case MEMORY_REQUEST:
      break;

    case CLOCK_ENABLE:
      break;
  }
}

