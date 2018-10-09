#ifndef __MEM_INTERFACE_H__
#define __MEM_INTERFACE_H__

#include <Arduino.h>

void MemInterface_Init();

void MemInterface_SetRead(bool high);
void MemInterface_SetEnable(bool high);

void MemInterface_Background();

#endif
