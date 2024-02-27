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
    uint16_t pc = 0x00;

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





void fetchOpcode(){
    machineCycles = 1; //default cycles per instruction is one
    opcode = memory[pc];
    pc++;
    printf("opcode: 0x%X\n", opcode);
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
                pc = memory[pc+1] | memory[pc+2]<<8;
                machineCycles = 5; // 2 more cycles if condition is met
            }
            else{
                pc += 3;
            }
            break;
        }

        case 0xC3: { // Jump to address, 3bytes
            machineCycles = 3;
            pc = memory[pc+1] | memory[pc+2]<<8;
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
            machineCycles = 3;
            memory[sp-1] = pc>>8;
            memory[sp-2] = pc;
            sp -= 2;
            pc = 0x38;
            break;
        }

        case 0xCB:{

            uint8_t cbOpcode = memory[pc+1];
            printf("CB prefix\n");

            //this is to determine which register to use, check header for defines

            uint8_t currentRegister = (cbOpcode & 0x0F);
            if (currentRegister > 7) {
                currentRegister -= 8;
            }

            switch (cbOpcode & 0xF0)
            {
                case 0x00:{ //RLC or RRC








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


