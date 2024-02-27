

print("hello world")


Start = "case 0xCB: {\n"

End = "}"

def getStartOpcode(Opcode, Reg):
    s = "case "
    return s


def getReg(Opcode): # B=0, C=1 ... A=7
    if (Opcode > 8):
        O = Opcode - 8
    else:
        O = Opcode

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
        return "(H<<8|L)"
    elif O == 7:
        return "A"
    else:
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
    else:
        return "N/A"
      
