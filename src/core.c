#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>


    uint8_t A;
    uint8_t B;
    uint8_t C;
    uint8_t D;
    uint8_t E;
    uint8_t H;
    uint8_t L;

    /*
    Registers are sometimes combined into 16-bit pairs (e.g. BC, DE, HL, AF)
    The pairs can be accessed as follows:

    uint16_t BC = B<<8 | C;

    
    */

    uint16_t sp;
    uint16_t pc;

    uint8_t flags; // Z N H C 0 0 0 0

    //defines for flags
    #define ZF 0x80;
    #define NF 0x40;
    #define HF 0x20;
    #define CF 0x10;

    /*  
    Z - Zero Flag
    N - Subtract Flag
    H - Half Carry Flag
    C - Carry Flag

    We can use the following to set and reset the flags:

    flags |= ZF; // set zero flag
    flags &= ~ZF; // reset zero flag
    ...
    ect

    */

    uint16_t opcode;
    uint8_t machineCycles;

    uint8_t memory[65536]; // 64KB of memory

void fetchOpcode(){
    machineCycles = 1; //default cycles per instruction is one
    opcode = memory[pc];
    pc++;
}


void executeCycle(){
    fetchOpcode();
    switch(opcode){
        
        case 0x00: // NOP
            break;

        case 0x01:{ // Load BC with nn16, 3bytes
        machineCycles = 3;
        C = memory[pc];
        pc++;
        B = memory[pc];
        pc++;
        
        break;
        }

        case 0x02: { // Load BC with A, 1byte
            machineCycles = 2;
            memory[B<<8 | C] = A;
            break;
        }

        case 0x03: { // Increment BC, 1byte
            machineCycles = 2;
            uint16_t BC = B<<8 | C;
            BC++;
            B = (BC & 0xFF00) >> 8;
            C = BC & 0x00FF;
            break;
        }

        case 0x04: { // Increment B, 1byte
            machineCycles = 1;
            B++;
            if(B == 0){
                flags |= ZF; // set zero flag
            }
            if(B & 0x10){
                flags |= HF; // set half carry flag
            }
            flags &= NF; // reset subtract flag
            break;
        }

        case 0x05: { // Decrement B, 1byte
            machineCycles = 1;
            B--;
            if(B == 0){
                flags |= ZF; // set zero flag
            }
            if(B & 0x10){
                flags |= HF; // set half carry flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0x06: { // Load B with nn8, 2bytes
            machineCycles = 2;
            B = memory[pc];
            pc++;
            break;
        }

        case 0x07: { // Rotate left A, 1byte
            machineCycles = 1;
            uint8_t carry = A & 0x80;
            A = A << 1;
            A = A | carry;
            if(carry){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            flags &= ~NF; // reset subtract flag
            flags &= ~HF; // reset half carry flag
            break;
        }

        case 0x08: { // Load SP with nn16, 3bytes
            machineCycles = 3;
            sp = memory[pc];
            pc++;
            sp = sp | (memory[pc] << 8);
            pc++;
            break;
        }

        case 0x09: { // Add HL to BC, 1byte
            machineCycles = 2;
            uint16_t HL = H<<8 | L;
            uint16_t BC = B<<8 | C;
            uint16_t result = HL + BC;
            H = (result & 0xFF00) >> 8;
            L = result & 0x00FF;
            if(result & 0x10000){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            flags &= ~NF; // reset subtract flag
            break;
        }
    }   
}
