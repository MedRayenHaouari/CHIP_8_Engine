#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../include/my_colors.h"
#include "../include/my_symbols.h"
#include "../include/my_types.h"
#define CHIP8_IMPLEMENTATION
#include "../core/chip8.h"

int main(int argc, char **argv){
  (void)argc;
  static chip8 chip;
  chip8_init(&chip, 11);
  
  // Fixed: Match u8 array type
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
  
  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);
  for (uint64_t i = 0; i < 5000000; i++) {
      chip8_run(&chip);
  }
  clock_gettime(CLOCK_MONOTONIC, &end);
  double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
  printf("USUAL TIME SPENT: %f seconds\n", elapsed);
  
  chip8_init(&chip, 11);
  u8 benchmark_rom[] = {
      0x60, 0x00, // V0 = 0 (X coordinate)
      0x61, 0x00, // V1 = 0 (Y coordinate)
      0xA0, 0x00, // I  = 0x000 (Point to sprite data in font RAM)
      0xD0, 0x1F, // D01F -> Draw a 15-byte sprite at (V0, V1)
      0x12, 0x06  // Jump back to 0x206 (repeat D01F forever)
  };
  
  // Fixed: Directly assign the u8 array to the struct
  rom.data = benchmark_rom; 
  rom.size = sizeof(benchmark_rom);
  
  chip8_load_rom(&chip, rom);
  clock_gettime(CLOCK_MONOTONIC, &start);
  for (uint64_t i = 0; i < 5000000; i++) {
      chip8_run(&chip);
  }
  clock_gettime(CLOCK_MONOTONIC, &end);
  elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
  printf("DRAW TIME SPENT: %f seconds\n", elapsed);
  
  return 0;
}
