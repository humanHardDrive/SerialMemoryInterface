#include "IOExpander.h"
#include "Pins.h"

#include <SPI.h>

void IOExpander_Reset()
{
  //reset line low
  delay(1);
  //reset line high
}


void IOExpander_WriteReg(uint8_t address, uint8_t reg, uint8_t val)
{
  byte buffer[3];
  buffer[0] = (IOREG_OPCODE | IOEXP_ADDR(address) | IOREG_WRITE);
  buffer[1] = reg;
  buffer[2] = val;

  //chip select enable
  SPI.transfer(buffer, 3);
  //chip select disable
}

void IOExpander_WriteReg(uint8_t address, uint8_t basereg, uint8_t* val, uint8_t len)
{
  byte buffer[2];
  buffer[0] = (IOREG_OPCODE | IOEXP_ADDR(address) | IOREG_WRITE);
  buffer[1] = basereg;

  //chip select enable
  SPI.transfer(buffer, 2);
  SPI.transfer(val, len);
  //chip select disable
}


uint8_t IOExpander_ReadReg(uint8_t address, uint8_t reg)
{
  byte buffer[3];
  buffer[0] = (IOREG_OPCODE | IOEXP_ADDR(address) | IOREG_READ);
  buffer[1] = reg;
  buffer[2] = 0x00;

  //chip select enable
  SPI.transfer(buffer, 3);
  //chip select disable
  
  return buffer[2];
}

void IOExpanderReadReg(uint8_t address, uint8_t basereg, uint8_t* val, uint8_t len)
{
  byte buffer[2];
  buffer[0] = (IOREG_OPCODE | IOEXP_ADDR(address) | IOREG_READ);
  buffer[1] = basereg;

  //chip select enable
  SPI.transfer(buffer, 2);
  SPI.transfer(val, len);
  //chip select disable
}

