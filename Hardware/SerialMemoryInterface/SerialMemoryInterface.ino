#include "MemInterface.h"
#include "Commander.h"

void setup() 
{
  Serial.begin(115200);
}

void loop() 
{
  MemInterface_Background();
}
