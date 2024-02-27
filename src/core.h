#include <stdint.h>
#include <stdio.h>

//defines for CB encodings
#define CB_A 0x7
#define CB_B 0x0
#define CB_C 0x1
#define CB_D 0x2
#define CB_E 0x3
#define CB_H 0x4
#define CB_L 0x5
#define CB_HL 0x6


extern uint8_t A;
extern uint8_t B;
extern uint8_t C;
extern uint8_t D;
extern uint8_t E;
extern uint8_t H;
extern uint8_t L;

extern uint16_t sp;
extern uint16_t pc;

extern uint8_t flags; // Z N H C 0 0 0 0

extern void fetchOpcode();
extern void executeCycle();

extern uint8_t memory[0x10000]; // 64KB of memory