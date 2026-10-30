#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include "../include/my_types.h"
#include "../include/my_colors.h"
#include "../include/my_symbols.h"
#include "../core/chip8.h"

#define TARGET_FPS 60
#define CPU_FREQ 700
#define INST_PER_FRAME (CPU_FREQ/TARGET_FPS)

void update_key_inputs(chip8 *chip) {
    chip->key = 0;
    if (IsKeyDown(KEY_ONE))   chip->key |= (1 << 0x1);
    if (IsKeyDown(KEY_TWO))   chip->key |= (1 << 0x2);
    if (IsKeyDown(KEY_THREE)) chip->key |= (1 << 0x3);
    if (IsKeyDown(KEY_FOUR))  chip->key |= (1 << 0x4);
    if (IsKeyDown(KEY_FIVE))  chip->key |= (1 << 0x5);
    if (IsKeyDown(KEY_SIX))   chip->key |= (1 << 0x6);
    if (IsKeyDown(KEY_SEVEN)) chip->key |= (1 << 0x7);
    if (IsKeyDown(KEY_EIGHT)) chip->key |= (1 << 0x8);
    if (IsKeyDown(KEY_NINE))  chip->key |= (1 << 0x9);
    if (IsKeyDown(KEY_A))     chip->key |= (1 << 0xA);
    if (IsKeyDown(KEY_B))     chip->key |= (1 << 0xB);
    if (IsKeyDown(KEY_C))     chip->key |= (1 << 0xC);
    if (IsKeyDown(KEY_D))     chip->key |= (1 << 0xD);
    if (IsKeyDown(KEY_E))     chip->key |= (1 << 0xE);
    if (IsKeyDown(KEY_F))     chip->key |= (1 << 0xF);
    if (IsKeyDown(KEY_ZERO))  chip->key |= (1 << 0x0);
}

int simulate(chip8 *chip, int scale){
  SetTraceLogLevel(LOG_NONE);
  InitWindow(64*scale, 32*scale, "CHIP-8 EMULATOR");
  SetTargetFPS(TARGET_FPS);
  
  chip8_string8_t screen_pixels;
  screen_pixels.size = 1024*8;
  // Fixed: Valid pointer casting
  screen_pixels.data = (u8*)malloc(screen_pixels.size * sizeof(u8)); 
  
  Image img = {
    .data = screen_pixels.data, // Fixed: Raylib requires the raw pointer, not the struct wrapper
    .width = 64,
    .height = 32,
    .mipmaps = 1,
    .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
  };
  
  Texture2D screen_texture = LoadTextureFromImage(img);
  SetTextureFilter(screen_texture, TEXTURE_FILTER_POINT);
  
  int i;
  while (!WindowShouldClose()){
    update_key_inputs(chip);
    
    for (i=0; i<INST_PER_FRAME; i++){
      if (chip8_run(chip) < 0){
        perror("[" COLOR_RED SYM_CROSS COLOR_RESET "] Error While Running the Chip\n");
        return -1;
      }
    }
    
    // Fixed: Pass the struct by reference
    chip8_copy_vram(chip, &screen_pixels);
    
    // Fixed: Pass the raw data buffer pointer to Raylib
    UpdateTexture(screen_texture, screen_pixels.data);
    
    BeginDrawing();
    ClearBackground(BLACK);
    DrawTextureEx(screen_texture, (Vector2){0, 0}, 0.0f, scale, WHITE);
    EndDrawing();
    
    if (chip->dt > 0) chip->dt--;
    if (chip->st > 0) chip->st--;
  }
  
  UnloadTexture(screen_texture);
  CloseWindow();
  free(screen_pixels.data);
  return 0;
}
