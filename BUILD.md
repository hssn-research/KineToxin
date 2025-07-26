# KineToxin Build Configuration

## Prerequisites
- Windows 10/11 (x64)
- Visual Studio 2022 (Community or Professional)
- Windows SDK (latest version)
- CMake 3.16 or higher
- Git for version control

## Build Instructions

### 1. Environment Setup
```cmd
# Open Developer Command Prompt for VS 2022
# Navigate to project directory
cd /path/to/KineToxin
```

### 2. Configure Build
```cmd
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -G "Visual Studio 17 2022" -A x64
```

### 3. Build Project
```cmd
# Build all targets
cmake --build . --config Release

# Or build specific target
cmake --build . --target KineToxinUtils --config Release
```

### 4. Development Build (Debug)
```cmd
# Configure for debug
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Debug

# Build debug version
cmake --build . --config Debug
```

## Project Structure
```
KineToxin/
├── src/                    # Source code
│   ├── core/              # Core functionality
│   │   ├── surveillance/  # Location, camera, audio modules
│   │   └── communication/ # Message interception
│   ├── evasion/           # Anti-analysis modules
│   ├── persistence/       # Persistence mechanisms
│   └── utils/             # Utility functions
├── tests/                 # Unit and integration tests
├── docs/                  # Documentation
├── build/                 # Build artifacts (created)
└── CMakeLists.txt         # Build configuration
```

## Development Workflow

### Adding New Modules
1. Create header (.h) and source (.cpp) files
2. Add files to appropriate CMakeLists.txt section
3. Update include directories if needed
4. Rebuild project

### Testing
```cmd
# Build and run tests (when implemented)
cmake --build . --target tests --config Debug
ctest -C Debug
```

### Code Style
- Follow Microsoft C++ coding standards
- Use meaningful variable and function names
- Include comprehensive documentation
- Implement proper error handling

## Dependencies
- Windows API (Kernel32, User32, Advapi32)
- Windows Crypto API (Crypt32)
- Location API (LocationAPI, SensorsAPI)
- COM/OLE libraries for system interaction

## Debugging
- Use Visual Studio debugger for development
- Enable debug symbols in Debug builds
- Use Windows Event Tracing (ETW) for advanced debugging
- Process Monitor for system call tracing

## Security Notes
- All builds include anti-debugging checks
- Release builds are obfuscated
- Debug builds include additional logging
- Test certificates used for code signing in development

## Troubleshooting

### Common Issues
1. **Missing Windows SDK**: Install latest Windows SDK
2. **CMake not found**: Add CMake to system PATH
3. **MSVC not detected**: Use Developer Command Prompt
4. **Link errors**: Verify all required libraries are available

### Build Errors
- Check CMake output for missing dependencies
- Verify Windows SDK version compatibility
- Ensure all header files are properly included
- Check for circular dependencies between modules

## Release Process
1. Update version in CMakeLists.txt
2. Build release configuration
3. Run full test suite
4. Apply code obfuscation
5. Generate documentation
6. Create release package with CPack
