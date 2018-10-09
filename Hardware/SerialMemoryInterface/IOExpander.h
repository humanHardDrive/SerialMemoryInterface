#ifndef __IO_EXPANDER_H__
#define __IO_EXPANDER_H__

#include <Arduino.h>

//Bank = 0 Configuration
#define IOREG_IODIRA    0x00
#define IOREG_IODIRB    0x01
#define IOREG_IPOLA     0x02
#define IOREG_IPOLB     0x03
#define IOREG_GPINTENA  0x04
#define IOREG_GPINTENB  0x05
#define IOREG_DEFVALA   0x06
#define IOREG_DEFVALB   0x07
#define IOREG_INTCONA   0x08
#define IOREG_INTCONB   0x09
#define IOREG_IOCON0    0x0A
#define IOREG_IOCON1    0x0B
#define IOREG_GPPUA     0x0C
#define IOREG_GPPUB     0x0D
#define IOREG_INTFA     0x0E
#define IOREG_INTFB     0x0F
#define IOREG_INTCAPA   0x10
#define IOREG_INTCAPB   0x11
#define IOREG_GPIOA     0x12
#define IOREG_GPIOB     0x13
#define IOREG_OLATA     0x14
#define IOREG_OLATB     0x15

//Command operations
#define IOREG_READ      0x01
#define IOREG_WRITE     0x00
#define IOREG_OPCODE    0x40

#define IOEXP_ADDR(addr)  ((addr << 1) & 0x0E)

void IOExpander_Reset();

void IOExpander_WriteReg(uint8_t address, uint8_t reg, uint8_t val);
void IOExpander_WriteReg(uint8_t address, uint8_t basereg, uint8_t* val, uint8_t len);

uint8_t IOExpander_ReadReg(uint8_t address, uint8_t reg);
void IOExpanderReadReg(uint8_t address, uint8_t basereg, uint8_t* val, uint8_t len);

#endif
