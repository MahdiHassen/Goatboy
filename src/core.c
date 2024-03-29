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
    uint16_t pc = 0x100;

    uint8_t flags; // Z N H C 0 0 0 0

    //defines for flags
    #define ZF 0x80
    #define NF 0x40
    #define HF 0x20
    #define CF 0x10

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

    uint8_t memory[0x10000]; // 64KB of memory



void incrementPC(){
    pc++;
}

void fetchOpcode(){
    machineCycles = 1; //default cycles per instruction is one
    opcode = memory[pc];
    printf("pc: 0x%X | ", pc);
    printf("opcode: 0x%X\n", opcode);
    incrementPC();
    
}


//prefix CB instructions are at the end of the switch statement
void executeCycle(){
    fetchOpcode();
    switch(opcode){
        
        case 0x00:{ // NOP
        machineCycles = 1;  
            break;
        }

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

        case 0x0A: { // Load A with BC, 1byte
            machineCycles = 2;
            A = memory[B<<8 | C];
            break;
        }

        case 0x0B: { // Decrement BC, 1byte
            machineCycles = 2;
            uint16_t BC = B<<8 | C;
            BC--;
            B = (BC & 0xFF00) >> 8;
            C = BC & 0x00FF;
            break;
        }

        case 0x0C: { // Increment C, 1byte
            machineCycles = 1;
            C++;
            if(C == 0){
                flags |= ZF; // set zero flag
            }
            if(C & 0x10){
                flags |= HF; // set half carry flag
            }
            flags &= NF; // reset subtract flag
            break;
        }

        case 0x0D: { // Decrement C, 1byte
            machineCycles = 1;
            C--;
            if(C == 0){
                flags |= ZF; // set zero flag
            }
            if(C & 0x10){
                flags |= HF; // set half carry flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0x0E: { // Load C with nn8, 2bytes
            machineCycles = 2;
            C = memory[pc];
            pc++;
            break;
        }

        case 0x0F: { // Rotate right A, 1byte
            machineCycles = 1;
            uint8_t carry = A & 0x01;
            A = A >> 1;
            A = A | (carry << 7);
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

        case 0x10: { // Stop, 1byte, TODO: Fix, currently treated as NOP
            machineCycles = 1;
            break;
        }

        case 0x11: { // Load DE with nn16, 3bytes
            machineCycles = 3;
            E = memory[pc];
            pc++;
            D = memory[pc];
            pc++;
            break;
        }

        case 0x12: { // Load DE with A, 1byte
            machineCycles = 2;
            memory[D<<8 | E] = A;
            break;
        }

        case 0x13: { // Increment DE, 1byte
            machineCycles = 2;
            uint16_t DE = D<<8 | E;
            DE++;
            D = (DE & 0xFF00) >> 8;
            E = DE & 0x00FF;
            break;
        }

        case 0x14: { // Increment D, 1byte
            machineCycles = 1;
            D++;
            if(D == 0){
                flags |= ZF; // set zero flag
            }
            if(D & 0x10){
                flags |= HF; // set half carry flag
            }
            flags &= NF; // reset subtract flag
            break;
        }

        case 0x15: { // Decrement D, 1byte
            machineCycles = 1;
            D--;
            if(D == 0){
                flags |= ZF; // set zero flag
            }
            if(D & 0x10){
                flags |= HF; // set half carry flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0x16: { // Load D with nn8, 2bytes
            machineCycles = 2;
            D = memory[pc];
            pc++;
            break;
        }

        case 0x17: { // Rotate left A through carry, 1byte
            machineCycles = 1;
            uint8_t carry = A & 0x80;
            A = A << 1;
            A = A | (flags & CF);
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

        case 0x18: { // Jump to PC + nn8, 2bytes
            machineCycles = 3;
            int8_t offset = memory[pc];
            pc++; 
            pc = pc + offset;
            break;
        }

        case 0x19: { // Add HL to DE, 1byte
            machineCycles = 2;
            uint16_t HL = H<<8 | L;
            uint16_t DE = D<<8 | E;
            uint16_t result = HL + DE;
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

        case 0x1A: { // Load A with DE, 1byte
            machineCycles = 2;
            A = memory[D<<8 | E];
            break;
        }

        case 0x1B: { // Decrement DE, 1byte
            machineCycles = 2;
            uint16_t DE = D<<8 | E;
            DE--;
            D = (DE & 0xFF00) >> 8;
            E = DE & 0x00FF;
            break;
        }

        case 0x1C: { // Increment E, 1byte
            machineCycles = 1;
            E++;
            if(E == 0){
                flags |= ZF; // set zero flag
            }
            if(E & 0x10){
                flags |= HF; // set half carry flag
            }
            flags &= NF; // reset subtract flag
            break;
        }

        case 0x1D: { // Decrement E, 1byte
            machineCycles = 1;
            E--;
            if(E == 0){
                flags |= ZF; // set zero flag
            }
            if(E & 0x10){
                flags |= HF; // set half carry flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0x1E: { // Load E with nn8, 2bytes
            machineCycles = 2;
            E = memory[pc];
            pc++;
            break;
        }

        case 0x1F: { // Rotate right A through carry, 1byte
            machineCycles = 1;
            uint8_t carry = A & 0x01;
            A = A >> 1;
            A = A | (flags & CF);
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

        case 0x20: { // Jump to PC + nn8 if ZF is reset, 2bytes
            machineCycles = 3;
            int8_t offset = memory[pc];
            pc++;
            if(!(flags & ZF)){
                pc = pc + offset;
            }
            break;
        }

        case 0x21: { // Load HL with nn16, 3bytes
            machineCycles = 3;
            L = memory[pc];
            pc++;
            H = memory[pc];
            pc++;
            break;
        }

        case 0x22: { // Load HL with A, 1byte
            machineCycles = 2;
            memory[H<<8 | L] = A;
            uint16_t HL = H<<8 | L;
            HL++;
            H = (HL & 0xFF00) >> 8;
            L = HL & 0x00FF;
            break;
        }

        case 0x23: { // Increment HL, 1byte
            machineCycles = 2;
            uint16_t HL = H<<8 | L;
            HL++;
            H = (HL & 0xFF00) >> 8;
            L = HL & 0x00FF;
            break;
        }

        case 0x24: { // Increment H, 1byte
            machineCycles = 1;
            H++;
            if(H == 0){
                flags |= ZF; // set zero flag
            }
            if(H & 0x10){
                flags |= HF; // set half carry flag
            }
            flags &= NF; // reset subtract flag
            break;
        }

        case 0x25: { // Decrement H, 1byte
            machineCycles = 1;
            H--;
            if(H == 0){
                flags |= ZF; // set zero flag
            }
            if(H & 0x10){
                flags |= HF; // set half carry flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0x26: { // Load H with nn8, 2bytes
            machineCycles = 2;
            H = memory[pc];
            pc++;
            break;
        }

        case 0x27: { // Decimal adjust A, 1byte
            machineCycles = 1;
            if((A & 0x0F) > 9 || (flags & HF)){
                A = A + 0x06;
                flags |= HF; // set half carry flag
            }
            if((A & 0xF0) > 0x90 || (flags & CF)){
                A = A + 0x60;
                flags |= CF; // set carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            break;
        }

        case 0x28: { // Jump to PC + nn8 if ZF is set, 2bytes
            machineCycles = 3;
            int8_t offset = memory[pc];
            pc++;
            if(flags & ZF){
                pc = pc + offset;
            }
            break;
        }

        case 0x29: { // Add HL to HL, 1byte
            machineCycles = 2;
            uint16_t HL = H<<8 | L;
            uint16_t result = HL + HL;
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

        case 0x2A: { // Load A with HL, 1byte
            machineCycles = 2;
            A = memory[H<<8 | L];
            uint16_t HL = H<<8 | L;
            HL++;
            H = (HL & 0xFF00) >> 8;
            L = HL & 0x00FF;
            break;
        }

        case 0x2B: { // Decrement HL, 1byte
            machineCycles = 2;
            uint16_t HL = H<<8 | L;
            HL--;
            H = (HL & 0xFF00) >> 8;
            L = HL & 0x00FF;
            break;
        }

        case 0x2C: { // Increment L, 1byte
            machineCycles = 1;
            L++;
            if(L == 0){
                flags |= ZF; // set zero flag
            }
            if(L & 0x10){
                flags |= HF; // set half carry flag
            }
            flags &= NF; // reset subtract flag
            break;
        }

        case 0x2D: { // Decrement L, 1byte
            machineCycles = 1;
            L--;
            if(L == 0){
                flags |= ZF; // set zero flag
            }
            if(L & 0x10){
                flags |= HF; // set half carry flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0x2E: { // Load L with nn8, 2bytes
            machineCycles = 2;
            L = memory[pc];
            pc++;
            break;
        }

        case 0x2F: { // Complement A, 1byte
            machineCycles = 1;
            A = ~A;
            flags |= NF; // set subtract flag
            flags |= HF; // set half carry flag
            break;
        }

        case 0x30: { // Jump to PC + nn8 if CF is reset, 2bytes
            machineCycles = 3;
            int8_t offset = memory[pc];
            pc++;
            if(!(flags & CF)){
                pc = pc + offset;
            }
            break;
        }

        case 0x31: { // Load SP with nn16, 3bytes
            machineCycles = 3;
            sp = memory[pc];
            pc++;
            sp = sp | (memory[pc] << 8);
            pc++;
            break;
        }

        case 0x32: { // Load HL with A, 1byte
            machineCycles = 2;
            memory[H<<8 | L] = A;
            uint16_t HL = H<<8 | L;
            HL--;
            H = (HL & 0xFF00) >> 8;
            L = HL & 0x00FF;
            break;
        }

        case 0x33: { // Increment SP, 1byte
            machineCycles = 2;
            sp++;
            break;
        }

        case 0x34: { // Increment (HL), 1byte
            machineCycles = 3;
            uint16_t HL = H<<8 | L;
            memory[HL]++;
            if(memory[HL] == 0){
                flags |= ZF; // set zero flag
            }
            if(memory[HL] & 0x10){
                flags |= HF; // set half carry flag
            }
            flags &= NF; // reset subtract flag
            break;
        }

        case 0x35: { // Decrement (HL), 1byte
            machineCycles = 3;
            uint16_t HL = H<<8 | L;
            memory[HL]--;
            if(memory[HL] == 0){
                flags |= ZF; // set zero flag
            }
            if(memory[HL] & 0x10){
                flags |= HF; // set half carry flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0x36: { // Load (HL) with nn8, 2bytes
            machineCycles = 3;
            uint16_t HL = H<<8 | L;
            memory[HL] = memory[pc];
            pc++;
            break;
        }

        case 0x37: { // Set carry flag, 1byte
            machineCycles = 1;
            flags |= CF; // set carry flag
            break;
        }

        case 0x38: { // Jump to PC + nn8 if CF is set, 2bytes
            machineCycles = 3;
            int8_t offset = memory[pc];
            pc++;
            if(flags & CF){
                pc = pc + offset;
            }
            break;
        }

        case 0x39: { // Add HL to SP, 1byte
            machineCycles = 2;
            uint16_t HL = H<<8 | L;
            uint16_t result = HL + sp;
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

        case 0x3A: { // Load A with HL, 1byte
            machineCycles = 2;
            A = memory[H<<8 | L];
            uint16_t HL = H<<8 | L;
            HL--;
            H = (HL & 0xFF00) >> 8;
            L = HL & 0x00FF;
            break;
        }

        case 0x3B: { // Decrement SP, 1byte
            machineCycles = 2;
            sp--;
            break;
        }

        case 0x3C: { // Increment A, 1byte
            machineCycles = 1;
            A++;
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            if(A & 0x10){
                flags |= HF; // set half carry flag
            }
            flags &= NF; // reset subtract flag
            break;
        }

        case 0x3D: { // Decrement A, 1byte
            machineCycles = 1;
            A--;
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            if(A & 0x10){
                flags |= HF; // set half carry flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0x3E: { // Load A with nn8, 2bytes
            machineCycles = 2;
            A = memory[pc];
            pc++;
            break;
        }

        case 0x3F: { // Complement carry flag, 1byte
            machineCycles = 1;
            flags ^= CF; // flip carry flag
            flags &= ~NF; // reset subtract flag
            flags &= ~HF; // reset half carry flag
            break;
        }

        case 0x40: { // Load B with B, 1byte
            machineCycles = 1;
            B = B;
            break;
        }

        case 0x41: { // Load B with C, 1byte
            machineCycles = 1;
            B = C;
            break;
        }

        case 0x42: { // Load B with D, 1byte
            machineCycles = 1;
            B = D;
            break;
        }

        case 0x43: { // Load B with E, 1byte
            machineCycles = 1;
            B = E;
            break;
        }

        case 0x44: { // Load B with H, 1byte
            machineCycles = 1;
            B = H;
            break;
        }

        case 0x45: { // Load B with L, 1byte
            machineCycles = 1;
            B = L;
            break;
        }

        case 0x46: { // Load B with (HL), 1byte
            machineCycles = 2;
            uint16_t HL = H<<8 | L;
            B = memory[H<<8 | L];
            break;
        }

        case 0x47: { // Load B with A, 1byte
            machineCycles = 1;
            B = A;
            break;
        }

        case 0x48: { // Load C with B, 1byte
            machineCycles = 1;
            C = B;
            break;
        }

        case 0x49: { // Load C with C, 1byte
            machineCycles = 1;
            C = C;
            break;
        }

        case 0x4A: { // Load C with D, 1byte
            machineCycles = 1;
            C = D;
            break;
        }

        case 0x4B: { // Load C with E, 1byte
            machineCycles = 1;
            C = E;
            break;
        }

        case 0x4C: { // Load C with H, 1byte
            machineCycles = 1;
            C = H;
            break;
        }

        case 0x4D: { // Load C with L, 1byte
            machineCycles = 1;
            C = L;
            break;
        }

        case 0x4E: { // Load C with (HL), 1byte
            machineCycles = 2;
            C = memory[H<<8 | L];
            break;
        }

        case 0x4F: { // Load C with A, 1byte
            machineCycles = 1;
            C = A;
            break;
        }

        case 0x50: { // Load D with B, 1byte
            machineCycles = 1;
            D = B;
            break;
        }

        case 0x51: { // Load D with C, 1byte
            machineCycles = 1;
            D = C;
            break;
        }

        case 0x52: { // Load D with D, 1byte
            machineCycles = 1;
            D = D;
            break;
        }

        case 0x53: { // Load D with E, 1byte
            machineCycles = 1;
            D = E;
            break;
        }

        case 0x54: { // Load D with H, 1byte
            machineCycles = 1;
            D = H;
            break;
        }

        case 0x55: { // Load D with L, 1byte
            machineCycles = 1;
            D = L;
            break;
        }

        case 0x56: { // Load D with (HL), 1byte
            machineCycles = 2;
            D = memory[H<<8 | L];
            break;
        }

        case 0x57: { // Load D with A, 1byte
            machineCycles = 1;
            D = A;
            break;
        }

        case 0x58: { // Load E with B, 1byte
            machineCycles = 1;
            E = B;
            break;
        }

        case 0x59: { // Load E with C, 1byte
            machineCycles = 1;
            E = C;
            break;
        }

        case 0x5A: { // Load E with D, 1byte
            machineCycles = 1;
            E = D;
            break;
        }

        case 0x5B: { // Load E with E, 1byte
            machineCycles = 1;
            E = E;
            break;
        }

        case 0x5C: { // Load E with H, 1byte
            machineCycles = 1;
            E = H;
            break;
        }

        case 0x5D: { // Load E with L, 1byte
            machineCycles = 1;
            E = L;
            break;
        }

        case 0x5E: { // Load E with (HL), 1byte
            machineCycles = 2;
            E = memory[H<<8 | L];
            break;
        }

        case 0x5F: { // Load E with A, 1byte
            machineCycles = 1;
            E = A;
            break;
        }

        case 0x60: { // Load H with B, 1byte
            machineCycles = 1;
            H = B;
            break;
        }

        case 0x61: { // Load H with C, 1byte
            machineCycles = 1;
            H = C;
            break;
        }

        case 0x62: { // Load H with D, 1byte
            machineCycles = 1;
            H = D;
            break;
        }

        case 0x63: { // Load H with E, 1byte
            machineCycles = 1;
            H = E;
            break;
        }

        case 0x64: { // Load H with H, 1byte
            machineCycles = 1;
            H = H;
            break;
        }

        case 0x65: { // Load H with L, 1byte
            machineCycles = 1;
            H = L;
            break;
        }

        case 0x66: { // Load H with (HL), 1byte
            machineCycles = 2;
            H = memory[H<<8 | L];
            break;
        }

        case 0x67: { // Load H with A, 1byte
            machineCycles = 1;
            H = A;
            break;
        }

        case 0x68: { // Load L with B, 1byte
            machineCycles = 1;
            L = B;
            break;
        }

        case 0x69: { // Load L with C, 1byte
            machineCycles = 1;
            L = C;
            break;
        }

        case 0x6A: { // Load L with D, 1byte
            machineCycles = 1;
            L = D;
            break;
        }

        case 0x6B: { // Load L with E, 1byte
            machineCycles = 1;
            L = E;
            break;
        }

        case 0x6C: { // Load L with H, 1byte
            machineCycles = 1;
            L = H;
            break;
        }

        case 0x6D: { // Load L with L, 1byte
            machineCycles = 1;
            L = L;
            break;
        }

        case 0x6E: { // Load L with (HL), 1byte
            machineCycles = 2;
            L = memory[H<<8 | L];
            break;
        }

        case 0x6F: { // Load L with A, 1byte
            machineCycles = 1;
            L = A;
            break;
        }

        case 0x70: { // Load (HL) with B, 1byte
            machineCycles = 2;
            memory[H<<8 | L] = B;
            break;
        }

        case 0x71: { // Load (HL) with C, 1byte
            machineCycles = 2;
            memory[H<<8 | L] = C;
            break;
        }

        case 0x72: { // Load (HL) with D, 1byte
            machineCycles = 2;
            memory[H<<8 | L] = D;
            break;
        }

        case 0x73: { // Load (HL) with E, 1byte
            machineCycles = 2;
            memory[H<<8 | L] = E;
            break;
        }

        case 0x74: { // Load (HL) with H, 1byte
            machineCycles = 2;
            memory[H<<8 | L] = H;
            break;
        }

        case 0x75: { // Load (HL) with L, 1byte
            machineCycles = 2;
            memory[H<<8 | L] = L;
            break;
        }

        case 0x76: { // Halt, 1byte TODO: Fix, currently treated as NOP
            machineCycles = 1;

            break;  
        }

        case 0x77: { // Load (HL) with A, 1byte
            machineCycles = 2;
            memory[H<<8 | L] = A;
            break;
        }

        case 0x78: { // Load A with B, 1byte
            machineCycles = 1;
            A = B;
            break;
        }

        case 0x79: { // Load A with C, 1byte
            machineCycles = 1;
            A = C;
            break;
        }

        case 0x7A: { // Load A with D, 1byte
            machineCycles = 1;
            A = D;
            break;
        }

        case 0x7B: { // Load A with E, 1byte
            machineCycles = 1;
            A = E;
            break;
        }

        case 0x7C: { // Load A with H, 1byte
            machineCycles = 1;
            A = H;
            break;
        }

        case 0x7D: { // Load A with L, 1byte
            machineCycles = 1;
            A = L;
            break;
        }

        case 0x7E: { // Load A with (HL), 1byte
            machineCycles = 2;
            A = memory[H<<8 | L];
            break;
        }

        case 0x7F: { // Load A with A, 1byte
            machineCycles = 1;
            A = A;
            break;
        }

        case 0x80: { // Add B to A, 1byte
            machineCycles = 1;
            uint16_t result = A + B;
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) + (B & 0x0F) > 0x0F){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            break;
        }

        case 0x81: { // Add C to A, 1byte
            machineCycles = 1;
            uint16_t result = A + C;
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) + (C & 0x0F) > 0x0F){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            break;
        }

        case 0x82: { // Add D to A, 1byte
            machineCycles = 1;
            uint16_t result = A + D;
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) + (D & 0x0F) > 0x0F){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            break;
        }

        case 0x83: { // Add E to A, 1byte
            machineCycles = 1;
            uint16_t result = A + E;
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) + (E & 0x0F) > 0x0F){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            break;
        }

        case 0x84: { // Add H to A, 1byte
            machineCycles = 1;
            uint16_t result = A + H;
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) + (H & 0x0F) > 0x0F){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            break;
        }

        case 0x85: { // Add L to A, 1byte
            machineCycles = 1;
            uint16_t result = A + L;
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) + (L & 0x0F) > 0x0F){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            break;
        }

        case 0x86: { // Add (HL) to A, 1byte
            machineCycles = 2;
            uint16_t result = A + memory[H<<8 | L];
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) + (memory[H<<8 | L] & 0x0F) > 0x0F){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            break;
        }

        case 0x87: { // Add A to A, 1byte
            machineCycles = 1;
            uint16_t result = A + A;
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) + (A & 0x0F) > 0x0F){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            break;
        }

        case 0x88: { // Add B + carry to A, 1byte
            machineCycles = 1;
            uint16_t result = A + B + (flags & CF);
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) + (B & 0x0F) + (flags & CF) > 0x0F){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            break;
        }

        case 0x89: { // Add C + carry to A, 1byte
            machineCycles = 1;
            uint16_t result = A + C + (flags & CF);
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) + (C & 0x0F) + (flags & CF) > 0x0F){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            break;
        }

        case 0x8A: { // Add D + carry to A, 1byte
            machineCycles = 1;
            uint16_t result = A + D + (flags & CF);
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) + (D & 0x0F) + (flags & CF) > 0x0F){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            break;
        }

        case 0x8B: { // Add E + carry to A, 1byte
            machineCycles = 1;
            uint16_t result = A + E + (flags & CF);
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) + (E & 0x0F) + (flags & CF) > 0x0F){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            break;
        }

        case 0x8C: { // Add H + carry to A, 1byte
            machineCycles = 1;
            uint16_t result = A + H + (flags & CF);
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) + (H & 0x0F) + (flags & CF) > 0x0F){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            break;
        }

        case 0x8D: { // Add L + carry to A, 1byte
            machineCycles = 1;
            uint16_t result = A + L + (flags & CF);
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) + (L & 0x0F) + (flags & CF) > 0x0F){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            break;
        }

        case 0x8E: { // Add (HL) + carry to A, 1byte
            machineCycles = 2;
            uint16_t result = A + memory[H<<8 | L] + (flags & CF);
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) + (memory[H<<8 | L] & 0x0F) + (flags & CF) > 0x0F){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            break;
        }

        case 0x8F: { // Add A + carry to A, 1byte
            machineCycles = 1;
            uint16_t result = A + A + (flags & CF);
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) + (A & 0x0F) + (flags & CF) > 0x0F){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            break;
        }

        case 0x90: { // Subtract B from A, 1byte
            machineCycles = 1;
            uint16_t result = A - B;
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) < (B & 0x0F)){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0x91: { // Subtract C from A, 1byte
            machineCycles = 1;
            uint16_t result = A - C;
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) < (C & 0x0F)){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0x92: { // Subtract D from A, 1byte
            machineCycles = 1;
            uint16_t result = A - D;
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) < (D & 0x0F)){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0x93: { // Subtract E from A, 1byte
            machineCycles = 1;
            uint16_t result = A - E;
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) < (E & 0x0F)){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0x94: { // Subtract H from A, 1byte
            machineCycles = 1;
            uint16_t result = A - H;
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) < (H & 0x0F)){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0x95: { // Subtract L from A, 1byte
            machineCycles = 1;
            uint16_t result = A - L;
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) < (L & 0x0F)){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0x96: { // Subtract (HL) from A, 1byte
            machineCycles = 2;
            uint16_t result = A - memory[H<<8 | L];
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) < (memory[H<<8 | L] & 0x0F)){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0x97: { // Subtract A from A, 1byte
            machineCycles = 1;
            uint16_t result = A - A;
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) < (A & 0x0F)){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0x98: { // Subtract B + carry from A, 1byte
            machineCycles = 1;
            uint16_t result = A - B - (flags & CF);
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) < (B & 0x0F) + (flags & CF)){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0x99: { // Subtract C + carry from A, 1byte
            machineCycles = 1;
            uint16_t result = A - C - (flags & CF);
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) < (C & 0x0F) + (flags & CF)){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0x9A: { // Subtract D + carry from A, 1byte
            machineCycles = 1;
            uint16_t result = A - D - (flags & CF);
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) < (D & 0x0F) + (flags & CF)){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0x9B: { // Subtract E + carry from A, 1byte
            machineCycles = 1;
            uint16_t result = A - E - (flags & CF);
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) < (E & 0x0F) + (flags & CF)){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0x9C: { // Subtract H + carry from A, 1byte
            machineCycles = 1;
            uint16_t result = A - H - (flags & CF);
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) < (H & 0x0F) + (flags & CF)){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0x9D: { // Subtract L + carry from A, 1byte
            machineCycles = 1;
            uint16_t result = A - L - (flags & CF);
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) < (L & 0x0F) + (flags & CF)){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0x9E: { // Subtract (HL) + carry from A, 1byte
            machineCycles = 2;
            uint16_t result = A - memory[H<<8 | L] - (flags & CF);
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) < (memory[H<<8 | L] & 0x0F) + (flags & CF)){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0x9F: { // Subtract A + carry from A, 1byte
            machineCycles = 1;
            uint16_t result = A - A - (flags & CF);
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) < (A & 0x0F) + (flags & CF)){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0xA0: { // AND B with A, 1byte
            machineCycles = 1;
            A &= B;
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            flags |= HF; // set half carry flag
            flags &= ~CF; // reset carry flag
            break;
        }

        case 0xA1: { // AND C with A, 1byte
            machineCycles = 1;
            A &= C;
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            flags |= HF; // set half carry flag
            flags &= ~CF; // reset carry flag
            break;
        }

        case 0xA2: { // AND D with A, 1byte
            machineCycles = 1;
            A &= D;
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            flags |= HF; // set half carry flag
            flags &= ~CF; // reset carry flag
            break;
        }

        case 0xA3: { // AND E with A, 1byte
            machineCycles = 1;
            A &= E;
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            flags |= HF; // set half carry flag
            flags &= ~CF; // reset carry flag
            break;
        }

        case 0xA4: { // AND H with A, 1byte
            machineCycles = 1;
            A &= H;
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            flags |= HF; // set half carry flag
            flags &= ~CF; // reset carry flag
            break;
        }

        case 0xA5: { // AND L with A, 1byte
            machineCycles = 1;
            A &= L;
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            flags |= HF; // set half carry flag
            flags &= ~CF; // reset carry flag
            break;
        }

        case 0xA6: { // AND (HL) with A, 1byte
            machineCycles = 2;
            A &= memory[H<<8 | L];
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            flags |= HF; // set half carry flag
            flags &= ~CF; // reset carry flag
            break;
        }

        case 0xA7: { // AND A with A, 1byte
            machineCycles = 1;
            A &= A;
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            flags |= HF; // set half carry flag
            flags &= ~CF; // reset carry flag
            break;
        }

        case 0xA8: { // XOR B with A, 1byte
            machineCycles = 1;
            A ^= B;
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            flags &= ~HF; // reset half carry flag
            flags &= ~CF; // reset carry flag
            break;
        }

        case 0xA9: { // XOR C with A, 1byte
            machineCycles = 1;
            A ^= C;
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            flags &= ~HF; // reset half carry flag
            flags &= ~CF; // reset carry flag
            break;
        }

        case 0xAA: { // XOR D with A, 1byte
            machineCycles = 1;
            A ^= D;
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            flags &= ~HF; // reset half carry flag
            flags &= ~CF; // reset carry flag
            break;
        }

        case 0xAB: { // XOR E with A, 1byte
            machineCycles = 1;
            A ^= E;
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            flags &= ~HF; // reset half carry flag
            flags &= ~CF; // reset carry flag
            break;
        }

        case 0xAC: { // XOR H with A, 1byte
            machineCycles = 1;
            A ^= H;
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            flags &= ~HF; // reset half carry flag
            flags &= ~CF; // reset carry flag
            break;
        }

        case 0xAD: { // XOR L with A, 1byte
            machineCycles = 1;
            A ^= L;
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            flags &= ~HF; // reset half carry flag
            flags &= ~CF; // reset carry flag
            break;
        }

        case 0xAE: { // XOR (HL) with A, 1byte
            machineCycles = 2;
            A ^= memory[H<<8 | L];
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            flags &= ~HF; // reset half carry flag
            flags &= ~CF; // reset carry flag
            break;
        }

        case 0xAF: { // XOR A with A, 1byte
            machineCycles = 1;
            A ^= A;
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            flags &= ~HF; // reset half carry flag
            flags &= ~CF; // reset carry flag
            break;
        }

        case 0xB0: { // OR B with A, 1byte
            machineCycles = 1;
            A |= B;
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            flags &= ~HF; // reset half carry flag
            flags &= ~CF; // reset carry flag
            break;
        }

        case 0xB1: { // OR C with A, 1byte
            machineCycles = 1;
            A |= C;
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            flags &= ~HF; // reset half carry flag
            flags &= ~CF; // reset carry flag
            break;
        }

        case 0xB2: { // OR D with A, 1byte
            machineCycles = 1;
            A |= D;
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            flags &= ~HF; // reset half carry flag
            flags &= ~CF; // reset carry flag
            break;
        }

        case 0xB3: { // OR E with A, 1byte
            machineCycles = 1;
            A |= E;
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            flags &= ~HF; // reset half carry flag
            flags &= ~CF; // reset carry flag
            break;
        }

        case 0xB4: { // OR H with A, 1byte
            machineCycles = 1;
            A |= H;
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            flags &= ~HF; // reset half carry flag
            flags &= ~CF; // reset carry flag
            break;
        }

        case 0xB5: { // OR L with A, 1byte
            machineCycles = 1;
            A |= L;
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            flags &= ~HF; // reset half carry flag
            flags &= ~CF; // reset carry flag
            break;
        }

        case 0xB6: { // OR (HL) with A, 1byte
            machineCycles = 2;
            A |= memory[H<<8 | L];
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            flags &= ~HF; // reset half carry flag
            flags &= ~CF; // reset carry flag
            break;
        }

        case 0xB7: { // OR A with A, 1byte
            machineCycles = 1;
            A |= A;
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            flags &= ~HF; // reset half carry flag
            flags &= ~CF; // reset carry flag
            break;
        }

        case 0xB8: { // Compare B with A, 1byte
            machineCycles = 1;
            uint16_t result = A - B;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) < (B & 0x0F)){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if((A & 0x00FF) == (B & 0x00FF)){
                flags |= ZF; // set zero flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0xB9: { // Compare C with A, 1byte
            machineCycles = 1;
            uint16_t result = A - C;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) < (C & 0x0F)){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if((A & 0x00FF) == (C & 0x00FF)){
                flags |= ZF; // set zero flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0xBA: { // Compare D with A, 1byte
            machineCycles = 1;
            uint16_t result = A - D;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) < (D & 0x0F)){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if((A & 0x00FF) == (D & 0x00FF)){
                flags |= ZF; // set zero flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0xBB: { // Compare E with A, 1byte
            machineCycles = 1;
            uint16_t result = A - E;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) < (E & 0x0F)){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if((A & 0x00FF) == (E & 0x00FF)){
                flags |= ZF; // set zero flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0xBC: { // Compare H with A, 1byte
            machineCycles = 1;
            uint16_t result = A - H;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) < (H & 0x0F)){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if((A & 0x00FF) == (H & 0x00FF)){
                flags |= ZF; // set zero flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0xBD: { // Compare L with A, 1byte
            machineCycles = 1;
            uint16_t result = A - L;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) < (L & 0x0F)){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if((A & 0x00FF) == (L & 0x00FF)){
                flags |= ZF; // set zero flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0xBE: { // Compare (HL) with A, 1byte
            machineCycles = 2;
            uint16_t result = A - memory[H<<8 | L];
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) < (memory[H<<8 | L] & 0x0F)){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if((A & 0x00FF) == (memory[H<<8 | L] & 0x00FF)){
                flags |= ZF; // set zero flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0xBF: { // Compare A with A, 1byte
            machineCycles = 1;
            uint16_t result = A - A;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) < (A & 0x0F)){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if((A & 0x00FF) == (A & 0x00FF)){
                flags |= ZF; // set zero flag
            }
            flags |= NF; // set subtract flag
            break;
        }

        case 0xC0: { // Return if not zero, 1byte
            machineCycles = 2;
            if(flags & ~ZF){
                pc = memory[sp] | memory[sp+1]<<8;
                sp += 2;
            machineCycles = 5; // 3 more cycles if condition is met
            }
            break;
        }

        case 0xC1: { // Pop BC off stack, 1byte
            machineCycles = 3;
            C = memory[sp];
            B = memory[sp+1];
            sp += 2;
            break;
        }

        case 0xC2: { // Jump to address if not zero, 3bytes
            machineCycles = 3;
            if(flags & ~ZF){
                pc = memory[pc] | memory[pc+1]<<8;
                machineCycles = 5; // 2 more cycles if condition is met
            }
            else{
                pc += 2;
            }
            break;
        }

        case 0xC3: { // Jump to address, 3bytes
            machineCycles = 3;
            pc = memory[pc] | memory[pc+1]<<8;
            break;
        }

        case 0xC4: { // Call address if not zero, 3bytes
            machineCycles = 3;
            if(flags & ~ZF){
                memory[sp-1] = pc>>8;
                memory[sp-2] = pc;
                sp -= 2;
                pc = memory[pc+1] | memory[pc+2]<<8;
                machineCycles = 6; // 3 more cycles if condition is met
            }
            else{
                pc += 3;
            }
            break;
        }

        case 0xC5: { // Push BC onto stack, 1byte
            machineCycles = 4;
            memory[sp-1] = B;
            memory[sp-2] = C;
            sp -= 2;
            break;
        }

        case 0xC6: { // Add immediate to A, 2bytes
            machineCycles = 2;
            uint16_t result = A + memory[pc+1];
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) + (memory[pc+1] & 0x0F) > 0x0F){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            pc += 2;
            break;
        }

        case 0xC7: { // Call 0x00, 1byte
            machineCycles = 3;
            memory[sp-1] = pc>>8;
            memory[sp-2] = pc;
            sp -= 2;
            pc = 0x00;
            break;
        }

        case 0xC8: { // Return if zero, 1byte
            machineCycles = 2;
            if(flags & ZF){
                pc = memory[sp] | memory[sp+1]<<8;
                sp += 2;
            machineCycles = 5; // 3 more cycles if condition is met
            }
            break;
        }

        case 0xC9: { // Return, 1byte
            machineCycles = 3;
            pc = memory[sp] | memory[sp+1]<<8;
            sp += 2;
            break;
        }

        case 0xCA: { // Jump to address if zero, 3bytes
            machineCycles = 3;
            if(flags & ZF){
                pc = memory[pc+1] | memory[pc+2]<<8;
                machineCycles = 5; // 2 more cycles if condition is met
            }
            else{
                pc += 3;
            }
            break;
        }

        case 0xCC: { // Call address if zero, 3bytes
            machineCycles = 3;
            if(flags & ZF){
                memory[sp-1] = pc>>8;
                memory[sp-2] = pc;
                sp -= 2;
                pc = memory[pc+1] | memory[pc+2]<<8;
                machineCycles = 6; // 3 more cycles if condition is met
            }
            else{
                pc += 3;
            }
            break;
        }

        case 0xCD: { // Call address, 3bytes
            machineCycles = 3;
            memory[sp-1] = pc>>8;
            memory[sp-2] = pc;
            sp -= 2;
            pc = memory[pc+1] | memory[pc+2]<<8;
            break;
        }

        case 0xCE: { // Add immediate and carry to A, 2bytes
            machineCycles = 2;
            uint16_t result = A + memory[pc+1] + (flags & CF);
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) + (memory[pc+1] & 0x0F) + (flags & CF) > 0x0F){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            pc += 2;
            break;
        }

        case 0xCF: { // Call 0x08, 1byte
            machineCycles = 3;
            memory[sp-1] = pc>>8;
            memory[sp-2] = pc;
            sp -= 2;
            pc = 0x08;
            break;
        }

        case 0xD0: { // Return if not carry, 1byte
            machineCycles = 2;
            if(flags & ~CF){
                pc = memory[sp] | memory[sp+1]<<8;
                sp += 2;
            machineCycles = 5; // 3 more cycles if condition is met
            }
            break;
        }

        case 0xD1: { // Pop DE off stack, 1byte
            machineCycles = 3;
            E = memory[sp];
            D = memory[sp+1];
            sp += 2;
            break;
        }

        case 0xD2: { // Jump to address if not carry, 3bytes
            machineCycles = 3;
            if(flags & ~CF){
                pc = memory[pc+1] | memory[pc+2]<<8;
                machineCycles = 5; // 2 more cycles if condition is met
            }
            else{
                pc += 3;
            }
            break;
        }

        case 0xD4: { // Call address if not carry, 3bytes
            machineCycles = 3;
            if(flags & ~CF){
                memory[sp-1] = pc>>8;
                memory[sp-2] = pc;
                sp -= 2;
                pc = memory[pc+1] | memory[pc+2]<<8;
                machineCycles = 6; // 3 more cycles if condition is met
            }
            else{
                pc += 3;
            }
            break;
        }

        case 0xD5: { // Push DE onto stack, 1byte
            machineCycles = 4;
            memory[sp-1] = D;
            memory[sp-2] = E;
            sp -= 2;
            break;
        }

        case 0xD6: { // Subtract immediate from A, 2bytes
            machineCycles = 2;
            uint16_t result = A - memory[pc+1];
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) < (memory[pc+1] & 0x0F)){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags |= NF; // set subtract flag
            pc += 2;
            break;
        }

        case 0xD7: { // Call 0x10, 1byte
            machineCycles = 3;
            memory[sp-1] = pc>>8;
            memory[sp-2] = pc;
            sp -= 2;
            pc = 0x10;
            break;
        }

        case 0xD8: { // Return if carry, 1byte
            machineCycles = 2;
            if(flags & CF){
                pc = memory[sp] | memory[sp+1]<<8;
                sp += 2;
            machineCycles = 5; // 3 more cycles if condition is met
            }
            break;
        }

        case 0xD9: { // Return and enable interrupts, 1byte
            machineCycles = 4;
            pc = memory[sp] | memory[sp+1]<<8;
            sp += 2;
            //TODO: enable interrupts
            break;
        }

        case 0xDA: { // Jump to address if carry, 3bytes
            machineCycles = 3;
            if(flags & CF){
                pc = memory[pc+1] | memory[pc+2]<<8;
                machineCycles = 5; // 2 more cycles if condition is met
            }
            else{
                pc += 3;
            }
            break;
        }

        case 0xDC: { // Call address if carry, 3bytes
            machineCycles = 3;
            if(flags & CF){
                memory[sp-1] = pc>>8;
                memory[sp-2] = pc;
                sp -= 2;
                pc = memory[pc+1] | memory[pc+2]<<8;
                machineCycles = 6; // 3 more cycles if condition is met
            }
            else{
                pc += 3;
            }
            break;
        }

        case 0xDE: { // Subtract immediate and carry from A, 2bytes
            machineCycles = 2;
            uint16_t result = A - memory[pc+1] - (flags & CF);
            A = result & 0x00FF;
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) < (memory[pc+1] & 0x0F) + (flags & CF)){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags |= NF; // set subtract flag
            pc += 2;
            break;
        }

        case 0xDF: { // Call 0x18, 1byte
            machineCycles = 3;
            memory[sp-1] = pc>>8;
            memory[sp-2] = pc;
            sp -= 2;
            pc = 0x18;
            break;
        }

        case 0xE0: { // Load A into 0xFF00 + immediate, 2bytes
            machineCycles = 3;
            memory[0xFF00 + memory[pc+1]] = A;
            pc += 2;
            break;
        }

        case 0xE1: { // Pop HL off stack, 1byte
            machineCycles = 3;
            L = memory[sp];
            H = memory[sp+1];
            sp += 2;
            break;
        }

        case 0xE2: { // Load A into 0xFF00 + C, 1byte
            machineCycles = 2;
            memory[0xFF00 + C] = A;
            break;
        }

        case 0xE5: { // Push HL onto stack, 1byte
            machineCycles = 4;
            memory[sp-1] = H;
            memory[sp-2] = L;
            sp -= 2;
            break;
        }

        case 0xE6: { // AND immediate with A, 2bytes
            machineCycles = 2;
            A &= memory[pc+1];
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            flags |= HF; // set half carry flag
            flags &= ~CF; // reset carry flag
            pc += 2;
            break;
        }

        case 0xE7: { // Call 0x20, 1byte
            machineCycles = 3;
            memory[sp-1] = pc>>8;
            memory[sp-2] = pc;
            sp -= 2;
            pc = 0x20;
            break;
        }

        case 0xE8: { // Add immediate to SP, 2bytes
            machineCycles = 4;
            if((sp & 0x00FF) + memory[pc+1] > 0x00FF){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if((sp & 0xFF) + memory[pc+1] > 0xFF){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            sp += memory[pc+1];
            pc += 2;
            break;
        }

        case 0xE9: { // Jump to HL, 1byte
            machineCycles = 1;
            pc = H<<8 | L;
            break;
        }

        case 0xEA: { // Load A into immediate address, 3bytes
            machineCycles = 4;
            memory[memory[pc+1] | memory[pc+2]<<8] = A;
            pc += 3;
            break;
        }

        case 0xEE: { // XOR immediate with A, 2bytes
            machineCycles = 2;
            A ^= memory[pc+1];
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            flags &= ~HF; // reset half carry flag
            flags &= ~CF; // reset carry flag
            pc += 2;
            break;
        }

        case 0xEF: { // Call 0x28, 1byte
            machineCycles = 3;
            memory[sp-1] = pc>>8;
            memory[sp-2] = pc;
            sp -= 2;
            pc = 0x28;
            break;
        }

        case 0xF0: { // Load 0xFF00 + immediate into A, 2bytes
            machineCycles = 3;
            A = memory[0xFF00 + memory[pc+1]];
            pc += 2;
            break;
        }

        case 0xF1: { // Pop AF off stack, 1byte
            machineCycles = 3;
            flags = memory[sp];
            A = memory[sp+1];
            sp += 2;
            break;
        }

        case 0xF2: { // Load 0xFF00 + C into A, 1byte
            machineCycles = 2;
            A = memory[0xFF00 + C];
            break;
        }

        case 0xF3: { // Disable interrupts, 1byte
            machineCycles = 1;
            //TODO: disable interrupts
            break;
        }

        case 0xF5: { // Push AF onto stack, 1byte
            machineCycles = 4;
            memory[sp-1] = A;
            memory[sp-2] = flags;
            sp -= 2;
            break;
        }

        case 0xF6: { // OR immediate with A, 2bytes
            machineCycles = 2;
            A |= memory[pc+1];
            if(A == 0){
                flags |= ZF; // set zero flag
            }
            flags &= ~NF; // reset subtract flag
            flags &= ~HF; // reset half carry flag
            flags &= ~CF; // reset carry flag
            pc += 2;
            break;
        }

        case 0xF7: { // Call 0x30, 1byte
            machineCycles = 3;
            memory[sp-1] = pc>>8;
            memory[sp-2] = pc;
            sp -= 2;
            pc = 0x30;
            break;
        }

        case 0xF8: { // Load SP + immediate into HL, 2bytes
            machineCycles = 3;
            uint16_t result = sp + memory[pc+1];
            L = result & 0x00FF;
            H = result >> 8;
            if((sp & 0x00FF) + memory[pc+1] > 0x00FF){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if((sp & 0xFF) + memory[pc+1] > 0xFF){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            pc += 2;
            break;
        }

        case 0xF9: { // Load HL into SP, 1byte
            machineCycles = 2;
            sp = H<<8 | L;
            break;
        }

        case 0xFA: { // Load immediate address into A, 3bytes
            machineCycles = 4;
            A = memory[memory[pc+1] | memory[pc+2]<<8];
            pc += 3;
            break;
        }

        case 0xFB: { // Enable interrupts, 1byte
            machineCycles = 1;
            //TODO: enable interrupts
            break;
        }

        case 0xFE: { // Compare immediate with A, 2bytes
            machineCycles = 2;
            uint16_t result = A - memory[pc+1];
            if(result & 0x100){
                flags |= CF; // set carry flag
            }
            else{
                flags &= ~CF; // reset carry flag
            }
            if((A & 0x0F) < (memory[pc+1] & 0x0F)){
                flags |= HF; // set half carry flag
            }
            else{
                flags &= ~HF; // reset half carry flag
            }
            if((A & 0x00FF) == (memory[pc+1] & 0x00FF)){
                flags |= ZF; // set zero flag
            }
            flags |= NF; // set subtract flag
            pc += 2;
            break;
        }

        case 0xFF: { // Call 0x38, 1byte
            machineCycles = 4;
            memory[sp-1] = pc>>8;
            memory[sp-2] = pc;
            sp -= 2;
            pc = 0x38;
            break;
        }

        case 0xCB:{

            uint8_t cbOpcode = memory[pc+1];
            printf("CB prefix\n");

            switch (cbOpcode)
            {
            case 0X0: { 
            
                    machineCycles = 2;
                    uint8_t result =B<< 1 |  + B >> 7;
                    B = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(B & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X1: { 
            
                    machineCycles = 2;
                    uint8_t result =C<< 1 |  + C >> 7;
                    C = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(C & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X2: { 
            
                    machineCycles = 2;
                    uint8_t result =D<< 1 |  + D >> 7;
                    D = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(D & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X3: { 
            
                    machineCycles = 2;
                    uint8_t result =E<< 1 |  + E >> 7;
                    E = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(E & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X4: { 
            
                    machineCycles = 2;
                    uint8_t result =H<< 1 |  + H >> 7;
                    H = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(H & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X5: { 
            
                    machineCycles = 2;
                    uint8_t result =L<< 1 |  + L >> 7;
                    L = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(L & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X6: { 
            
                    machineCycles = 2;
                    uint8_t result =memory[H<<8|L]<< 1 |  + memory[H<<8|L] >> 7;
                    memory[H<<8|L] = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(memory[H<<8|L] & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X7: { 
            
                    machineCycles = 2;
                    uint8_t result =A<< 1 |  + A >> 7;
                    A = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(A & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X8: { 
            
                    machineCycles = 2;
                    uint8_t result =B>> 1 |  + B << 7;
                    B = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(B & 0x80){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X9: { 
            
                    machineCycles = 2;
                    uint8_t result =C>> 1 |  + C << 7;
                    C = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(C & 0x80){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0XA: { 
            
                    machineCycles = 2;
                    uint8_t result =D>> 1 |  + D << 7;
                    D = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(D & 0x80){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0XB: { 
            
                    machineCycles = 2;
                    uint8_t result =E>> 1 |  + E << 7;
                    E = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(E & 0x80){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0XC: { 
            
                    machineCycles = 2;
                    uint8_t result =H>> 1 |  + H << 7;
                    H = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(H & 0x80){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0XD: { 
            
                    machineCycles = 2;
                    uint8_t result =L>> 1 |  + L << 7;
                    L = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(L & 0x80){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0XE: { 
            
                    machineCycles = 2;
                    uint8_t result =memory[H<<8|L]>> 1 |  + memory[H<<8|L] << 7;
                    memory[H<<8|L] = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(memory[H<<8|L] & 0x80){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0XF: { 
            
                    machineCycles = 2;
                    uint8_t result =A>> 1 |  + A << 7;
                    A = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(A & 0x80){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X10: { 
            
                    machineCycles = 2;
                    uint8_t result =B<< 1 |  + flags & CF;
                    B = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(B & 0x80){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X11: { 
            
                    machineCycles = 2;
                    uint8_t result =C<< 1 |  + flags & CF;
                    C = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(C & 0x80){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X12: { 
            
                    machineCycles = 2;
                    uint8_t result =D<< 1 |  + flags & CF;
                    D = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(D & 0x80){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X13: { 
            
                    machineCycles = 2;
                    uint8_t result =E<< 1 |  + flags & CF;
                    E = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(E & 0x80){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X14: { 
            
                    machineCycles = 2;
                    uint8_t result =H<< 1 |  + flags & CF;
                    H = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(H & 0x80){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X15: { 
            
                    machineCycles = 2;
                    uint8_t result =L<< 1 |  + flags & CF;
                    L = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(L & 0x80){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X16: { 
            
                    machineCycles = 2;
                    uint8_t result =memory[H<<8|L]<< 1 |  + flags & CF;
                    memory[H<<8|L] = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(memory[H<<8|L] & 0x80){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X17: { 
            
                    machineCycles = 2;
                    uint8_t result =A<< 1 |  + flags & CF;
                    A = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(A & 0x80){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X18: { 
            
                    machineCycles = 2;
                    uint8_t result =B>> 1 |  + flags & CF;
                    B = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(B & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X19: { 
            
                    machineCycles = 2;
                    uint8_t result =C>> 1 |  + flags & CF;
                    C = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(C & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X1A: { 
            
                    machineCycles = 2;
                    uint8_t result =D>> 1 |  + flags & CF;
                    D = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(D & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X1B: { 
            
                    machineCycles = 2;
                    uint8_t result =E>> 1 |  + flags & CF;
                    E = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(E & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X1C: { 
            
                    machineCycles = 2;
                    uint8_t result =H>> 1 |  + flags & CF;
                    H = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(H & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X1D: { 
            
                    machineCycles = 2;
                    uint8_t result =L>> 1 |  + flags & CF;
                    L = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(L & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X1E: { 
            
                    machineCycles = 2;
                    uint8_t result =memory[H<<8|L]>> 1 |  + flags & CF;
                    memory[H<<8|L] = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(memory[H<<8|L] & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X1F: { 
            
                    machineCycles = 2;
                    uint8_t result =A>> 1 |  + flags & CF;
                    A = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(A & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X20: { 
            
                    machineCycles = 2;
                    uint8_t result =B<< 1;
                    B = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(B & 0x80){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X21: { 
            
                    machineCycles = 2;
                    uint8_t result =C<< 1;
                    C = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(C & 0x80){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X22: { 
            
                    machineCycles = 2;
                    uint8_t result =D<< 1;
                    D = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(D & 0x80){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X23: { 
            
                    machineCycles = 2;
                    uint8_t result =E<< 1;
                    E = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(E & 0x80){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X24: { 
            
                    machineCycles = 2;
                    uint8_t result =H<< 1;
                    H = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(H & 0x80){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X25: { 
            
                    machineCycles = 2;
                    uint8_t result =L<< 1;
                    L = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(L & 0x80){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X26: { 
            
                    machineCycles = 2;
                    uint8_t result =memory[H<<8|L]<< 1;
                    memory[H<<8|L] = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(memory[H<<8|L] & 0x80){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X27: { 
            
                    machineCycles = 2;
                    uint8_t result =A<< 1;
                    A = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(A & 0x80){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X28: { 
            
                    machineCycles = 2;
                    uint8_t result =B>> 1 | B & 0x80;
                    B = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(B & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X29: { 
            
                    machineCycles = 2;
                    uint8_t result =C>> 1 | C & 0x80;
                    C = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(C & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X2A: { 
            
                    machineCycles = 2;
                    uint8_t result =D>> 1 | D & 0x80;
                    D = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(D & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X2B: { 
            
                    machineCycles = 2;
                    uint8_t result =E>> 1 | E & 0x80;
                    E = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(E & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X2C: { 
            
                    machineCycles = 2;
                    uint8_t result =H>> 1 | H & 0x80;
                    H = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(H & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X2D: { 
            
                    machineCycles = 2;
                    uint8_t result =L>> 1 | L & 0x80;
                    L = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(L & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X2E: { 
            
                    machineCycles = 2;
                    uint8_t result =memory[H<<8|L]>> 1 | memory[H<<8|L] & 0x80;
                    memory[H<<8|L] = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(memory[H<<8|L] & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X2F: { 
            
                    machineCycles = 2;
                    uint8_t result =A>> 1 | A & 0x80;
                    A = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(A & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X30: { 
            
                    machineCycles = 2;
                    uint8_t result =(B & 0x0F) << 4 | B >> 4;
                    B = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                flags &= ~CF; // reset carry flag
                                pc += 2;
                                break;
                    } 

            case 0X31: { 
            
                    machineCycles = 2;
                    uint8_t result =(C & 0x0F) << 4 | C >> 4;
                    C = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                flags &= ~CF; // reset carry flag
                                pc += 2;
                                break;
                    } 

            case 0X32: { 
            
                    machineCycles = 2;
                    uint8_t result =(D & 0x0F) << 4 | D >> 4;
                    D = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                flags &= ~CF; // reset carry flag
                                pc += 2;
                                break;
                    } 

            case 0X33: { 
            
                    machineCycles = 2;
                    uint8_t result =(E & 0x0F) << 4 | E >> 4;
                    E = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                flags &= ~CF; // reset carry flag
                                pc += 2;
                                break;
                    } 

            case 0X34: { 
            
                    machineCycles = 2;
                    uint8_t result =(H & 0x0F) << 4 | H >> 4;
                    H = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                flags &= ~CF; // reset carry flag
                                pc += 2;
                                break;
                    } 

            case 0X35: { 
            
                    machineCycles = 2;
                    uint8_t result =(L & 0x0F) << 4 | L >> 4;
                    L = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                flags &= ~CF; // reset carry flag
                                pc += 2;
                                break;
                    } 

            case 0X36: { 
            
                    machineCycles = 2;
                    uint8_t result =(memory[H<<8|L] & 0x0F) << 4 | memory[H<<8|L] >> 4;
                    memory[H<<8|L] = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                flags &= ~CF; // reset carry flag
                                pc += 2;
                                break;
                    } 

            case 0X37: { 
            
                    machineCycles = 2;
                    uint8_t result =(A & 0x0F) << 4 | A >> 4;
                    A = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                flags &= ~CF; // reset carry flag
                                pc += 2;
                                break;
                    } 

            case 0X38: { 
            
                    machineCycles = 2;
                    uint8_t result =B>> 1;
                    B = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(B & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X39: { 
            
                    machineCycles = 2;
                    uint8_t result =C>> 1;
                    C = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(C & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X3A: { 
            
                    machineCycles = 2;
                    uint8_t result =D>> 1;
                    D = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(D & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X3B: { 
            
                    machineCycles = 2;
                    uint8_t result =E>> 1;
                    E = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(E & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X3C: { 
            
                    machineCycles = 2;
                    uint8_t result =H>> 1;
                    H = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(H & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X3D: { 
            
                    machineCycles = 2;
                    uint8_t result =L>> 1;
                    L = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(L & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X3E: { 
            
                    machineCycles = 2;
                    uint8_t result =memory[H<<8|L]>> 1;
                    memory[H<<8|L] = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(memory[H<<8|L] & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X3F: { 
            
                    machineCycles = 2;
                    uint8_t result =A>> 1;
                    A = result;
                    if(result == 0){
                                    flags |= ZF; // set zero flag
                                }
                                else{
                                    flags &= ~ZF; // reset zero flag
                                }
                                flags &= ~NF; // reset subtract flag
                                flags &= ~HF; // reset half carry flag
                                if(A & 0x01){
                                    flags |= CF; // set carry flag
                                }
                                else{
                                    flags &= ~CF; // reset carry flag
                                }
                                pc += 2;
                                break;
                    } 

            case 0X40: { 
            
                    machineCycles = 2;
                    if((1 << B) & B){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X41: { 
            
                    machineCycles = 2;
                    if((1 << C) & C){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X42: { 
            
                    machineCycles = 2;
                    if((1 << D) & D){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X43: { 
            
                    machineCycles = 2;
                    if((1 << E) & E){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X44: { 
            
                    machineCycles = 2;
                    if((1 << H) & H){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X45: { 
            
                    machineCycles = 2;
                    if((1 << L) & L){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X46: { 
            
                    machineCycles = 2;
                    if((1 << memory[H<<8|L]) & memory[H<<8|L]){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X47: { 
            
                    machineCycles = 2;
                    if((1 << A) & A){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X48: { 
            
                    machineCycles = 2;
                    if((1 << B) & B){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X49: { 
            
                    machineCycles = 2;
                    if((1 << C) & C){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X4A: { 
            
                    machineCycles = 2;
                    if((1 << D) & D){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X4B: { 
            
                    machineCycles = 2;
                    if((1 << E) & E){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X4C: { 
            
                    machineCycles = 2;
                    if((1 << H) & H){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X4D: { 
            
                    machineCycles = 2;
                    if((1 << L) & L){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X4E: { 
            
                    machineCycles = 2;
                    if((1 << memory[H<<8|L]) & memory[H<<8|L]){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X4F: { 
            
                    machineCycles = 2;
                    if((1 << A) & A){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X50: { 
            
                    machineCycles = 2;
                    if((1 << B) & B){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X51: { 
            
                    machineCycles = 2;
                    if((1 << C) & C){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X52: { 
            
                    machineCycles = 2;
                    if((1 << D) & D){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X53: { 
            
                    machineCycles = 2;
                    if((1 << E) & E){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X54: { 
            
                    machineCycles = 2;
                    if((1 << H) & H){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X55: { 
            
                    machineCycles = 2;
                    if((1 << L) & L){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X56: { 
            
                    machineCycles = 2;
                    if((1 << memory[H<<8|L]) & memory[H<<8|L]){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X57: { 
            
                    machineCycles = 2;
                    if((1 << A) & A){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X58: { 
            
                    machineCycles = 2;
                    if((1 << B) & B){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X59: { 
            
                    machineCycles = 2;
                    if((1 << C) & C){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X5A: { 
            
                    machineCycles = 2;
                    if((1 << D) & D){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X5B: { 
            
                    machineCycles = 2;
                    if((1 << E) & E){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X5C: { 
            
                    machineCycles = 2;
                    if((1 << H) & H){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X5D: { 
            
                    machineCycles = 2;
                    if((1 << L) & L){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X5E: { 
            
                    machineCycles = 2;
                    if((1 << memory[H<<8|L]) & memory[H<<8|L]){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X5F: { 
            
                    machineCycles = 2;
                    if((1 << A) & A){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X60: { 
            
                    machineCycles = 2;
                    if((1 << B) & B){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X61: { 
            
                    machineCycles = 2;
                    if((1 << C) & C){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X62: { 
            
                    machineCycles = 2;
                    if((1 << D) & D){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X63: { 
            
                    machineCycles = 2;
                    if((1 << E) & E){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X64: { 
            
                    machineCycles = 2;
                    if((1 << H) & H){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X65: { 
            
                    machineCycles = 2;
                    if((1 << L) & L){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X66: { 
            
                    machineCycles = 2;
                    if((1 << memory[H<<8|L]) & memory[H<<8|L]){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X67: { 
            
                    machineCycles = 2;
                    if((1 << A) & A){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X68: { 
            
                    machineCycles = 2;
                    if((1 << B) & B){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X69: { 
            
                    machineCycles = 2;
                    if((1 << C) & C){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X6A: { 
            
                    machineCycles = 2;
                    if((1 << D) & D){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X6B: { 
            
                    machineCycles = 2;
                    if((1 << E) & E){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X6C: { 
            
                    machineCycles = 2;
                    if((1 << H) & H){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X6D: { 
            
                    machineCycles = 2;
                    if((1 << L) & L){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X6E: { 
            
                    machineCycles = 2;
                    if((1 << memory[H<<8|L]) & memory[H<<8|L]){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X6F: { 
            
                    machineCycles = 2;
                    if((1 << A) & A){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X70: { 
            
                    machineCycles = 2;
                    if((1 << B) & B){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X71: { 
            
                    machineCycles = 2;
                    if((1 << C) & C){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X72: { 
            
                    machineCycles = 2;
                    if((1 << D) & D){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X73: { 
            
                    machineCycles = 2;
                    if((1 << E) & E){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X74: { 
            
                    machineCycles = 2;
                    if((1 << H) & H){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X75: { 
            
                    machineCycles = 2;
                    if((1 << L) & L){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X76: { 
            
                    machineCycles = 2;
                    if((1 << memory[H<<8|L]) & memory[H<<8|L]){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X77: { 
            
                    machineCycles = 2;
                    if((1 << A) & A){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X78: { 
            
                    machineCycles = 2;
                    if((1 << B) & B){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X79: { 
            
                    machineCycles = 2;
                    if((1 << C) & C){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X7A: { 
            
                    machineCycles = 2;
                    if((1 << D) & D){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X7B: { 
            
                    machineCycles = 2;
                    if((1 << E) & E){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X7C: { 
            
                    machineCycles = 2;
                    if((1 << H) & H){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X7D: { 
            
                    machineCycles = 2;
                    if((1 << L) & L){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X7E: { 
            
                    machineCycles = 2;
                    if((1 << memory[H<<8|L]) & memory[H<<8|L]){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X7F: { 
            
                    machineCycles = 2;
                    if((1 << A) & A){
                        flags &= ~ZF; // reset zero flag
                    }
                    else{
                        flags |= ZF; // set zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags |= HF; // set half carry flag
                    pc += 2;
                    break;
                    } 

            case 0X80: { 
            
                    machineCycles = 2;
                    B &= ~(1 << B);
                    pc += 2;
                    break;
                    } 

            case 0X81: { 
            
                    machineCycles = 2;
                    C &= ~(1 << C);
                    pc += 2;
                    break;
                    } 

            case 0X82: { 
            
                    machineCycles = 2;
                    D &= ~(1 << D);
                    pc += 2;
                    break;
                    } 

            case 0X83: { 
            
                    machineCycles = 2;
                    E &= ~(1 << E);
                    pc += 2;
                    break;
                    } 

            case 0X84: { 
            
                    machineCycles = 2;
                    H &= ~(1 << H);
                    pc += 2;
                    break;
                    } 

            case 0X85: { 
            
                    machineCycles = 2;
                    L &= ~(1 << L);
                    pc += 2;
                    break;
                    } 

            case 0X86: { 
            
                    machineCycles = 2;
                    memory[H<<8|L] &= ~(1 << memory[H<<8|L]);
                    pc += 2;
                    break;
                    } 

            case 0X87: { 
            
                    machineCycles = 2;
                    A &= ~(1 << A);
                    pc += 2;
                    break;
                    } 

            case 0X88: { 
            
                    machineCycles = 2;
                    B &= ~(1 << B);
                    pc += 2;
                    break;
                    } 

            case 0X89: { 
            
                    machineCycles = 2;
                    C &= ~(1 << C);
                    pc += 2;
                    break;
                    } 

            case 0X8A: { 
            
                    machineCycles = 2;
                    D &= ~(1 << D);
                    pc += 2;
                    break;
                    } 

            case 0X8B: { 
            
                    machineCycles = 2;
                    E &= ~(1 << E);
                    pc += 2;
                    break;
                    } 

            case 0X8C: { 
            
                    machineCycles = 2;
                    H &= ~(1 << H);
                    pc += 2;
                    break;
                    } 

            case 0X8D: { 
            
                    machineCycles = 2;
                    L &= ~(1 << L);
                    pc += 2;
                    break;
                    } 

            case 0X8E: { 
            
                    machineCycles = 2;
                    memory[H<<8|L] &= ~(1 << memory[H<<8|L]);
                    pc += 2;
                    break;
                    } 

            case 0X8F: { 
            
                    machineCycles = 2;
                    A &= ~(1 << A);
                    pc += 2;
                    break;
                    } 

            case 0X90: { 
            
                    machineCycles = 2;
                    B &= ~(1 << B);
                    pc += 2;
                    break;
                    } 

            case 0X91: { 
            
                    machineCycles = 2;
                    C &= ~(1 << C);
                    pc += 2;
                    break;
                    } 

            case 0X92: { 
            
                    machineCycles = 2;
                    D &= ~(1 << D);
                    pc += 2;
                    break;
                    } 

            case 0X93: { 
            
                    machineCycles = 2;
                    E &= ~(1 << E);
                    pc += 2;
                    break;
                    } 

            case 0X94: { 
            
                    machineCycles = 2;
                    H &= ~(1 << H);
                    pc += 2;
                    break;
                    } 

            case 0X95: { 
            
                    machineCycles = 2;
                    L &= ~(1 << L);
                    pc += 2;
                    break;
                    } 

            case 0X96: { 
            
                    machineCycles = 2;
                    memory[H<<8|L] &= ~(1 << memory[H<<8|L]);
                    pc += 2;
                    break;
                    } 

            case 0X97: { 
            
                    machineCycles = 2;
                    A &= ~(1 << A);
                    pc += 2;
                    break;
                    } 

            case 0X98: { 
            
                    machineCycles = 2;
                    B &= ~(1 << B);
                    pc += 2;
                    break;
                    } 

            case 0X99: { 
            
                    machineCycles = 2;
                    C &= ~(1 << C);
                    pc += 2;
                    break;
                    } 

            case 0X9A: { 
            
                    machineCycles = 2;
                    D &= ~(1 << D);
                    pc += 2;
                    break;
                    } 

            case 0X9B: { 
            
                    machineCycles = 2;
                    E &= ~(1 << E);
                    pc += 2;
                    break;
                    } 

            case 0X9C: { 
            
                    machineCycles = 2;
                    H &= ~(1 << H);
                    pc += 2;
                    break;
                    } 

            case 0X9D: { 
            
                    machineCycles = 2;
                    L &= ~(1 << L);
                    pc += 2;
                    break;
                    } 

            case 0X9E: { 
            
                    machineCycles = 2;
                    memory[H<<8|L] &= ~(1 << memory[H<<8|L]);
                    pc += 2;
                    break;
                    } 

            case 0X9F: { 
            
                    machineCycles = 2;
                    A &= ~(1 << A);
                    pc += 2;
                    break;
                    } 

            case 0XA0: { 
            
                    machineCycles = 2;
                    B &= ~(1 << B);
                    pc += 2;
                    break;
                    } 

            case 0XA1: { 
            
                    machineCycles = 2;
                    C &= ~(1 << C);
                    pc += 2;
                    break;
                    } 

            case 0XA2: { 
            
                    machineCycles = 2;
                    D &= ~(1 << D);
                    pc += 2;
                    break;
                    } 

            case 0XA3: { 
            
                    machineCycles = 2;
                    E &= ~(1 << E);
                    pc += 2;
                    break;
                    } 

            case 0XA4: { 
            
                    machineCycles = 2;
                    H &= ~(1 << H);
                    pc += 2;
                    break;
                    } 

            case 0XA5: { 
            
                    machineCycles = 2;
                    L &= ~(1 << L);
                    pc += 2;
                    break;
                    } 

            case 0XA6: { 
            
                    machineCycles = 2;
                    memory[H<<8|L] &= ~(1 << memory[H<<8|L]);
                    pc += 2;
                    break;
                    } 

            case 0XA7: { 
            
                    machineCycles = 2;
                    A &= ~(1 << A);
                    pc += 2;
                    break;
                    } 

            case 0XA8: { 
            
                    machineCycles = 2;
                    B &= ~(1 << B);
                    pc += 2;
                    break;
                    } 

            case 0XA9: { 
            
                    machineCycles = 2;
                    C &= ~(1 << C);
                    pc += 2;
                    break;
                    } 

            case 0XAA: { 
            
                    machineCycles = 2;
                    D &= ~(1 << D);
                    pc += 2;
                    break;
                    } 

            case 0XAB: { 
            
                    machineCycles = 2;
                    E &= ~(1 << E);
                    pc += 2;
                    break;
                    } 

            case 0XAC: { 
            
                    machineCycles = 2;
                    H &= ~(1 << H);
                    pc += 2;
                    break;
                    } 

            case 0XAD: { 
            
                    machineCycles = 2;
                    L &= ~(1 << L);
                    pc += 2;
                    break;
                    } 

            case 0XAE: { 
            
                    machineCycles = 2;
                    memory[H<<8|L] &= ~(1 << memory[H<<8|L]);
                    pc += 2;
                    break;
                    } 

            case 0XAF: { 
            
                    machineCycles = 2;
                    A &= ~(1 << A);
                    pc += 2;
                    break;
                    } 

            case 0XB0: { 
            
                    machineCycles = 2;
                    B &= ~(1 << B);
                    pc += 2;
                    break;
                    } 

            case 0XB1: { 
            
                    machineCycles = 2;
                    C &= ~(1 << C);
                    pc += 2;
                    break;
                    } 

            case 0XB2: { 
            
                    machineCycles = 2;
                    D &= ~(1 << D);
                    pc += 2;
                    break;
                    } 

            case 0XB3: { 
            
                    machineCycles = 2;
                    E &= ~(1 << E);
                    pc += 2;
                    break;
                    } 

            case 0XB4: { 
            
                    machineCycles = 2;
                    H &= ~(1 << H);
                    pc += 2;
                    break;
                    } 

            case 0XB5: { 
            
                    machineCycles = 2;
                    L &= ~(1 << L);
                    pc += 2;
                    break;
                    } 

            case 0XB6: { 
            
                    machineCycles = 2;
                    memory[H<<8|L] &= ~(1 << memory[H<<8|L]);
                    pc += 2;
                    break;
                    } 

            case 0XB7: { 
            
                    machineCycles = 2;
                    A &= ~(1 << A);
                    pc += 2;
                    break;
                    } 

            case 0XB8: { 
            
                    machineCycles = 2;
                    B &= ~(1 << B);
                    pc += 2;
                    break;
                    } 

            case 0XB9: { 
            
                    machineCycles = 2;
                    C &= ~(1 << C);
                    pc += 2;
                    break;
                    } 

            case 0XBA: { 
            
                    machineCycles = 2;
                    D &= ~(1 << D);
                    pc += 2;
                    break;
                    } 

            case 0XBB: { 
            
                    machineCycles = 2;
                    E &= ~(1 << E);
                    pc += 2;
                    break;
                    } 

            case 0XBC: { 
            
                    machineCycles = 2;
                    H &= ~(1 << H);
                    pc += 2;
                    break;
                    } 

            case 0XBD: { 
            
                    machineCycles = 2;
                    L &= ~(1 << L);
                    pc += 2;
                    break;
                    } 

            case 0XBE: { 
            
                    machineCycles = 2;
                    memory[H<<8|L] &= ~(1 << memory[H<<8|L]);
                    pc += 2;
                    break;
                    } 

            case 0XBF: { 
            
                    machineCycles = 2;
                    A &= ~(1 << A);
                    pc += 2;
                    break;
                    } 

            case 0XC0: { 
            
                    machineCycles = 2;
                    B |= (1 << B);
                    pc += 2;
                    break;
                    } 

            case 0XC1: { 
            
                    machineCycles = 2;
                    C |= (1 << C);
                    pc += 2;
                    break;
                    } 

            case 0XC2: { 
            
                    machineCycles = 2;
                    D |= (1 << D);
                    pc += 2;
                    break;
                    } 

            case 0XC3: { 
            
                    machineCycles = 2;
                    E |= (1 << E);
                    pc += 2;
                    break;
                    } 

            case 0XC4: { 
            
                    machineCycles = 2;
                    H |= (1 << H);
                    pc += 2;
                    break;
                    } 

            case 0XC5: { 
            
                    machineCycles = 2;
                    L |= (1 << L);
                    pc += 2;
                    break;
                    } 

            case 0XC6: { 
            
                    machineCycles = 2;
                    memory[H<<8|L] |= (1 << memory[H<<8|L]);
                    pc += 2;
                    break;
                    } 

            case 0XC7: { 
            
                    machineCycles = 2;
                    A |= (1 << A);
                    pc += 2;
                    break;
                    } 

            case 0XC8: { 
            
                    machineCycles = 2;
                    B |= (1 << B);
                    pc += 2;
                    break;
                    } 

            case 0XC9: { 
            
                    machineCycles = 2;
                    C |= (1 << C);
                    pc += 2;
                    break;
                    } 

            case 0XCA: { 
            
                    machineCycles = 2;
                    D |= (1 << D);
                    pc += 2;
                    break;
                    } 

            case 0XCB: { 
            
                    machineCycles = 2;
                    E |= (1 << E);
                    pc += 2;
                    break;
                    } 

            case 0XCC: { 
            
                    machineCycles = 2;
                    H |= (1 << H);
                    pc += 2;
                    break;
                    } 

            case 0XCD: { 
            
                    machineCycles = 2;
                    L |= (1 << L);
                    pc += 2;
                    break;
                    } 

            case 0XCE: { 
            
                    machineCycles = 2;
                    memory[H<<8|L] |= (1 << memory[H<<8|L]);
                    pc += 2;
                    break;
                    } 

            case 0XCF: { 
            
                    machineCycles = 2;
                    A |= (1 << A);
                    pc += 2;
                    break;
                    } 

            case 0XD0: { 
            
                    machineCycles = 2;
                    B |= (1 << B);
                    pc += 2;
                    break;
                    } 

            case 0XD1: { 
            
                    machineCycles = 2;
                    C |= (1 << C);
                    pc += 2;
                    break;
                    } 

            case 0XD2: { 
            
                    machineCycles = 2;
                    D |= (1 << D);
                    pc += 2;
                    break;
                    } 

            case 0XD3: { 
            
                    machineCycles = 2;
                    E |= (1 << E);
                    pc += 2;
                    break;
                    } 

            case 0XD4: { 
            
                    machineCycles = 2;
                    H |= (1 << H);
                    pc += 2;
                    break;
                    } 

            case 0XD5: { 
            
                    machineCycles = 2;
                    L |= (1 << L);
                    pc += 2;
                    break;
                    } 

            case 0XD6: { 
            
                    machineCycles = 2;
                    memory[H<<8|L] |= (1 << memory[H<<8|L]);
                    pc += 2;
                    break;
                    } 

            case 0XD7: { 
            
                    machineCycles = 2;
                    A |= (1 << A);
                    pc += 2;
                    break;
                    } 

            case 0XD8: { 
            
                    machineCycles = 2;
                    B |= (1 << B);
                    pc += 2;
                    break;
                    } 

            case 0XD9: { 
            
                    machineCycles = 2;
                    C |= (1 << C);
                    pc += 2;
                    break;
                    } 

            case 0XDA: { 
            
                    machineCycles = 2;
                    D |= (1 << D);
                    pc += 2;
                    break;
                    } 

            case 0XDB: { 
            
                    machineCycles = 2;
                    E |= (1 << E);
                    pc += 2;
                    break;
                    } 

            case 0XDC: { 
            
                    machineCycles = 2;
                    H |= (1 << H);
                    pc += 2;
                    break;
                    } 

            case 0XDD: { 
            
                    machineCycles = 2;
                    L |= (1 << L);
                    pc += 2;
                    break;
                    } 

            case 0XDE: { 
            
                    machineCycles = 2;
                    memory[H<<8|L] |= (1 << memory[H<<8|L]);
                    pc += 2;
                    break;
                    } 

            case 0XDF: { 
            
                    machineCycles = 2;
                    A |= (1 << A);
                    pc += 2;
                    break;
                    } 

            case 0XE0: { 
            
                    machineCycles = 2;
                    B |= (1 << B);
                    pc += 2;
                    break;
                    } 

            case 0XE1: { 
            
                    machineCycles = 2;
                    C |= (1 << C);
                    pc += 2;
                    break;
                    } 

            case 0XE2: { 
            
                    machineCycles = 2;
                    D |= (1 << D);
                    pc += 2;
                    break;
                    } 

            case 0XE3: { 
            
                    machineCycles = 2;
                    E |= (1 << E);
                    pc += 2;
                    break;
                    } 

            case 0XE4: { 
            
                    machineCycles = 2;
                    H |= (1 << H);
                    pc += 2;
                    break;
                    } 

            case 0XE5: { 
            
                    machineCycles = 2;
                    L |= (1 << L);
                    pc += 2;
                    break;
                    } 

            case 0XE6: { 
            
                    machineCycles = 2;
                    memory[H<<8|L] |= (1 << memory[H<<8|L]);
                    pc += 2;
                    break;
                    } 

            case 0XE7: { 
            
                    machineCycles = 2;
                    A |= (1 << A);
                    pc += 2;
                    break;
                    } 

            case 0XE8: { 
            
                    machineCycles = 2;
                    B |= (1 << B);
                    pc += 2;
                    break;
                    } 

            case 0XE9: { 
            
                    machineCycles = 2;
                    C |= (1 << C);
                    pc += 2;
                    break;
                    } 

            case 0XEA: { 
            
                    machineCycles = 2;
                    D |= (1 << D);
                    pc += 2;
                    break;
                    } 

            case 0XEB: { 
            
                    machineCycles = 2;
                    E |= (1 << E);
                    pc += 2;
                    break;
                    } 

            case 0XEC: { 
            
                    machineCycles = 2;
                    H |= (1 << H);
                    pc += 2;
                    break;
                    } 

            case 0XED: { 
            
                    machineCycles = 2;
                    L |= (1 << L);
                    pc += 2;
                    break;
                    } 

            case 0XEE: { 
            
                    machineCycles = 2;
                    memory[H<<8|L] |= (1 << memory[H<<8|L]);
                    pc += 2;
                    break;
                    } 

            case 0XEF: { 
            
                    machineCycles = 2;
                    A |= (1 << A);
                    pc += 2;
                    break;
                    } 

            case 0XF0: { 
            
                    machineCycles = 2;
                    B |= (1 << B);
                    pc += 2;
                    break;
                    } 

            case 0XF1: { 
            
                    machineCycles = 2;
                    C |= (1 << C);
                    pc += 2;
                    break;
                    } 

            case 0XF2: { 
            
                    machineCycles = 2;
                    D |= (1 << D);
                    pc += 2;
                    break;
                    } 

            case 0XF3: { 
            
                    machineCycles = 2;
                    E |= (1 << E);
                    pc += 2;
                    break;
                    } 

            case 0XF4: { 
            
                    machineCycles = 2;
                    H |= (1 << H);
                    pc += 2;
                    break;
                    } 

            case 0XF5: { 
            
                    machineCycles = 2;
                    L |= (1 << L);
                    pc += 2;
                    break;
                    } 

            case 0XF6: { 
            
                    machineCycles = 2;
                    memory[H<<8|L] |= (1 << memory[H<<8|L]);
                    pc += 2;
                    break;
                    } 

            case 0XF7: { 
            
                    machineCycles = 2;
                    A |= (1 << A);
                    pc += 2;
                    break;
                    } 

            case 0XF8: { 
            
                    machineCycles = 2;
                    B |= (1 << B);
                    pc += 2;
                    break;
                    } 

            case 0XF9: { 
            
                    machineCycles = 2;
                    C |= (1 << C);
                    pc += 2;
                    break;
                    } 

            case 0XFA: { 
            
                    machineCycles = 2;
                    D |= (1 << D);
                    pc += 2;
                    break;
                    } 

            case 0XFB: { 
            
                    machineCycles = 2;
                    E |= (1 << E);
                    pc += 2;
                    break;
                    } 

            case 0XFC: { 
            
                    machineCycles = 2;
                    H |= (1 << H);
                    pc += 2;
                    break;
                    } 

            case 0XFD: { 
            
                    machineCycles = 2;
                    L |= (1 << L);
                    pc += 2;
                    break;
                    } 

            case 0XFE: { 
            
                    machineCycles = 2;
                    memory[H<<8|L] |= (1 << memory[H<<8|L]);
                    pc += 2;
                    break;
                    } 

            case 0XFF: { 
            
                    machineCycles = 2;
                    A |= (1 << A);
                    pc += 2;
                    break;
                    } 

                        default:
                            break;
                        }
                        break;
                    }

                    default: {
                        printf("Unknown opcode: 0x%02X\n", opcode);
                        break;
                    }

                    
                }

            }


