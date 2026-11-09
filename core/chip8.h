/*
** ############### CHIP 8 ENGINE ###############
**
** IMPORTANT:
** - When dealing with buffer use the chip8_string8_t type defined below!!!
** - The loaded ROM must be less than 3500 mB.
**
** NOTE: One of these must be sepcified
** - CHIP8_LEGACY flag is for emulating the COSMAC VIP
** - CHIP8_MODERN flag is for emulating the modern CHIP 8 
**
** - CHIP8_IMPLEMENTATION flag to get the functions implementions 
** - CHIP8_FORCE_ALIGNEMENT can be used to force array's alignement (useful for ARM and RISCV arch) and therefore the chip8
**   struct must be allocated in the heap to avoid bugs.
** - CHIP8_1BPP flag can be used to obtain a 1bpp vram maybe useful when using a monochronous screen in an embedded project
**   who knows!!
** - CHIP8_ARGB flag can be used to get an ARGB vram values in the default 4 bytes per pixel mode (if not specified
**   the vram will have RGBA kind of values)
** - CHIP8_NO_DIVISION flag used when you're using an arch that don't support one
*/



#pragma once

// these header files can be omitted if you want other definitions for the variables and types below
#include <assert.h> // for implementing assertions (useful for testing)
#include <stddef.h> // to define NULL and size_t
#include <stdint.h> // to define uint8_t and so on

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef uint64_t u64;
typedef int64_t i64;


#if (!defined(CHIP8_LEGACY) && !defined(CHIP8_MODERN))
#  error "You have to specify whether you wanna run legacy or modern chip 8 engine, one of them"
#elif (defined(CHIP8_LEGACY) && defined(CHIP8_MODERN))
#  error "You have to specify you either want legacy or modern chip 8 engine not both"
#endif

#if CHIP8_FORCE_ALIGNEMENT
#  if defined(_MSC_VER)
#    define CHIP8_ALIGN(n) __declspec(align(n))
#  elif defined(__clang__) || defined(__GNUC__)
#    define CHIP8_ALIGN(n) __attribute__((aligned(n)))
#  endif
#else
#  define CHIP8_ALIGN(n)
#endif

#if defined(__clang__) || defined(__GNU__)
    #define CHIP8_MEMSET(dest, val, size) __builtin_memset(dest, val, size)
#else
#   define CHIP8_MEMSET(dest, val, size)\
    do{\
      int j;\
      for (j=0;j<size;j++){\
        dest[j] = val;\
      }\
    }while(0)
#endif

#define CHIP8_MAX_ROM_SIZE 3500

#if defined(CHIP8_1BPP) // 1 bit per pixel
# define CHIP8_VRAM_SIZE 256
#else
# define CHIP8_VRAM_SIZE 8192
#endif

#define CHIP8_RAM_SIZE 4096


typedef struct{
    u8 ram[CHIP8_RAM_SIZE] CHIP8_ALIGN(sizeof(size_t)); // RAM slot
    u8 vram[CHIP8_VRAM_SIZE] CHIP8_ALIGN(sizeof(size_t)); // VRAM slot
    u8 v[16] CHIP8_ALIGN(sizeof(size_t));  // general purpose Vx registers
    u16 key;  // keyboard keys with LSB at index 0
    u16 i;  // the 16bit I register
    u16 pc;  // program counter
    u16 stack[16];  // stack
    u8 rand;  // random number
    u8 st;  // sound timer
    u8 dt;  // delay timer
    i8 sp;  // stack pointer
    u8 waiting;
    u8 pressed_key;
} chip8;

// string8 structure
typedef struct{
  u8 *data;
  size_t size;
} chip8_string8_t;


int chip8_init(chip8 *chip,u8 rand);
int chip8_load_rom(chip8 *chip,chip8_string8_t rom);
int chip8_run(chip8 *chip);
int chip8_copy_vram(chip8 *chip,chip8_string8_t *pixel_buffer);
int chip8_unpack_1bpp_vram(chip8 *chip,chip8_string8_t *pixel_buffer);


#if defined(CHIP8_IMPLEMENTATION)
int chip8_init(chip8 *chip,u8 rand){
  if (chip == NULL || rand == 0) return -1;
  
  // initializing the RAM
  i16 i;
  u8 *chunck = chip->ram;
  CHIP8_MEMSET(chunck,0,CHIP8_RAM_SIZE);
  
  // initializing the VRAM
  #if defined(CHIP8_1BPP)
  chunck = chip->vram;
  CHIP8_MEMSET(chunck,0,CHIP8_VRAM_SIZE);
  #else
  #if defined(CHIP8_ARGB)
  chunck = chip->vram;
  CHIP8_MEMSET(chunck,0,CHIP8_VRAM_SIZE);
  for (i=0;i<CHIP8_VRAM_SIZE;i=i+4){
      chip->vram[i] = 0xFF;
  }
  #else
  chunck = chip->vram;
  CHIP8_MEMSET(chunck,0,CHIP8_VRAM_SIZE);
  for (i=3;i<CHIP8_VRAM_SIZE;i=i+4){
      chip->vram[i] = 0xFF;
  }
  #endif
  #endif
  
  // writing 0 to F sprites to RAM starting from @ 0
  // this array definition is AI generated
  static u8 chip8_sprites[80] = {
      0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
      0x20, 0x60, 0x20, 0x20, 0x70, // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
      0x90, 0x90, 0xF0, 0x10, 0x10, // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
      0xF0, 0x10, 0x20, 0x40, 0x40, // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90, // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
      0xF0, 0x80, 0x80, 0x80, 0xF0, // C
      0xE0, 0x90, 0x90, 0x90, 0xE0, // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
      0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };
  for (i=0;i<80;i++){
    chip->ram[i] = chip8_sprites[i];
  }
  
  // initializing the Registers
  CHIP8_MEMSET(chip->v,0,16);
  
  // initializing other variables
  chip->pc = 0x200;
  chip->sp = -1;
  chip->rand = rand;
  chip->dt = 0;
  chip->st = 0;
  chip->pressed_key = 0;
  chip->waiting = 0;
  return 0;
}


// load ROM data to the chip's RAM starting from offset 0x200
int chip8_load_rom(chip8 *chip,chip8_string8_t rom){
  if (chip == NULL || rom.size > CHIP8_MAX_ROM_SIZE) return -1;
  size_t i;
  for (i=0;i<rom.size;i++){
    chip->ram[i+0x200] = rom.data[i];
  }
  return 0;
}


// a pseudo u8 number generator from the chip random variable
static inline u8 chip8_random_byte(chip8 *chip){
  u8 seed = chip->rand;
  if (seed == 0) seed = (u8)0xAD; // some random ahh number
  int x = seed ^ chip->ram[seed + 0x200]; // & for modulo so that it never exceds RAM_SIZE and be above 0x200
  x ^= x << 13;
  x ^= chip->v[seed & 15];
  x ^= x >> 17;
  x ^= chip->vram[seed];
  x ^= x << 5;
  chip->rand = x;
  return (u8)(x & 0xFF);
}


// count tailing zero in a u16 integer
static inline u8 chip8_ctz16(u16 x) {
    u8 i = 0;
    while ((x & 1) == 0 && i<16) {
        x >>= 1;
        i++;
    }
    return i;
}


// the main execution loop
int chip8_run(chip8 *chip){
  if (chip == NULL) return -1;

  // fetching instruction
  u16 inst = chip->ram[chip->pc] << 8 | chip->ram[chip->pc+1];

  // decoding and executing
  // assuming the 16bit instruction is in this format "abcd" where a containing most significant 4 bits
  u8 a = inst >> 12;
  u8 b = (inst & 0x0F00) >> 8;
  u8 c = (inst & 0x00F0) >> 4;
  u8 d = inst & 0x000F;
  chip->pc += 2;

// this switch is AI generated
  switch (a){
    case 0: goto label_0;
    case 1: goto label_1;
    case 2: goto label_2;
    case 3: goto label_3;
    case 4: goto label_4;
    case 5: goto label_5;
    case 6: goto label_6;
    case 7: goto label_7;
    case 8: goto label_8;
    case 9: goto label_9;
    case 0xA: goto label_A;
    case 0xB: goto label_B;
    case 0xC: goto label_C;
    case 0xD: goto label_D;
    case 0xE: goto label_E;
    case 0xF: goto label_F;
    default: return -1;
  }

  label_0:{
    if (b == 0 && c == 0xE && d == 0){
      int i;
      u8 *chunck = chip->vram;
      CHIP8_MEMSET(chunck,0,CHIP8_VRAM_SIZE);
    }
    else if (b == 0 && c == 0xE && d == 0xE){
      if (chip->sp <= -1) return -1;
      chip->pc = chip->stack[(chip->sp)--];
    }
    return 0;
  }

  label_1:{
    chip->pc = (b << 8) | (c << 4) | (d);
    return 0;
  }

  label_2:{
    if (chip->sp >= 15) return -1;
    chip->stack[++(chip->sp)] = chip->pc;
    goto label_1;
    return 0;
  }

  label_3:{
    u8 value = (c << 4) | d;
    chip->pc += 2*(chip->v[b] == value);
    return 0;
  }

  label_4:{
    u8 value = (c << 4) | d;
    chip->pc += 2*(chip->v[b] != value);
    return 0;
  }

  label_5:{
    if (d != 0) return -1;
    chip->pc += 2*(chip->v[b] == chip->v[c]);
    return 0;
  }

  label_6:{
    chip->v[b] = (c << 4) | d;
    return 0;
  }

  label_7:{
    chip->v[b] += (c << 4) | d;
    return 0;
  }

  label_8:{
    u16 vx = chip->v[b];
    u16 vy = chip->v[c];
    u8 flag;
    u8 flag_set = 0;
    switch (d){
      case 0:
        vx = vy;
        break;
      case 1:
        vx |= vy;
        #if defined(CHIP8_LEGACY)
        flag_set = 1;
        flag = 0;
        #endif
        break;
      case 2:
        vx &= vy;
        #if defined(CHIP8_LEGACY)
        flag_set = 1;
        flag = 0;
        #endif
        break;
      case 3:
        vx ^= vy;
        #if defined(CHIP8_LEGACY)
        flag_set = 1;
        flag = 0;
        #endif
        break;
      case 4:
        vx += vy;
        flag = vx >> 8;
        flag_set = 1;
        vx &= 0xFF;
        break;
      case 5:
        flag = (vx >= vy);
        flag_set = 1;
        vx = (vx - vy) & 0xFF;
        break; 
      case 6:
        #if defined(CHIP8_LEGACY)
        vx=vy;
        #endif
        flag_set = 1;
        flag = vx & 0x01;
        vx = vx >> 1;
        break;
      case 7:
        flag = (vy >= vx);
        flag_set = 1;
        vx = (vy - vx) & 0xFF;
        break;
      case 0xE:
        #if defined(CHIP8_LEGACY)
        vx = vy;
        #endif
        flag = vx >> 7;
        flag_set = 1;
        vx = (vx << 1) & 0xFF;
        break;
      default: return -1;
    }
    chip->v[b] = (u8)vx;
    chip->v[0xF] = flag * flag_set;
    return 0;
  }

  label_9:{
    if (d != 0) return -1;
    chip->pc += 2*(chip->v[b] != chip->v[c]);
    return 0;
  }

  label_A:{
    chip->i = (b << 8) | (c << 4) | d;
    return 0;
  }

  label_B:{
    #if defined(CHIP8_LEGACY)
    chip->pc = ((b << 8) | (c << 4) | d) + chip->v[0];
    #elif defined(CHIP8_MODERN)
    chip->pc = ((b << 8) | (c << 4) | d) + chip->v[b];
    #endif
    return 0;
  }

  label_C:{
    u8 random = chip8_random_byte(chip);
    chip->v[b] = random & ((c << 4) | d);
    return 0;
  }

  #if defined(CHIP8_1BPP)
  label_D:{
    u8 collision = 0;
    int i = chip->i;
    u8 vx = chip->v[b];
    u8 vy = chip->v[c];
    u8 n = d;
    u8 shift = vx%8;
    u8 byte,slice1,slice2;
    for (;i<chip->i+n;i++){
      byte = chip->ram[i];
      slice1 = byte >> shift;
      slice2 = (shift > 0 ? byte << (8 - shift) : 0);
      collision |= chip->vram[(vx/8)%8+vy*8] & slice1;
      chip->vram[(vx/8)%8+vy*8] ^= slice1;
      collision |= chip->vram[(vx/8+1)%8+vy*8] & slice2;
      chip->vram[(vx/8+1)%8+vy*8] ^= slice2;
      vy = (vy+1)%32;
    }
    chip->v[0xF] = (collision > 0);
    return 0;
  }
  #else
  label_D:{
    u8 word,counter,byte;
    u8 collision = 0;
    int i = chip->i;
    u8 vx = (chip->v[b])%64;
    u8 vy = (chip->v[c])%32;
    u8 n = d;
    for (;i<chip->i + n; i++){
      byte = chip->ram[i];
      for (counter = 0;counter<8;counter++){
        #if defined(CHIP8_ARGB)
        word = ((byte >> (7-counter))&1)*0xFF;
        collision |= chip->vram[((vx+counter)%64)*4 + vy*256 + 1] & word;
        chip->vram[((vx+counter)%64)*4 + vy*256 + 1] ^= word;
        chip->vram[((vx+counter)%64)*4 + vy*256 + 2] ^= word;
        chip->vram[((vx+counter)%64)*4 + vy*256 + 3] ^= word;
        #else
        word = ((byte >> (7-counter))&1)*0xFF;
        collision |= chip->vram[((vx+counter)%64)*4 + vy*256 + 1] & word;
        chip->vram[((vx+counter)%64)*4 + vy*256 + 0] ^= word;
        chip->vram[((vx+counter)%64)*4 + vy*256 + 1] ^= word;
        chip->vram[((vx+counter)%64)*4 + vy*256 + 2] ^= word;
        #endif
      }
      vy = (vy+1)%32;
    } 
    return 0;
  }
  #endif

  label_E:{
    if (c == 9 && d == 0xE){
      chip->pc += 2*(((chip->key) & (1 << chip->v[b])) > 0);
      return 0;
    }else if (c == 0xA && d == 1){
      chip->pc += 2*(((chip->key) & (1 << chip->v[b])) == 0);
      return 0;
    }else{
      return -1;
    }
  }
    
  label_F:{
    int a;  
    u8 x = (c << 4) | d;
    switch (x){
      case 0x07:
        chip->v[b]=chip->dt;
        break;
      case 0x0A:
        if (!(chip->waiting)) {
          if (chip->key != 0) {
            chip->pressed_key = chip8_ctz16(chip->key);
            chip->waiting = 1;
          }
          chip->pc -= 2; // Keep blocking execution
        } else {
          // Wait for the key to be released
          if (chip->key == 0) {
            chip->v[b] = chip->pressed_key;
            chip->waiting = 0;
            // Do not subtract pc; let the chip proceed to the next instruction
          } else {
            chip->pc -= 2; // Key still held down, keep blocking
          }
        }
        break;
      case 0x15:
        chip->dt = chip->v[b];
        break;
      case 0x18:
        chip->st = chip->v[b];
        break;
      case 0x1E:
        chip->i += chip->v[b];
        break;
      case 0x29:
        chip->i = chip->v[b]*5;
        break;
      case 0x33:
        a = chip->i;
        int v = chip->v[b];
        #if defined(CHIP8_NO_DIVISION)
        // for processors that don't support division
        u8 hundreds = 0, tens = 0;
        while (v >= 100) { v -= 100; hundreds++; }
        while (v >= 10)  { v -= 10; tens++; }
        chip->ram[a++] = hundreds;
        chip->ram[a++] = tens;
        chip->ram[a] = v;
        #else
        // for processors that support division
        chip->ram[a++] = v/100;
        v %= 100;
        chip->ram[a++] = v/10;
        v %= 10;
        chip->ram[a] = v;
        #endif
        break;
      case 0x55:{
        a = chip->i;
        if (a + b < CHIP8_RAM_SIZE){
          u8 *ram = chip->ram+a;
          u8 *reg = chip->v;
          for (int i=0;i<=b;i++){
            *(ram + i) = *(reg + i);
          }
          #if defined(CHIP8_LEGACY)
          chip->i = a+b+1;
          #endif
        }else return -1;
        break;
      }
      case 0x65:{
        a = chip->i;
        if (a + b < CHIP8_RAM_SIZE){
          u8 *ram = chip->ram+a;
          u8 *reg = chip->v;
          for (int i=0;i<=b;i++){
            *(reg + i) = *(ram + i);
          }
          #if defined(CHIP8_LEGACY)
          chip->i = a+b+1;
          #endif
        }else return -1;
        break;
      }
      default: return -1;
    }
    return 0;
    }
}


#if defined(CHIP8_1BPP)
// unpack the 256B 1bpp vram into u32 pixel buffer
int chip8_unpack_1bpp_vram(chip8 *chip,chip8_string8_t *pixel_buffer){
  if (chip == NULL || pixel_buffer == NULL || pixel_buffer->size < CHIP8_VRAM_SIZE*32 || pixel_buffer->data == NULL) return -1;
  u8 *array = (u8*)pixel_buffer->data;
  u32 i;
  u8 word,counter,byte;
  for (i=0;i<256;i++){
    byte = chip->vram[i];
    for (counter = 0;counter<8;counter++){
      #if defined(CHIP8_ARGB)
      word = ((byte >> (7-counter))&1)*0xFF;
      array[i*32 + 0] = 0xFF;
      array[i*32 + 1] = word;
      array[i*32 + 2] = word;
      array[i*32 + 3] = word;
      #else
      word = ((byte >> (7-counter))&1)*0xFF;
      array[i*32 + 0] = word;
      array[i*32 + 1] = word;
      array[i*32 + 2] = word;
      array[i*32 + 3] = 0xFF;
      #endif
    }
  } 
  return 0;
}
#endif


// copy the containt of vram into pixel_buffer
int chip8_copy_vram(chip8 *chip,chip8_string8_t *pixel_buffer){
  if (chip == NULL || pixel_buffer == NULL || pixel_buffer->size < CHIP8_VRAM_SIZE || pixel_buffer->data == NULL) return -1;
  u8 *buffer = pixel_buffer->data;
  u8 *array = chip->vram;
  int i;
  for (i=0;i<(CHIP8_VRAM_SIZE);i++){
    buffer[i] = array[i];
  }
  return 0;
}

#endif // CHIP8_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#undef CHIP8_ALIGN
#undef CHIP8_MEMSET
