

print("hello world")


Start = "case 0xCB: {\n\n"

End = "} \n"

def printStartOpcode(Opcode):

    #OpcodeMem = getOperation(Opcode)
    #CurReg = getReg(Reg)

    s = "case " + str(hex(Opcode)).upper() + ": { \n"

    return s



def printMidOpcode(Opcode):

    CurOpcode = getOperation(Opcode)
    CurReg = getReg(Opcode)

    if (CurOpcode == "RLC"):
        s = ''' 
        machineCycles = 2;
        uint8_t result =''' + CurReg + '''<< 1 |  + ''' + CurReg + ''' >> 7;
        ''' + CurReg + ''' = result;
        if(result == 0){
                        flags |= ZF; // set zero flag
                    }
                    else{
                        flags &= ~ZF; // reset zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags &= ~HF; // reset half carry flag
                    if(''' + CurReg + ''' & 0x01){
                        flags |= CF; // set carry flag
                    }
                    else{
                        flags &= ~CF; // reset carry flag
                    }
                    pc += 2;
                    break;
        '''

    if (CurOpcode == "RRC"):
        s = ''' 
        machineCycles = 2;
        uint8_t result =''' + CurReg + '''>> 1 |  + ''' + CurReg + ''' << 7;
        ''' + CurReg + ''' = result;
        if(result == 0){
                        flags |= ZF; // set zero flag
                    }
                    else{
                        flags &= ~ZF; // reset zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags &= ~HF; // reset half carry flag
                    if(''' + CurReg + ''' & 0x80){
                        flags |= CF; // set carry flag
                    }
                    else{
                        flags &= ~CF; // reset carry flag
                    }
                    pc += 2;
                    break;
        '''

    if (CurOpcode == "RL"):
        s = ''' 
        machineCycles = 2;
        uint8_t result =''' + CurReg + '''<< 1 |  + flags & CF;
        ''' + CurReg + ''' = result;
        if(result == 0){
                        flags |= ZF; // set zero flag
                    }
                    else{
                        flags &= ~ZF; // reset zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags &= ~HF; // reset half carry flag
                    if(''' + CurReg + ''' & 0x80){
                        flags |= CF; // set carry flag
                    }
                    else{
                        flags &= ~CF; // reset carry flag
                    }
                    pc += 2;
                    break;
        '''

    if (CurOpcode == "RR"): 
        s = ''' 
        machineCycles = 2;
        uint8_t result =''' + CurReg + '''>> 1 |  + flags & CF;
        ''' + CurReg + ''' = result;
        if(result == 0){
                        flags |= ZF; // set zero flag
                    }
                    else{
                        flags &= ~ZF; // reset zero flag
                    }
                    flags &= ~NF; // reset subtract flag
                    flags &= ~HF; // reset half carry flag
                    if(''' + CurReg + ''' & 0x01){
                        flags |= CF; // set carry flag
                    }
                    else{
                        flags &= ~CF; // reset carry flag
                    }
                    pc += 2;
                    break;
        '''

        if (CurOpcode == "SLA"):    
            s = ''' 
            machineCycles = 2;
            uint8_t result =''' + CurReg + '''<< 1;
            ''' + CurReg + ''' = result;
            if(result == 0){
                            flags |= ZF; // set zero flag
                        }
                        else{
                            flags &= ~ZF; // reset zero flag
                        }
                        flags &= ~NF; // reset subtract flag
                        flags &= ~HF; // reset half carry flag
                        if(''' + CurReg + ''' & 0x80){
                            flags |= CF; // set carry flag
                        }
                        else{
                            flags &= ~CF; // reset carry flag
                        }
                        pc += 2;
                        break;
            '''

        if (CurOpcode == "SRA"):
            s = ''' 
            machineCycles = 2;
            uint8_t result =''' + CurReg + '''>> 1 | ''' + CurReg + ''' & 0x80;
            ''' + CurReg + ''' = result;
            if(result == 0){
                            flags |= ZF; // set zero flag
                        }
                        else{
                            flags &= ~ZF; // reset zero flag
                        }
                        flags &= ~NF; // reset subtract flag
                        flags &= ~HF; // reset half carry flag
                        if(''' + CurReg + ''' & 0x01){
                            flags |= CF; // set carry flag
                        }
                        else{
                            flags &= ~CF; // reset carry flag
                        }
                        pc += 2;
                        break;
            '''

        if (CurOpcode == "SWAP"):
            s = ''' 
            machineCycles = 2;
            uint8_t result =(''' + CurReg + ''' & 0x0F) << 4 | ''' + CurReg + ''' >> 4;
            ''' + CurReg + ''' = result;
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
            '''
        
        if (CurOpcode == "SRL"):
            s = ''' 
            machineCycles = 2;
            uint8_t result =''' + CurReg + '''>> 1;
            ''' + CurReg + ''' = result;
            if(result == 0){
                            flags |= ZF; // set zero flag
                        }
                        else{
                            flags &= ~ZF; // reset zero flag
                        }
                        flags &= ~NF; // reset subtract flag
                        flags &= ~HF; // reset half carry flag
                        if(''' + CurReg + ''' & 0x01){
                            flags |= CF; // set carry flag
                        }
                        else{
                            flags &= ~CF; // reset carry flag
                        }
                        pc += 2;
                        break;
            '''
      
    else:
        s = "//idk"
            

    return s

def getReg(Opcode): # B=0, C=1 ... A=7
    if ((Opcode & 0x0F) > 8):
        O = (Opcode & 0x0F) - 8
    else:
        O = Opcode & 0x0F


    if O == 0:
        return "B"
    elif O == 1:
        return "C"
    elif O == 2:
        return "D"
    elif O == 3:
        return "E"
    elif O == 4:
        return "H"
    elif O == 5:
        return "L"
    elif O == 6:
        return "memory[H<<8|L]"
    elif O == 7:
        return "A"
    else:
        print("Error: getReg")
        print(hex(Opcode))
        return "FAIL"
        
def getOperation(Opcode):
    if (Opcode <= 0x7):
        return "RLC"
    elif (Opcode <= 0xF):
        return "RRC"
    elif (Opcode <= 0x17):
        return "RL"
    elif (Opcode <= 0x1F):
        return "RR"
    elif (Opcode <= 0x27):
        return "SLA"
    elif (Opcode <= 0x2F):
        return "SRA"
    elif (Opcode <= 0x37):
        return "SWAP"
    elif (Opcode <= 0x3F):
        return "SRL"
    elif (Opcode <= 0x47):
        return "BIT"
    elif (Opcode <= 0x4F):
        return "RES"
    elif (Opcode <= 0x57):
        return "SET"
    #TODO: Add the rest of the opcodes

    else:
        return "N/A"
      
for op in range(0, 0x100):
    print(printStartOpcode(op) + printMidOpcode(op) + End)
