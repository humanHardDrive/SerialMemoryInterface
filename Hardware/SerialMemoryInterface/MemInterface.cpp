#include "MemInterface.h"
#include "Commander.h"
#include "IOExpander.h"
#include "Pins.h"

bool l_ReadIsHigh = false;
bool l_EnableIsHigh = false;

bool l_LastReadWriteState = false;
bool l_LastMemEnableState = false;

ADDRESS_BUS_TYPE l_LastReqAddress = (ADDRESS_BUS_TYPE)(-1);
DATA_BUS_TYPE l_LastPlacedData = 0;

DATA_BUS_TYPE l_DataCache[CACHE_SIZE];
ADDRESS_BUS_TYPE l_CacheStartAddress = (ADDRESS_BUS_TYPE)(-1);
bool l_CacheDirty = false;
bool l_WaitingForMemory = false;

void ChangeDataBusDirection(bool input)
{
  for(byte i = 0; i < DATA_EXPANDERS; i++)
  {
    IOExpander_WriteReg(DATA_EXP_OFFSET + i, IOREG_IODIRA, input ? 0xFF : 0x00);
    IOExpander_WriteReg(DATA_EXP_OFFSET + i, IOREG_IODIRB, input ? 0xFF : 0x00);
  }
}

DATA_BUS_TYPE GetData(ADDRESS_BUS_TYPE address)
{
  if(address >= l_CacheStartAddress && address < (l_CacheStartAddress + CACHE_SIZE)) //The data is in cache
  {
    l_WaitingForMemory = false;
    return l_DataCache[address - l_CacheStartAddress];
  }
  else if(!l_WaitingForMemory) //Have to request it from the host
  {
    if(l_CacheDirty) //Write back any changes made
      Commander_WriteMemory(address, l_DataCache, DATA_BYTES);

    //Request new memory
    Commander_RequestMemory(address, DATA_BYTES);

    l_WaitingForMemory = true;

    return 0; //This value ultimately doesn't matter
  }
}

void PutData(DATA_BUS_TYPE data)
{
  uint16_t RegWord;
  
  for(uint8_t i = 0; i < DATA_EXPANDERS; i++)
  {
    RegWord = ((uint16_t*)&data)[i];
    IOExpander_WriteReg(DATA_EXP_OFFSET + i, IOREG_GPIOA, (uint8_t*)&RegWord, 2);
  }
}

ADDRESS_BUS_TYPE GetCurrentAddress()
{
  ADDRESS_BUS_TYPE address = 0;
  uint16_t RegWord;

  for(uint8_t i = 0; i < ADDRESS_EXPANDERS; i--)
  {    
    IOExpander_ReadReg(ADDRESS_EXP_OFFSET + i, IOREG_GPIOA, (uint8_t*)&RegWord, 2);
    ((uint16_t*)&address)[i] = RegWord;
  }

  return address;
}

void MemInterface_Init()
{
  //Setup IO
  pinMode(MEMENABLE_PIN, INPUT);
  pinMode(READ_PIN, INPUT);
  pinMode(WRITE_PIN, INPUT);

  //Setup IO expanders
  IOExpander_Reset();
  IOExpander_WriteReg(0, IOREG_IOCON0, 0x08); //Enable hardware addresses
  for(byte i = 0; i < ADDRESS_EXPANDERS; i++)
  {
    //Set as inputs
    IOExpander_WriteReg(ADDRESS_EXP_OFFSET + i, IOREG_IODIRA, 0xFF);
    IOExpander_WriteReg(ADDRESS_EXP_OFFSET + i, IOREG_IODIRB, 0xFF);
  }

  ChangeDataBusDirection(true);
}


void MemInteface_SetRead(bool high)
{
  l_ReadIsHigh = high;
}

void MemInterface_SetEnable(bool high)
{
  l_EnableIsHigh = high;
}


void MemInterface_UpdateMemory(ADDRESS_BUS_TYPE address, void* data, uint8_t len)
{
  memcpy(l_DataCache, data, len);
  l_CacheStartAddress = address;
}


void MemInterface_Background()
{
}

