# 📥 Installation Guide

Welcome to Bl3nder! This guide will walk you through the installation process for your operating system.

## System Requirements

### Minimum Requirements
- **OS**: Windows 10/11, macOS 10.15+, or Linux (64-bit)
- **CPU**: 64-bit quad-core CPU with SSE2 support
- **RAM**: 8GB (16GB recommended)
- **GPU**: 2GB VRAM, OpenGL 4.3 compatible
- **Storage**: 10GB free space

### Recommended Specifications
- **CPU**: 8-core or better
- **RAM**: 32GB or more
- **GPU**: 8GB+ VRAM, NVIDIA RTX 2070 or better with latest drivers
- **Storage**: SSD with 50GB+ free space

## Download Bl3nder

### Latest Stable Release
- [Download for Windows (64-bit)](https://example.com/download/windows)
- [Download for macOS (Universal)](https://example.com/download/macos)
- [Download for Linux (AppImage)](https://example.com/download/linux)

> ℹ️ **Note**: For production work, we recommend using the latest stable release.

### Development Builds
For the latest features and improvements, you can try our [nightly builds](https://example.com/nightly).

## Installation Instructions

### Windows
1. Download the installer (`.exe` file)
2. Run the installer as administrator
3. Follow the on-screen instructions
4. (Optional) Add Bl3nder to your system PATH

### macOS
1. Download the `.dmg` file
2. Open the downloaded file
3. Drag Bl3nder to your Applications folder
4. (First run) Right-click and select "Open" to bypass security restrictions

### Linux
```bash
# Make the AppImage executable
chmod +x Bl3nder-*.AppImage

# Run Bl3nder
./Bl3nder-*.AppImage
```

## Verifying Your Installation

To verify that Bl3nder is installed correctly:

1. Launch Bl3nder
2. Open the Python Console (Window > Toggle System Console)
3. Enter the following command:
   ```python
   import bpy
   print("Bl3nder version:", bpy.app.version_string)
   print("AI Engine:", bpy.app.ai_version)
   ```

## Updating Bl3nder

Bl3nder will notify you when updates are available. You can also check manually from the Help menu.

## Troubleshooting

### Common Issues

#### Installation Fails
- Ensure you have administrator privileges
- Check that your system meets the minimum requirements
- Disable any antivirus software temporarily

#### Bl3nder Won't Start
- Update your graphics drivers
- Try running in safe mode (add `--safe-mode` to the command line)
- Check the [Troubleshooting Guide](troubleshooting.md) for more solutions

## Next Steps

- [Quick Start Guide](quick-start.md)
- [Configuration](configuration.md)
- [Getting Help](https://github.com/MKWorldWide/Bl3nder/discussions)

---

> 💡 **Tip**: Join our [Discord community](https://discord.gg/mkworldwide) for real-time support and updates!
