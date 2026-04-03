# ST-Tactical

**ST-Tactical** is a modular tactical turn-based roguelike engine, inspired by the classic Super Star Trek, currently in development.

## 🚀 Current Milestone: Card 0.0 (Zenit-Video Pack)
At this stage, the project is building its "Virtual Hardware", effectively turning the viewport into a fully programmable, double-buffered visual text terminal. It removes any modern graphic capabilities from the game loop and imposes a rigid text-based grid (e.g., 80x25, 132x42) with strict palette rendering, blink, bold, and italic characteristics simulating a high-end modern terminal. 

The terminal UI (Dashboard) accurately renders a complex grid of CP437/Unicode Box-Drawing blocks with pixel-perfect precision via Raylib backend (with custom gapless rendering for both single and double box-drawing blocks), hosting the logic for LRS (Long Range Scan, dynamic 3x3 relative view), SRS (Short Range Scan, implemented via a floating 8x8 camera math over a continuous linear global 96x96 sector grid), and basic Galaxy Generation mechanics (Deterministic LCG, Bestiary, and Quadrant distributions).

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
- **Arrows**: Move the local camera cursor.
- **Type commands directly in the prompt**: (Press Enter to execute)
  - `SRS`: Short Range Scan
  - `LRS`: Long Range Scan
  - `MAP`: Galaxy Map / Chart
  - `IMP <X> <Y>`: Impulse engines to (X, Y) coordinates. Uses energy.
  - `WRP <X> <Y> <FATOR>`: Warp engine to (X, Y) at given warp factor [1-8]. Uses energy.
  - `SHI <AMOUNT>`: Transfer energy between shields and main reserves (use negative values to drain shields).
  - `HELP`: Display available commands.
  - `QUIT`: Exit game.

## 🗂️ Architecture
* `/include/video/terminal.h`: The API for the VRAM and Virtual Monitor.
* `/src/video/terminal.c`: Implementation using Raylib to enforce the grid rules and draw cycles.
* `/src/game/dashboard.c`: The procedural terminal layout mapping for the tactical dashboard and scans.
* `/src/game/galaxy.c`: Core mechanics for deterministic Universe generation.
* `/src/video/test_video.c`: Current entry-point validating the hardware.
