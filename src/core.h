#include <stdint.h>
#include <stdio.h>

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