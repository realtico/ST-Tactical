# ST-Tactical

**ST-Tactical** is a modular tactical turn-based engine, currently in development.

## 🚀 Current Milestone: Card 0.0 (Zenit-Video Pack)
At this stage, the project is building its "Virtual Hardware", effectively turning the viewport into a fully programmable, double-buffered visual text terminal. It removes any modern graphic capabilities from the game loop and imposes a rigid text-based grid (e.g., 80x25, 132x42) with strict palette rendering, blink, bold, and italic characteristics simulating a high-end modern terminal. 

## 🛠️ Build System

The project uses **CMake** to support a dual-environment. You can develop in Linux (GCC) or native Windows (MSVC) natively with minimal setup.

### Dependencies
- **CMake** (3.16+)
- **C99 Compiler** (GCC on Linux, MSVC on Windows)
- **Wayland / X11 dev headers** (Only if on Linux)

*Note: You do **not** need to install Raylib manually on your system. The CMake script uses `FetchContent` to download and statically link Raylib 5.0 at build-time.*

### Building on Linux / WSL / GCC
```bash
# Clone the repository
git clone <your-repo-url>
cd ST-Tactical

# Configure the build directory
mkdir build
cd build
cmake ..

# Compile the project
cmake --build . -j$(nproc)
```

### Building on Windows (Visual Studio / MSVC)
Open the folder in **VS Code**, and standard CMake Tools will configure it automatically. 
Or, if you use the Developer Command Prompt:
```cmd
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Running the App
Once compiled, you can run the test environment:
```bash
./build/ST_Tactical
```
*(On Windows, the executable will be `build\Release\ST_Tactical.exe`)*

## 🕹️ Controls (Test Environment)
- **Arrows**: Move the testing cursor.
- **1, 2, 3**: Switch between resolutions (80x25, 100x32, 132x42).

## 🗂️ Architecture
* `/include/video/terminal.h`: The API for the VRAM and Virtual Monitor.
* `/src/video/terminal.c`: Implementation using Raylib to enforce the grid rules and draw cycles.
* `/src/video/test_video.c`: Current entry-point validating the hardware.
