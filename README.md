# chip8-c

A CHIP-8 interpreter written in C, using OpenGL and GLFW for cross-platform rendering, window creation, and input handling.

## Installation, Compilation, and Execution

⚠️ **Windows Users:** You need `gcc`, `make`, and other basic GNU tools (like `mkdir` and `rm`) to be able to compile the source files using make.  
Install them easily using [MSYS2](https://www.msys2.org/).

### Steps

```bash
git clone https://github.com/GitHubUser82/chip8-c
cd chip8-c
make PLATFORM
.\bin\chip8_interpreter.exe .\roms\rom-name.ch8
```
- Replace PLATFORM with your target (e.g., windows, linux, or macos) if your Makefile supports platform-specific builds.
- Replace rom-name.ch8 with the actual name of the ROM file you want to run.