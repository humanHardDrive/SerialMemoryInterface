#include "IOExpander.h"
#include "Pins.h"

#include <SPI.h>

void IOExpander_Reset()
{
  digitalWrite(IOEXPRST_PIN, LOW);
  delay(1);
  digitalWrite(IOEXPRST_PIN, HIGH);
}


void IOExpander_WriteReg(uint8_t address, uint8_t reg, uint8_t val)
{
  byte buffer[3];
  buffer[0] = (IOREG_OPCODE | IOEXP_ADDR(address) | IOREG_WRITE);
  buffer[1] = reg;
  buffer[2] = val;

  digitalWrite(IOEXPCS_PIN, LOW);
  SPI.transfer(buffer, 3);
  digitalWrite(IOEXPCS_PIN, HIGH);
}

void IOExpander_WriteReg(uint8_t address, uint8_t basereg, uint8_t* val, uint8_t len)
{
  byte buffer[2];
  buffer[0] = (IOREG_OPCODE | IOEXP_ADDR(address) | IOREG_WRITE);
  buffer[1] = basereg;

  digitalWrite(IOEXPCS_PIN, LOW);
  SPI.transfer(buffer, 2);
  SPI.transfer(val, len);
  digitalWrite(IOEXPCS_PIN, HIGH);
}


uint8_t IOExpander_ReadReg(uint8_t address, uint8_t reg)
{
  byte buffer[3];
  buffer[0] = (IOREG_OPCODE | IOEXP_ADDR(address) | IOREG_READ);
  buffer[1] = reg;
  buffer[2] = 0x00;

  digitalWrite(IOEXPCS_PIN, LOW);
  SPI.transfer(buffer, 3);
  digitalWrite(IOEXPCS_PIN, HIGH);
  
  return buffer[2];
}

void IOExpander_ReadReg(uint8_t address, uint8_t basereg, uint8_t* val, uint8_t len)
{
  byte buffer[2];
  buffer[0] = (IOREG_OPCODE | IOEXP_ADDR(address) | IOREG_READ);
  buffer[1] = basereg;

  digitalWrite(IOEXPCS_PIN, LOW);
  SPI.transfer(buffer, 2);
  SPI.transfer(val, len);
  digitalWrite(IOEXPCS_PIN, HIGH);
}

