#ifndef __COMMANDER_H__
#define __COMMANDER_H__

#include <Arduino.h>

#include "MemInterface.h"

#define PACKET_STX  0x55
#define PACKET_ETX  0xAA

enum
{
  NO_COMMAND = 0,
  VERSION_REQUEST, //From host to hardware
  CACHE_SIZE_REQUEST, //From host to hardware
  MEMORY_REQUEST, //From hardware to host
  MEMORY_WRITE, //From hardware to host
  ALL_COMMANDS
};

void Commander_RequestMemory(ADDRESS_BUS_TYPE address, uint8_t len);
void Commander_WriteMemory(ADDRESS_BUS_TYPE address, void* data, uint8_t len);

void Commander_Background(uint8_t c);

#endif
