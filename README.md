# chip8-c

A CHIP-8 interpreter written in C, using OpenGL and GLFW for cross-platform rendering, window creation, and input handling.

## Installation, compilation, and execution

⚠️ **Windows Users:** You need `gcc`, `make`, and other basic GNU tools (like `mkdir` and `rm`) to be able to compile the source files using `make`.  
Install them easily using [MSYS2](https://www.msys2.org/).

### Steps

```bash
git clone https://github.com/GitHubUser82/chip8-c
cd chip8-c
make PLATFORM
.\bin\EXECUTABLE_NAME .\roms\ROM_NAME
```
- Replace **PLATFORM** with your target OS (windows or linux)
- Replace **EXECUTABLE_NAME** with the name of the generated executable (chip8_interpreter.exe on Windows and chip8_interpreter.out on Linux by default)
- Replace **ROM_NAME** with the name of the CHIP-8 ROM you want to execute