#include "core.h"
#include <SDL2/SDL.h>
#include <stdio.h>

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144

#define SCREEN_SCALE 3

#define PPU_Control 0xFF40 // [LCD_EN, WIN_MAP, WIN_EN, BG_MAP, OBJ_SIZE, OBJ_EN, BG_EN]
#define PPU_Status 0xFF41 // []
#define Scroll_Y 0xFF42 
#define Scroll_X 0xFF43
#define SCANLINE_Control 0xFF44
#define SCANLINE_Compare 0xFF45

#define OAM 0xFE00 // 0xFE00 - 0xFE9F Object Attribute Memory

#define VRAM_START 0x8000

#define TILE_MAP_0 0x9800
#define TILE_MAP_1 0x9C00

void initGraphics();
