#include "MemInterface.h"
#include "Commander.h"
#include "IOExpander.h"
#include "Pins.h"

#include <SPI.h>
#include <string.h>

bool l_ReadIsHigh = false;
bool l_EnableIsHigh = false;

bool l_LastReadWriteState = false;
bool l_LastMemEnableState = false;

ADDRESS_BUS_TYPE l_CurrentAddress = (ADDRESS_BUS_TYPE)(-1);
DATA_BUS_TYPE l_LastPlacedData = 0;

DATA_BUS_TYPE l_DataCache[CACHE_SIZE];
ADDRESS_BUS_TYPE l_CacheStartAddress = (ADDRESS_BUS_TYPE)(-1);
bool l_CacheDirty = false;
bool l_WaitingForMemory = false;

uint32_t l_LastClkTime = 0;
bool l_ReadMode = false;
bool l_WriteMode = false;

void ChangeDataBusDirection(bool input)
{
#ifdef DEBUG_MEMINTERFACE
  Serial.print(__FUNCTION__);
  Serial.print('('); Serial.print(input); Serial.println(')');
#endif

  for (byte i = 0; i < DATA_EXPANDERS; i++)
  {
    IOExpander_WriteReg(DATA_EXP_OFFSET + i, IOREG_IODIRA, input ? 0xFF : 0x00);
    IOExpander_WriteReg(DATA_EXP_OFFSET + i, IOREG_IODIRB, input ? 0xFF : 0x00);
  }
}

DATA_BUS_TYPE GetData(ADDRESS_BUS_TYPE address)
{
  if (address >= l_CacheStartAddress && address < (l_CacheStartAddress + CACHE_SIZE)) //The data is in cache
  {
#ifdef DEBUG_MEMINTERFACE
    Serial.print("Address ");
    Serial.print(address, HEX);
    Serial.println(" in cache");
#endif
    l_WaitingForMemory = false;
    return l_DataCache[address - l_CacheStartAddress];
  }
  else if (!l_WaitingForMemory) //Have to request it from the host
  {
#ifdef DEBUG_MEMINTERFACE
    Serial.print("Address ");
    Serial.print(address, HEX);
    Serial.println(" not in cache. Requesting");
#endif

    if (l_CacheDirty) //Write back any changes made
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

  for (uint8_t i = 0; i < DATA_EXPANDERS; i++)
  {
    RegWord = ((uint16_t*)&data)[i];
    IOExpander_WriteReg(DATA_EXP_OFFSET + i, IOREG_GPIOA, (uint8_t*)&RegWord, 2);
  }
}

ADDRESS_BUS_TYPE GetCurrentAddress()
{
  uint32_t address = 0;
  uint16_t RegWord;
  uint16_t* pAddress = (uint16_t*)&address;

  for (uint8_t i = 0; i < ADDRESS_EXPANDERS; i++)
  {
    IOExpander_ReadReg(ADDRESS_EXP_OFFSET + i, IOREG_GPIOA, (uint8_t*)&RegWord, 2);
    memcpy(&address + 2 * i, &RegWord, 2);
  }

  return (ADDRESS_BUS_TYPE)(address);
}

void MemInterface_Init()
{
#ifdef DEBUG_MEMINTERFACE
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
#endif

  pinMode(IOEXPCS_PIN, OUTPUT);
  digitalWrite(IOEXPCS_PIN, HIGH);

  pinMode(IOEXPRST_PIN, OUTPUT);
  digitalWrite(IOEXPRST_PIN, HIGH);

  pinMode(CLOCKOUT_PIN, OUTPUT);
  digitalWrite(CLOCKOUT_PIN, HIGH);

  pinMode(WAIT_PIN, OUTPUT);
  digitalWrite(WAIT_PIN, HIGH);

  pinMode(READ_PIN, INPUT);
  pinMode(WRITE_PIN, INPUT);

  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV4);

  //Setup IO expanders
  IOExpander_Reset();
  delay(1);
  IOExpander_WriteReg(0, IOREG_IOCON0, 0x08); //Enable hardware addresses
  for (byte i = 0; i < ADDRESS_EXPANDERS; i++)
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
  ADDRESS_BUS_TYPE tempaddress;

  //Update the clock
  //Remove later
  if (millis() - l_LastClkTime > 100)
  {
    digitalWrite(CLOCKOUT_PIN, !digitalRead(CLOCKOUT_PIN));
    l_LastClkTime = millis();
  }

  //Handle chaning the direction of the data bus according to the read pin
  if (!digitalRead(READ_PIN) && !l_ReadMode)
  {
#ifdef DEBUG_MEMINTERFACE
    Serial.println("Changing data bus to output");
#endif

    ChangeDataBusDirection(false);
    l_ReadMode = true;
  }
  else if (digitalRead(READ_PIN) && l_ReadMode) //Regardless of the write mode
  { //Switch to a high impedance state
#ifdef DEBUG_MEMINTERFACE
    Serial.println("Changing data bus to input");
#endif

    ChangeDataBusDirection(true);
    l_ReadMode = false;
  }

  if (!digitalRead(WRITE_PIN) && !l_WriteMode)
  {
#ifdef DEBUG_MEMINTERFACE
    Serial.println("Write mode active");
#endif
    l_WriteMode = true;
  }
  else if (digitalRead(WRITE_PIN) && l_WriteMode)
  {
#ifdef DEBUG_MEMINTERFACE
    Serial.println("Write mode inactive");
#endif
    l_WriteMode = false;
  }

  //Only care about the address if one of the access modes are set
  //But not both
  if ((!l_ReadMode && l_WriteMode) ||
      (l_ReadMode && !l_WriteMode))
  {
    //Update the current address bus
    tempaddress = GetCurrentAddress();
    if (tempaddress != l_CurrentAddress)
    {
      DATA_BUS_TYPE data;
      
#ifdef DEBUG_MEMINTERFACE
      Serial.print("New address: ");
      Serial.println(tempaddress, HEX);
#endif
      l_CurrentAddress = tempaddress;
      data = GetData(tempaddress);
      if(!l_WaitingForMemory)
      {
        digitalWrite(WAIT_PIN, HIGH);
      }
      else
      {
        digitalWrite(WAIT_PIN, LOW);
      }
    }
  }
}

