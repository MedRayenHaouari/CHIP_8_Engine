/* ########## MAIN PROGRAM ##########
**
** NOTES:
** - This program is meant essentially for testing my core.h logic
** - Make sure you have raylib installed in your PC and added to your library folder
*/

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#define CHIP8_IMPLEMENTATION
#include "../core/chip8.h"
#include "simulate.c"
#include "../include/my_colors.h"
#include "../include/my_symbols.h"
#include "../include/my_types.h"

#define SCALE 20

int main(int argc, char **argv){
  if (argc == 1){
    printf("You need to specify the ROM path to run like this:\n>>> %s <ROM Path>\n", argv[0]);
    return 0;
  }
  else if(argc == 2){
    printf("Starting the Emulator...\n");
    
    // initialization phase
    static chip8 chip;
    u8 random = (rand() & 0xFF);
    if (chip8_init(&chip, random) < 0){
      fprintf(stderr, "[" COLOR_RED SYM_CROSS COLOR_RESET "] Error While Initializing the Chip\n");
      return -1;
    }
    printf("[" COLOR_GREEN SYM_CHECK COLOR_RESET "] Chip Initialized\n");

    // loading the rom (Fixed: Cast to u8* to match chip8_string8_t definition)
    u8 *buffer = (u8*)calloc(3584, sizeof(u8));
    if (!buffer){
      fprintf(stderr, "[" COLOR_RED SYM_CROSS COLOR_RESET "] Error While Allocation Buffer\n");
      return -1;
    }
    
    chip8_string8_t rom;
    FILE *f = fopen(argv[1], "rb");
    if (!f){    
      fprintf(stderr, "[" COLOR_RED SYM_CROSS COLOR_RESET "] Error While Opening File\n");
      free(buffer);
      return -1;
    }
    
    size_t rom_size = fread(buffer, 1, 3584, f);
    fclose(f);
    
    rom.size = rom_size;
    rom.data = buffer;
    printf("[" COLOR_GREEN SYM_CHECK COLOR_RESET "] ROM Read Successfully\n");
    
    if (chip8_load_rom(&chip, rom) < 0){
      fprintf(stderr, "[" COLOR_RED SYM_CROSS COLOR_RESET "] Error While Loading ROM to the Chip\n");
      free(buffer);
      return -1;
    }
    printf("[" COLOR_GREEN SYM_CHECK COLOR_RESET "] ROM Loaded to Chip Successfully\n");

    // running the chip
    if (simulate(&chip, SCALE) < 0){
      fprintf(stderr, "[" COLOR_RED SYM_CROSS COLOR_RESET "] Error During Simulation\n");
      free(buffer);
      return -1;
    }

    free(buffer);
    printf("Finishing Execution\n");
    return 0;
  } else {
    fprintf(stderr, "ERROR: Argument parser is panicking rn\n");
    return -1;
  }
}
