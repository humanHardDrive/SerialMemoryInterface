#ifndef __MEM_INTERFACE_H__
#define __MEM_INTERFACE_H__

#include <Arduino.h>

#define DEBUG_MEMINTERFACE

#define CACHE_SIZE  128 //Keep this as a power of 2 to avoid using modulo

#define ADDRESS_BITS  16
#define DATA_BITS     8

#define ADDRESS_EXPANDERS   ((ADDRESS_BITS + EXPANDER_WIDTH - 1) / EXPANDER_WIDTH)
#define ADDRESS_BYTES       (ADDRESS_BITS / 8)
#define DATA_EXPANDERS      ((DATA_BITS + EXPANDER_WIDTH - 1) / EXPANDER_WIDTH)
#define DATA_BYTES          (DATA_BITS / 8)

#if ADDRESS_BYTES > 4
#error "Address bus too large"
#elif ADDRESS_BYTES > 2
#define ADDRESS_BUS_TYPE uint32_t
#else
#define ADDRESS_BUS_TYPE uint16_t
#endif

#if DATA_BYTES > 4
#error "Data bus too large"
#elif DATA_BYTES > 2
#define DATA_BUS_TYPE uint32_t
#else
#define DATA_BUS_TYPE uint16_t
#endif

#define ADDRESS_EXP_OFFSET  0x00
#define DATA_EXP_OFFSET     0x04

void MemInterface_Init();

void MemInterface_SetRead(bool high);
void MemInterface_SetEnable(bool high);

void MemInterface_UpdateMemory(ADDRESS_BUS_TYPE address, void* data, uint8_t len);

void MemInterface_ClockEnable(uint16_t period);
void MemInterface_ClockDisable();

void MemInterface_Background();

#endif
