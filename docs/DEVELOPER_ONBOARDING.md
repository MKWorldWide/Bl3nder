# 👨‍💻 Blender Developer Onboarding Guide

## 📋 Welcome to Blender Development!

This guide will help you get started with Blender development, from initial setup to making your first contribution. Whether you're a seasoned developer or new to open source, this guide provides everything you need to become a productive Blender contributor.

## 🎯 Quick Start (1 Hour Setup)

### 📋 Prerequisites Check

Before starting, ensure you have the following:

#### 🖥️ System Requirements
- **Operating System**: Windows 10+, macOS 10.15+, or Linux (Ubuntu 20.04+)
- **RAM**: 8GB minimum, 16GB recommended
- **Storage**: 10GB free space for source code and build
- **CPU**: Multi-core processor (4+ cores recommended)

#### 🔧 Required Software
- **Git**: Version control system
- **CMake**: 3.10 or higher
- **C++ Compiler**: 
  - **Windows**: Visual Studio 2019 or later
  - **macOS**: Xcode 12 or later
  - **Linux**: GCC 11.0+ or Clang 8.0+
- **Python**: 3.10 or higher
- **IDE**: VS Code, CLion, or Visual Studio (recommended)

### 🚀 Step-by-Step Setup

#### 1. Clone the Repository
```bash
# Clone Blender repository
git clone https://projects.blender.org/blender/blender.git
cd blender

# Initialize submodules
git submodule update --init --recursive
```

#### 2. Configure Build Environment
```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ../blender \
  -DCMAKE_BUILD_TYPE=Debug \
  -DWITH_CYCLES=ON \
  -DWITH_OPENVDB=ON
```

#### 3. Build Blender
```bash
# Build (adjust -j flag based on your CPU cores)
make -j$(nproc)  # Linux/macOS
# or
cmake --build . --config Debug --parallel  # Windows
```

#### 4. Test Your Build
```bash
# Run Blender
./bin/blender

# Run tests
make test
```

## 🎯 Understanding the Codebase

### 📁 Project Structure Overview

```
blender/
├── source/blender/          # Main source code
│   ├── blenlib/            # Core utilities (math, data structures)
│   ├── blenkernel/         # Application logic and data management
│   ├── editors/            # User interface components
│   ├── render/             # Rendering engines (Cycles, EEVEE)
│   ├── gpu/                # GPU computing and OpenGL
│   ├── nodes/              # Node system (materials, compositing)
│   └── python/             # Python API
├── extern/                 # External dependencies
├── build_files/            # Build system utilities
├── doc/                    # Documentation
└── scripts/                # Python scripts and addons
```

### 🔧 Key Components to Know

#### 📚 BlenLib (`source/blender/blenlib/`)
- **Purpose**: Core utility library
- **Key Files**: `BLI_math_vector_types.hh`, `BLI_vector.hh`, `BLI_string.h`
- **Learning Focus**: Data structures, mathematical operations

#### 🧠 BlenKernel (`source/blender/blenkernel/`)
- **Purpose**: Application logic and data management
- **Key Files**: `BKE_scene.h`, `BKE_object.h`, `BKE_mesh.h`
- **Learning Focus**: Scene graph, object management, data flow

#### 🎨 Editors (`source/blender/editors/`)
- **Purpose**: User interface components
- **Key Files**: `ED_view3d.h`, `ED_screen.h`, `ED_util.h`
- **Learning Focus**: UI architecture, event handling

## 🔧 Development Workflow

### 📋 Daily Development Process

#### 1. Morning Setup
```bash
# Update your local repository
git pull origin main
git submodule update

# Rebuild if needed
cd build
make -j$(nproc)
```

#### 2. Development Session
```bash
# Create feature branch
git checkout -b feature/my-new-feature

# Make changes and test
# ... edit code ...
make -j$(nproc)

# Test your changes
./bin/blender --python-expr "import bpy; print('Test successful')"
```

#### 3. Code Quality Checks
```bash
# Format code
make format

# Run static analysis
make clang-tidy

# Run tests
make test
```

### 🔄 Git Workflow

#### Branching Strategy
```bash
# Main development branch
git checkout main

# Feature branches
git checkout -b feature/description
git checkout -b bugfix/description
git checkout -b cleanup/description
```

#### Commit Guidelines
```bash
# Good commit message format
git commit -m "Component: Brief description of changes

Detailed explanation of what was changed and why.
Include any important technical details.

Fixes: #1234"
```

**Commit Message Categories**:
- `BlenLib`: Core utility changes
- `BlenKernel`: Data management changes
- `Editors`: UI and editor changes
- `Render`: Rendering engine changes
- `GPU`: GPU computing changes
- `Nodes`: Node system changes
- `Python`: Python API changes

### 🧪 Testing Your Changes

#### Unit Tests
```bash
# Run all tests
make test

# Run specific test
./bin/tests/gtests/blenlib/tests/BLI_vector_test

# Run Python tests
make test_python
```

#### Manual Testing
```bash
# Test specific functionality
./bin/blender --python-expr "
import bpy
# Your test code here
"

# Test with specific file
./bin/blender test_file.blend --python-expr "
import bpy
# Test code
"
```

## 🎯 Common Development Tasks

### 🔧 Adding a New Feature

#### 1. Plan Your Feature
- **Research**: Understand existing similar features
- **Design**: Plan the user interface and data flow
- **Implementation**: Break down into manageable tasks

#### 2. Implementation Steps
```cpp
// 1. Add data structures (if needed)
// In appropriate BKE_*.h file
struct MyNewData {
  char name[64];
  float value;
  // ...
};

// 2. Add UI elements
// In appropriate ED_*.c file
void ED_my_new_operator(bContext *C) {
  // Implementation
}

// 3. Add Python API (if needed)
// In source/blender/python/
static PyObject *py_my_new_function(PyObject *self, PyObject *args) {
  // Python binding
}
```

#### 3. Testing and Documentation
```bash
# Test your feature
./bin/blender --python-expr "
import bpy
# Test your new feature
"

# Update documentation
# Edit relevant .md files
```

### 🐛 Fixing a Bug

#### 1. Reproduce the Bug
```bash
# Create minimal test case
./bin/blender --python-expr "
import bpy
# Minimal code to reproduce bug
"
```

#### 2. Debug the Issue
```cpp
// Add debug output
printf("Debug: value = %f\n", value);

// Use Blender's debug system
BLI_assert(condition);

// Use debugger
// Set breakpoints in your IDE
```

#### 3. Fix and Test
```bash
# Make your fix
# Test thoroughly
make test

# Verify fix works
./bin/blender --python-expr "
import bpy
# Test that bug is fixed
"
```

## 🔍 Debugging Techniques

### 🐛 Common Debugging Tools

#### Print Debugging
```cpp
// Simple printf debugging
printf("Debug: %s = %d\n", variable_name, value);

// Blender's debug system
BLI_assert(condition);
BLI_assert_msg(condition, "Error message");

// Debug macros
#ifdef DEBUG
  printf("Debug info\n");
#endif
```

#### IDE Debugging
```bash
# Set up debugging in your IDE
# Configure launch settings for ./bin/blender
# Set breakpoints in source code
# Use step-through debugging
```

#### Python Debugging
```python
# Python debugging
import pdb; pdb.set_trace()

# Or use Blender's Python console
import bpy
# Interactive debugging
```

### 🔍 Performance Profiling

#### CPU Profiling
```bash
# Use Blender's built-in profiler
./bin/blender --python-expr "
import bpy
import cProfile
cProfile.run('your_function()')
"
```

#### Memory Profiling
```cpp
// Track memory usage
MEM_PrintMemList();

// Check for memory leaks
// Use tools like Valgrind (Linux) or AddressSanitizer
```

## 📚 Learning Resources

### 📖 Essential Documentation
- **[Architecture Documentation](docs/ARCHITECTURE.md)**: System design overview
- **[Build System Documentation](docs/BUILD_SYSTEM.md)**: Build process details
- **[Component Documentation](docs/components/)**: Individual component guides
- **[API Documentation](https://docs.blender.org/api/)**: Python API reference

### 🔗 Community Resources
- **[Developer Forum](https://devtalk.blender.org)**: Technical discussions
- **[Bug Tracker](https://projects.blender.org)**: Report bugs and track issues
- **[Code Review](https://projects.blender.org)**: Submit and review patches
- **[IRC/Discord](https://www.blender.org/community/)**: Real-time chat

### 📚 Recommended Reading
- **Blender Source Code**: Start with `source/blender/blenlib/` and `source/blender/blenkernel/`
- **C++ Best Practices**: Modern C++ development techniques
- **3D Graphics**: Understanding of 3D mathematics and graphics programming
- **Open Source Development**: Git workflow and collaboration

## 🎯 First Contribution Guide

### 🚀 Beginner-Friendly Tasks

#### 1. Documentation Improvements
- Fix typos in documentation
- Add missing examples
- Improve code comments
- Update README files

#### 2. Bug Fixes
- Look for "good first issue" labels
- Fix simple bugs
- Improve error messages
- Add missing error checking

#### 3. Code Cleanup
- Remove unused code
- Fix compiler warnings
- Improve code formatting
- Add missing const qualifiers

### 📋 Contribution Process

#### 1. Find a Task
```bash
# Browse issues on projects.blender.org
# Look for "good first issue" or "help wanted" labels
# Start with small, well-defined tasks
```

#### 2. Implement Your Fix
```bash
# Create feature branch
git checkout -b fix/issue-description

# Make your changes
# Test thoroughly
# Follow coding standards
```

#### 3. Submit Your Contribution
```bash
# Push your branch
git push origin fix/issue-description

# Create pull request on projects.blender.org
# Include clear description of changes
# Reference related issues
```

## 🔧 Development Tools Setup

### 🛠️ IDE Configuration

#### VS Code Setup
```json
// .vscode/settings.json
{
  "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools",
  "cmake.configureOnOpen": true,
  "cmake.buildDirectory": "${workspaceFolder}/build"
}
```

#### CLion Setup
- Open the Blender project
- Configure CMake settings
- Set up debugging configuration
- Install Blender-specific plugins

#### Visual Studio Setup
- Open CMake project
- Configure build settings
- Set up debugging
- Install C++ tools

### 🔧 Useful Development Scripts

#### Build Scripts
```bash
#!/bin/bash
# quick-build.sh
cd build
make -j$(nproc) && echo "Build successful!" || echo "Build failed!"
```

#### Test Scripts
```bash
#!/bin/bash
# run-tests.sh
cd build
make test && echo "Tests passed!" || echo "Tests failed!"
```

## 🎯 Advanced Topics

### 🔧 Performance Optimization
- **Profiling**: Use Blender's built-in profiler
- **Memory Management**: Understand Blender's memory system
- **SIMD Optimization**: Leverage vectorized operations
- **Threading**: Use Blender's task system

### 🔧 GPU Development
- **OpenGL**: Understand Blender's GPU abstraction
- **Shaders**: Learn GLSL shader programming
- **Compute**: Use GPU compute for parallel processing

### 🔧 Python API Development
- **C++ Bindings**: Create Python bindings for C++ functions
- **Operator Development**: Create new Blender operators
- **Addon Development**: Build Python addons

## 🔮 Next Steps

### 📈 Skill Development Path

#### Beginner (0-6 months)
- [ ] Complete setup and first build
- [ ] Understand basic codebase structure
- [ ] Make first documentation contribution
- [ ] Fix simple bugs

#### Intermediate (6-12 months)
- [ ] Contribute feature improvements
- [ ] Understand core systems deeply
- [ ] Help review other contributions
- [ ] Mentor new contributors

#### Advanced (1+ years)
- [ ] Lead major features
- [ ] Design system architecture
- [ ] Maintain core components
- [ ] Guide project direction

### 🎯 Long-term Goals
- **Deep Expertise**: Master specific Blender subsystems
- **Community Leadership**: Help guide project direction
- **Mentorship**: Help new developers get started
- **Innovation**: Contribute to Blender's future

---

*Welcome to the Blender development community! Your contributions help make Blender better for everyone.*

**Last Updated**: 2024-12-19
**Version**: 1.0 