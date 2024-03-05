#include <SDL2/SDL.h>
#include <stdio.h>
#include "core.h"
#include "graphics.h"
#include "cartridge.h"

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("GoatBoy: Load ROM", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH*SCREEN_SCALE, SCREEN_HEIGHT*SCREEN_SCALE, SDL_WINDOW_SHOWN);
    SDL_bool running = SDL_TRUE;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = SDL_FALSE;
                    break;
                    
                case SDL_DROPFILE: {
                    char* dropped_filedir = event.drop.file;
                    printf("Dropped file path: %s\n", dropped_filedir);

                    printf("Loading ROM file...\n");
                    loadCartridge(dropped_filedir); //testing loading a rom file with drag and drop

                    printf("ROM file loaded.\n");
                    initGraphics();

                    //fix this after
                    //TODO: Add step through (maybe with GUI?)
                    u_int32_t lastCycleTime = SDL_GetTicks();
                    u_int32_t cycleDelay = 500; //1000/clockSpeed; // ms per cycle

                    while (1) {
                        u_int32_t currentTime = SDL_GetTicks();
                        if ((currentTime - lastCycleTime >= cycleDelay)) {
                            lastCycleTime = currentTime;

                            if (machineCycles > 0) {
                                machineCycles--;
                            }

                            else {
                            executeCycle();
                            

                            }
                        }
                    }


                    SDL_free(dropped_filedir);  // free dropped_filedir memory block to prevent memory leak
                    break;
                }
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

