# ⚙️ Configuration Guide

Customize Bl3nder to fit your workflow with these configuration options.

## User Preferences

### Accessing Preferences
- **Windows/Linux**: `Edit > Preferences`
- **macOS**: `Bl3nder > Preferences`
- **Shortcut**: `Cmd + ,` (macOS) or `Ctrl + ,` (Windows/Linux)

### Key Settings

#### Interface
```python
# Example: Set UI scale
import bpy
bpy.context.preferences.view.ui_scale = 1.2
```

#### Input
- **Emulate 3-Button Mouse**: For trackpad users
- **Select With**: Left/Right mouse button selection
- **Keymap**: Customize keyboard shortcuts

#### File Paths
- **Temporary Directory**: Set a custom temp folder
- **Render Output**: Default render output location
- **Asset Libraries**: Configure your asset library paths

## Startup Configuration

### Default Startup File
Customize the default scene that loads when starting Bl3nder:
1. Set up your scene as desired
2. `File > Defaults > Save Startup File`

### Command Line Arguments
Run Bl3nder with these common arguments:

```bash
# Basic usage
blender --python-use-system-env

# Run a script on startup
blender --python /path/to/script.py

# Start in background mode (no UI)
blender -b

# Render an animation
blender -b /path/to/file.blend -a
```

## AI Configuration

### API Keys
1. Go to `Edit > Preferences > Add-ons > AI Assistant`
2. Enter your API key
3. Configure model preferences

### Local AI Models
To use local AI models:
1. Download models from the AI Model Manager
2. Configure in `Preferences > File Paths > AI Models`
3. Select your preferred model in the AI Assistant panel

## Performance Settings

### System
- **Memory & Limits**: Adjust undo steps and system memory
- **Viewport**: Optimize viewport performance
- **Render**: Configure render settings for your hardware

### GPU Acceleration
1. Go to `Edit > Preferences > System`
2. Select your GPU under `Cycles Render Devices`
3. Enable `OptiX` for NVIDIA RTX cards

## Customization

### Themes
1. `Edit > Preferences > Themes`
2. Customize colors or import themes
3. Save your theme for future use

### Add-ons
1. `Edit > Preferences > Add-ons`
2. Enable/disable add-ons
3. Configure add-on settings

## Environment Variables

Set these in your system environment or `blender.cfg`:

```ini
# Enable GPU rendering
CYCLES_DEVICE=CUDA+CPU

# Set temporary directory
TMP=/path/to/temp

# Enable debug logging
BLENDER_SYSTEM_SCRIPTS=/path/to/scripts
```

## Configuration Files

### Locations
- **Windows**: `%USERPROFILE%\AppData\Roaming\Blender\3.6\config\`
- **macOS**: `~/Library/Application Support/Blender/3.6/config/`
- **Linux**: `~/.config/blender/3.6/config/`

### Important Files
- `userpref.blend`: User preferences
- `startup.blend`: Default startup file
- `keymap.xml`: Custom keymaps

## Troubleshooting

### Reset to Defaults
1. Close Bl3nder
2. Rename the config directory
3. Restart Bl3nder to generate fresh configs

### Logs
- **Debug Log**: `Window > Toggle System Console`
- **Crash Logs**: Check the config directory

## Next Steps
- [Advanced Configuration](../guides/advanced-configuration.md)
- [Performance Optimization](../features/performance.md)
- [Python API Reference](../api/python.md)
