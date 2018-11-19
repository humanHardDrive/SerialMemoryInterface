#include "MemInterface.h"
#include "Commander.h"
#include "IOExpander.h"
#include "Pins.h"

#include <SPI.h>

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

uint32_t l_LastClkTime = 0;
bool l_ReadMode = false;

void ChangeDataBusDirection(bool input)
{
#ifdef DEBUG_MEMINTERFACE
  Serial.println(__FUNCTION__);
  Serial.println();
#endif
  
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
#ifdef DEBUG_MEMINTERFACE
  Serial.println(__FUNCTION__);
  
  Serial.print("Cache Size: ");
  Serial.println(CACHE_SIZE);
  
  Serial.print("Addresss Bus: ");
  Serial.print(ADDRESS_BITS);
  Serial.print(" ");
  Serial.print(ADDRESS_BYTES);
  Serial.print(" ");
  Serial.println(ADDRESS_EXPANDERS);

  Serial.print("Data Bus: ");
  Serial.print(DATA_BITS);
  Serial.print(" ");
  Serial.print(DATA_BYTES);
  Serial.print(" ");
  Serial.println(DATA_EXPANDERS);

  Serial.println();
#endif

  pinMode(IOEXPCS_PIN, OUTPUT);  
  digitalWrite(IOEXPCS_PIN, HIGH);

  pinMode(IOEXPRST_PIN, OUTPUT);
  digitalWrite(IOEXPRST_PIN, HIGH);

  pinMode(CLOCKOUT_PIN, OUTPUT);
  digitalWrite(CLOCKOUT_PIN, HIGH);

  pinMode(READ_PIN, INPUT);
  pinMode(WRITE_PIN, INPUT);

  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV8);

  //Setup IO expanders
  IOExpander_Reset();
  delay(1);
  IOExpander_WriteReg(0, IOREG_IOCON0, 0x08); //Enable hardware addresses
  for(byte i = 0; i < ADDRESS_EXPANDERS; i++)
  {
    //Set as inputs
    IOExpander_WriteReg(ADDRESS_EXP_OFFSET + i, IOREG_IODIRA, 0xFF);
    IOExpander_WriteReg(ADDRESS_EXP_OFFSET + i, IOREG_IODIRB, 0xFF);

    IOExpander_ReadReg(ADDRESS_EXP_OFFSET + i, IOREG_IODIRA);
    IOExpander_ReadReg(ADDRESS_EXP_OFFSET + i, IOREG_IODIRB);
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
  if(millis() - l_LastClkTime > 1000)
  {
    digitalWrite(CLOCKOUT_PIN, !digitalRead(CLOCKOUT_PIN));
    l_LastClkTime = millis();
  }

  if(!digitalRead(READ_PIN) && !l_ReadMode)
  {
#ifdef DEBUG_MEMINTERFACE
    Serial.println(__FUNCTION__);
    Serial.println("Changing data bus to output");
    Serial.println();
#endif
    
    ChangeDataBusDirection(false);
    l_ReadMode = true;
  }
  else if(digitalRead(READ_PIN) && l_ReadMode)
  {
#ifdef DEBUG_MEMINTERFACE
    Serial.println(__FUNCTION__);
    Serial.println("Changing data bus to input");
    Serial.println();
#endif
    
    ChangeDataBusDirection(true);
    l_ReadMode = false;
  }
}

