#ifndef __COMMANDER_H__
#define __COMMANDER_H__

#include <Arduino.h>

#include "Msgs.h"
#include "MemInterface.h"

//#define DEBUG_COMMANDER

void Commander_RequestMemory(ADDRESS_BUS_TYPE address, uint8_t len);
void Commander_WriteMemory(ADDRESS_BUS_TYPE address, void* data, uint8_t len);

bool Commander_HostConnected();

void Commander_Background(uint8_t c);

#endif
