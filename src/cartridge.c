#include "core.h"
#include "cartridge.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void loadCartridge(char* filename){
    FILE* file = fopen(filename, "rb");
    if (file == NULL){
        printf("could not open rom %s\n", filename);
        exit(1);
    }
    
    //location 0x147 in the cartridge data contains the cartridge type

    fseek(file, 0x147, SEEK_SET);
    uint8_t cartridgeType;
    fread(&cartridgeType, sizeof(cartridgeType), 1, file);
    printf("value at position 147: 0x%X\n", cartridgeType);

    switch (cartridgeType) {
        
        case ROM:{ //If only ROM, all data is loaded directly to memory, up to 32KB (0x8000)

            printf("Cartridge type is: ROM only\n");

             fseek(file, 0, SEEK_END);
             long fileSize = ftell(file);

             printf("File size: %ld\n", fileSize);

             rewind(file);
             fread(memory, 1, fileSize, file);
             fclose(file);

            break;

        }
        case MBC1:{

            printf("Cartridge type is: MBC1\n");
        
            break;

        }
        // Add cases for other cartridge types if needed
        default:
            // Handle unknown cartridge types
            break;
    }


    
}