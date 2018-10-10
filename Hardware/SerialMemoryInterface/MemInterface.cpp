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
    return l_DataCache[address - l_CacheStartAddress];
  }
  else
  {
    if(l_CacheDirty)
      Commander_WriteMemory(address, l_DataCache, DATA_BYTES);

    Commander_RequestMemory(address, DATA_BYTES);
  }
}

void MemInterface_Init()
{
  //Setup IO
  pinMode(MEMENABLE_PIN, INPUT);
  pinMode(READWRITE_PIN, INPUT);

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


void MemInterface_Background()
{
  bool CurrentMemEnableState = digitalRead(MEMENABLE_PIN);

  if (CurrentMemEnableState && l_EnableIsHigh ||
      !CurrentMemEnableState && !l_EnableIsHigh)
  {
    bool CurrentReadWriteState = digitalRead(READWRITE_PIN);

    if (CurrentReadWriteState != l_LastReadWriteState)
    {
      if (CurrentReadWriteState && l_ReadIsHigh ||
          !CurrentReadWriteState && !l_ReadIsHigh)
      {
        //Set data IO expanders as outputs
        ChangeDataBusDirection(false);
      }
      else
      {
        //Set data IO expanders as inputs
        ChangeDataBusDirection(true);
      }
    }

    l_LastReadWriteState = CurrentReadWriteState;
  }
  else
  {
    if (CurrentMemEnableState != l_LastMemEnableState)
    {
      //Set data IO expanders as inputs (high impedance)
      ChangeDataBusDirection(true);
    }
  }

  l_LastMemEnableState = CurrentMemEnableState;
}

