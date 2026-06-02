# GDPHYSX Phase 1 - Windows Build Guide

## Prerequisites

- Visual Studio 2019 or 2022 (with C++ workload)
- CMake 3.15 or newer
- Git

## Option 1: Automatic Build via GitHub Actions (Easiest)

The project includes a GitHub Actions workflow that automatically builds the Windows executable.

1. Push this code to your GitHub repository
2. Go to "Actions" tab
3. The "Build Windows Executable" workflow will run automatically
4. Download the compiled `GDPHYSX_Phase1.exe` from the artifact

## Option 2: Manual Build with vcpkg

### Step 1: Install vcpkg
```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
cd ..
```

### Step 2: Install Dependencies
```bash
vcpkg\vcpkg install glfw3:x64-windows glew:x64-windows glm:x64-windows opengl:x64-windows
```

### Step 3: Configure and Build
```bash
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=..\vcpkg\scripts\buildsystems\vcpkg.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
cd ..
```

### Step 4: Run
```bash
build\Release\GDPHYSX_Phase1.exe
```

The executable expects shader files (`.vert` and `.frag`) in the same directory.

## Troubleshooting

**Error: "Could not find GLFW3"**
- Ensure vcpkg installed GLFW3 successfully
- Check that the toolchain file path is correct

**Error: "Could not find GLEW"**
- Run: `vcpkg\vcpkg install glew:x64-windows` again
- Verify x64-windows triplet is being used

**Executable won't run**
- Ensure all shader files are in the same directory as the .exe
- Check that you're using Windows 10/11 with OpenGL 3.3+ support

## Project Structure

```
GDPHYSX_Phase1_v1.0.0/
├── src/                     # Source code
├── include/                 # Header files
├── shaders/                 # OpenGL shader files
├── CMakeLists.txt          # Build configuration
└── .github/workflows/       # GitHub Actions
```
