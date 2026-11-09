# CHIP-8-ENGINE

A high-performance, flexible, zero-dependency, single-header (`stb`-style) CHIP-8 engine written in pure C. Designed from the ground up for both desktop environments and bare-metal embedded systems (such as ARM Cortex-M and RISC-V microcontrollers).

---

## Features

* **Header-Only (`stb`-style):** Easy integration into any project with a single `#include`.
* **Zero Dependencies:** Completely independent of external runtime libraries.
* **Dual Architecture Support:** Toggle between legacy (Cosmac VIP) and modern CHIP-8 instruction behaviors via compile-time flags.
* **Flexible Display Pipelines:** 
  * Default 32-bit RGBA / ARGB framebuffers.
  * **1BPP Mode (`CHIP8_1BPP`):** Ultra-lightweight 256-byte VRAM layout designed for monochrome embedded displays.
* **Embedded-Optimized:** 
  * `CHIP8_FORCE_ALIGNEMENT` for strict memory alignment architectures (ARM, RISC-V).
  * `CHIP8_NO_DIVISION` path for processors lacking hardware division support.

---

## Download

* **To download the header file directly:**
```bash
wget https://raw.githubusercontent.com/MedRayenHaouari/CHIP-8-ENGINE/main/core/chip8.h
```

* **To clone the whole repo:**
```bash
git clone https://github.com/MedRayenHaouari/CHIP-8-ENGINE.git
```

---

## Compilation Flags

NOTE: without any flag, you'll end up with the default modern chip8 without function definition, without granted alignement and RGBA8888 VRAM.

| Flag | Description |
| :--- | :--- |
| `#define CHIP8_LEGACY` | Emulates original COSMAC VIP behavior. |
| `#define CHIP8_IMPLEMENTATION` | Compiles the function definitions. |
| `#define CHIP8_1BPP` | Shrinks VRAM down to 256 bytes for 1-bit-per-pixel monochrome rendering. |
| `#define CHIP8_ARGB` | Configures 32-bit color output to ARGB8888 order instead of the default RGBA. |
| `#define CHIP8_FORCE_ALIGNEMENT` | Aligns internal buffers for strict architectures. |
| `#define CHIP8_NO_DIVISION` | Uses subtraction loops instead of division instructions for architectures with no division. |
