#include <SDL2/SDL.h>
#include <stdio.h>
#include "core.h"
#include "graphics.h"
#include "cartridge.h"

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("GoatBoy", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
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


                    SDL_free(dropped_filedir);  // Free dropped_filedir memory block to prevent memory leak
                    break;
                }
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
