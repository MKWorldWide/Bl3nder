# 🤝 Contributing to Bl3nder

Thank you for your interest in contributing to Bl3nder! This guide will help you get started with contributing to our AI-enhanced Blender fork.

## 📋 Table of Contents

- [Getting Started](#getting-started)
- [Development Setup](#development-setup)
- [Contribution Guidelines](#contribution-guidelines)
- [Code Standards](#code-standards)
- [Testing](#testing)
- [Documentation](#documentation)
- [AI Development](#ai-development)
- [Render Engine Development](#render-engine-development)
- [Community Guidelines](#community-guidelines)
- [Support](#support)

## 🚀 Getting Started

### Prerequisites
- **Git**: Version control system
- **CMake**: Build system (3.16+)
- **C++ Compiler**: GCC 9+, Clang 10+, or MSVC 2019+
- **Python**: 3.10+ (for AI features)
- **GPU**: OpenGL 4.3+ compatible graphics card (recommended)

### Quick Start
```bash
# Clone the repository
git clone https://github.com/your-org/bl3nder.git
cd bl3nder

# Install dependencies
./download_libs.sh

# Build the project
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## 🔧 Development Setup

### Environment Setup

#### Linux
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential cmake git python3-dev
sudo apt-get install libboost-all-dev libeigen3-dev libopenexr-dev

# Fedora
sudo dnf install gcc-c++ cmake git python3-devel
sudo dnf install boost-devel eigen3-devel openexr-devel
```

#### macOS
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake boost eigen openexr
```

#### Windows
```bash
# Install Visual Studio 2019 or later with C++ development tools
# Install CMake from https://cmake.org/download/
# Install Git from https://git-scm.com/download/win
```

### IDE Setup

#### Visual Studio Code
```json
{
    "cmake.configureOnOpen": true,
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "cmake.generator": "Unix Makefiles"
}
```

#### CLion
- Open the project directory
- CLion will automatically detect CMake configuration
- Configure build settings in Settings > Build, Execution, Deployment > CMake

## 📝 Contribution Guidelines

### Issue Reporting
Before contributing, please check if your issue or feature request already exists:

1. **Search Issues**: Use the search function to find similar issues
2. **Check Discussions**: Look in GitHub Discussions for ongoing conversations
3. **Create Issue**: If not found, create a new issue with:
   - Clear title describing the problem/feature
   - Detailed description with steps to reproduce
   - Expected vs actual behavior
   - System information (OS, GPU, etc.)
   - Screenshots or logs if applicable

### Pull Request Process

1. **Fork the Repository**: Create your own fork of the project
2. **Create Feature Branch**: `git checkout -b feature/amazing-feature`
3. **Make Changes**: Implement your feature or fix
4. **Test Thoroughly**: Ensure all tests pass and new features work
5. **Update Documentation**: Add or update relevant documentation
6. **Commit Changes**: Use clear, descriptive commit messages
7. **Push to Branch**: `git push origin feature/amazing-feature`
8. **Create Pull Request**: Submit PR with detailed description

### Commit Message Format
```
type(scope): description

[optional body]

[optional footer]
```

**Types:**
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes (formatting, etc.)
- `refactor`: Code refactoring
- `test`: Adding or updating tests
- `chore`: Maintenance tasks

**Examples:**
```
feat(ai): add scene complexity analysis to Lilybear

- Implement scene complexity calculation
- Add performance optimization based on complexity
- Update documentation with new features

Closes #123
```

## 🎨 Code Standards

### C++ Standards
- **C++17**: Use C++17 features where appropriate
- **Naming**: Use snake_case for variables and functions, PascalCase for classes
- **Comments**: Use Doxygen-style comments for public APIs
- **Error Handling**: Use exceptions for exceptional cases, return codes for expected failures

### Python Standards
- **PEP 8**: Follow Python style guide
- **Type Hints**: Use type hints for function parameters and return values
- **Docstrings**: Use Google-style docstrings
- **Testing**: Use pytest for unit tests

### File Organization
```
source/blender/
├── render/lilybear/          # Lilybear render engine
├── ai_agent/                 # AI agent system
├── athenamist_integration/   # AthenaMist integration
└── [other modules]/
```

## 🧪 Testing

### Running Tests
```bash
# Build with tests enabled
cmake .. -DCMAKE_BUILD_TYPE=Debug -DWITH_TESTS=ON
make -j$(nproc)

# Run all tests
make test

# Run specific test suite
ctest -R "lilybear"
```

### Writing Tests
- **Unit Tests**: Test individual functions and classes
- **Integration Tests**: Test component interactions
- **Performance Tests**: Test rendering performance and AI processing
- **UI Tests**: Test user interface functionality

### Test Coverage
- Aim for 80%+ code coverage
- Focus on critical paths and edge cases
- Include tests for AI features and render engines

## 📚 Documentation

### Documentation Standards
- **Quantum Detail**: Provide comprehensive, detailed documentation
- **Cross-References**: Link related documentation sections
- **Examples**: Include practical usage examples
- **Real-Time Updates**: Keep documentation current with code changes

### Documentation Types
- **API Documentation**: Function and class documentation
- **Architecture Documentation**: System design and component relationships
- **User Guides**: End-user tutorials and guides
- **Developer Guides**: Development setup and contribution guidelines

### Updating Documentation
1. **Inline Comments**: Add detailed comments in code files
2. **README Updates**: Update main documentation for significant changes
3. **API Docs**: Update API documentation for new/changed functions
4. **Architecture Docs**: Update system architecture documentation

## 🤖 AI Development

### AI Agent Development
The AI agent system is located in `source/blender/ai_agent/`:

```python
# Example AI agent structure
class SceneAnalysisAgent:
    """AI agent for scene complexity analysis."""
    
    def analyze_scene(self, scene_data):
        """Analyze scene complexity and suggest optimizations."""
        # Implementation here
        pass
```

### AI Integration Guidelines
- **Modular Design**: Keep AI components loosely coupled
- **Performance**: Optimize AI processing for real-time use
- **Error Handling**: Graceful degradation when AI features fail
- **User Control**: Always allow users to disable AI features

### AI Testing
```python
# Example AI test
def test_scene_analysis():
    agent = SceneAnalysisAgent()
    scene_data = create_test_scene()
    result = agent.analyze_scene(scene_data)
    assert result.complexity_score > 0
    assert result.optimization_suggestions
```

## 🎨 Render Engine Development

### Lilybear Development
The Lilybear render engine is located in `source/blender/render/lilybear/`:

```cpp
// Example render engine structure
class LilybearEngine : public RenderEngine {
public:
    RenderEngine *create_instance() override;
    void render(RenderData *rd) override;
};
```

### Render Engine Guidelines
- **Performance**: Optimize for real-time rendering
- **Quality**: Maintain high visual quality standards
- **Compatibility**: Ensure compatibility with existing Blender features
- **Extensibility**: Design for easy feature additions

### Render Engine Testing
```cpp
// Example render engine test
TEST(LilybearEngine, BasicRendering) {
    LilybearEngine engine;
    RenderData rd = create_test_render_data();
    engine.render(&rd);
    // Verify render results
}
```

## 🌐 Community Guidelines

### Code of Conduct
- **Respect**: Treat all community members with respect
- **Inclusivity**: Welcome contributors from all backgrounds
- **Constructive Feedback**: Provide helpful, constructive feedback
- **Collaboration**: Work together to improve the project

### Communication
- **GitHub Issues**: Use for bug reports and feature requests
- **GitHub Discussions**: Use for general questions and discussions
- **Pull Requests**: Use for code contributions
- **Documentation**: Contribute to documentation improvements

### Recognition
- **Contributors**: All contributors are recognized in the project
- **Documentation**: Credit contributors in documentation
- **Releases**: Acknowledge contributions in release notes

## 🆘 Support

### Getting Help
- **[Documentation](docs/)**: Comprehensive project documentation
- **[Issues](https://github.com/your-org/bl3nder/issues)**: Report bugs and request features
- **[Discussions](https://github.com/your-org/bl3nder/discussions)**: Community discussions and support
- **[Blender Community](https://www.blender.org/community/)**: Official Blender community support

### Development Resources
- **[Architecture Guide](docs/ARCHITECTURE.md)**: System architecture documentation
- **[Build System Guide](docs/BUILD_SYSTEM.md)**: Build system documentation
- **[API Documentation](doc/python_api/)**: Python API reference
- **[Developer Forum](https://devtalk.blender.org)**: Blender developer community

### Funding Support
Support the development of Bl3nder:
- **[Blender Foundation](https://fund.blender.org/)**: Support the core Blender project
- **[Community Funding](https://www.blender.org/funding.json)**: Support community-driven features

## 🎯 Contribution Areas

### High Priority
- **AI Feature Enhancement**: Improve AI capabilities and performance
- **Render Engine Optimization**: Optimize Lilybear and other render engines
- **Documentation**: Improve and expand documentation
- **Testing**: Add comprehensive test coverage
- **Performance**: Optimize overall system performance

### Medium Priority
- **UI/UX Improvements**: Enhance user interface and experience
- **New Features**: Add new AI and rendering features
- **Platform Support**: Improve cross-platform compatibility
- **Community Tools**: Enhance community collaboration tools

### Low Priority
- **Experimental Features**: Explore cutting-edge AI and rendering techniques
- **Research Integration**: Integrate research findings into the project
- **Educational Content**: Create tutorials and educational materials

## 🚀 Getting Started Checklist

- [ ] Set up development environment
- [ ] Build project from source
- [ ] Run tests successfully
- [ ] Read architecture documentation
- [ ] Choose contribution area
- [ ] Create first issue or pull request
- [ ] Join community discussions
- [ ] Start contributing!

---

**Thank you for contributing to Bl3nder!** 🚀

*Together, we're building the future of AI-enhanced 3D creation.* 