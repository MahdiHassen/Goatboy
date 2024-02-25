#include <stdio.h>
#include "core.h"

#define ROM 0x00
#define MBC1 0x01
#define MBC1_RAM 0x02
#define MBC1_RAM_BATTERY 0x03
#define MBC2 0x05
#define MBC2_BATTERY 0x06
#define ROM_RAM 0x08
#define ROM_RAM_BATTERY 0x09
#define MMM01 0x0B
#define MMM01_RAM 0x0C
#define MMM01_RAM_BATTERY 0x0D
#define MBC3_TIMER_BATTERY 0x0F
#define MBC3_TIMER_RAM_BATTERY 0x10
#define MBC3 0x11
#define MBC3_RAM 0x12
#define MBC3_RAM_BATTERY 0x13
#define MBC4 0x15
#define MBC4_RAM 0x16
#define MBC4_RAM_BATTERY 0x17
#define MBC5 0x19
#define MBC5_RAM 0x1A
#define MBC5_RAM_BATTERY 0x1B
#define MBC5_RUMBLE 0x1C
#define MBC5_RUMBLE_RAM 0x1D
#define MBC5_RUMBLE_RAM_BATTERY 0x1E
#define POCKET_CAMERA 0x1F
#define BANDAI_TAMA5 0xFD
#define HuC3 0xFE
#define HuC1_RAM_BATTERY 0xFF

extern void loadCartridge(char* filename);
extern void readCartridgeHeader();

extern uint8_t cartridgeType;
extern uint8_t romSize; 
extern uint8_t ramSize;

extern uint8_t cartridgeData[];
