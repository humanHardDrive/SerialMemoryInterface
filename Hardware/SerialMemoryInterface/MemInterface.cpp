#include "MemInterface.h"
#include "IOExpander.h"
#include "Pins.h"

bool l_ReadIsHigh = false;
bool l_EnableIsHigh = false;

bool l_LastReadWriteState = false;
bool l_LastMemEnableState = false;

void MemInterface_Init()
{
  //Setup IO
  pinMode(MEMENABLE_PIN, INPUT);
  pinMode(READWRITE_PIN, INPUT);

  //Setup IO expanders
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
      }
      else
      {
        //Set data IO expanders as inputs
      }
    }

    l_LastReadWriteState = CurrentReadWriteState;
  }
  else
  {
    if (CurrentMemEnableState != l_LastMemEnableState)
    {
      //Set data IO expanders as inputs (high impedance)
    }
  }

  l_LastMemEnableState = CurrentMemEnableState;
}

