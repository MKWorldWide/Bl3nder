# ⚡ Quick Start Guide

Welcome to Bl3nder! This guide will help you create your first 3D scene with AI assistance in under 5 minutes.

## First Launch

1. **Start Bl3nder** from your applications menu or desktop shortcut
2. **Select a Theme** - Choose between Dark and Light mode
3. **Set Up Workspace** - Select "General" for a balanced layout

## Your First Scene

### 1. Create a Basic Object
```python
import bpy

# Create a cube
bpy.ops.mesh.primitive_cube_add(size=2, location=(0, 0, 1))

# Add a material
mat = bpy.data.materials.new(name="MyMaterial")
mat.use_nodes = True
bsdf = mat.node_tree.nodes["Principled BSDF"]
bsdf.inputs["Base Color"].default_value = (0.8, 0.2, 0.2, 1.0)
bpy.context.object.data.materials.append(mat)
```

### 2. Enable AI Assistant
1. Open the AI Panel (Press `N` to toggle sidebar)
2. Click "Enable AI Assistant"
3. Sign in with your MK WorldWide account

### 3. Try AI-Powered Features

#### AI Material Generation
1. Select your object
2. In the AI Panel, click "Generate Material"
3. Type a description (e.g., "rusty metal with scratches")
4. Click "Generate" and watch the magic happen!

#### AI Lighting Setup
1. Delete the default light (select and press `X`)
2. In the AI Panel, click "AI Lighting Setup"
3. Choose a lighting style (e.g., "studio portrait" or "sunset")
4. Let the AI configure the perfect lighting

### 4. Render Your Scene
1. Press `F12` to render
2. Or click the Render button in the top bar
3. Save your render with `F3`

## Essential Shortcuts

| Shortcut | Action |
|----------|--------|
| `Tab` | Toggle Edit/Object mode |
| `Shift + A` | Add menu |
| `G` | Grab/move |
| `R` | Rotate |
| `S` | Scale |
| `F3` | Search operators |
| `Ctrl + Z` | Undo |
| `Ctrl + Shift + Z` | Redo |

## Next Steps

- [Explore the Interface](interface.md)
- [Learn About AI Features](../features/ai-integration.md)
- [Join Our Community](../community/)

## Getting Help

- Press `F1` for in-app help
- Join our [Discord](https://discord.gg/mkworldwide)
- Check the [FAQ](../faq.md)

> 💡 **Pro Tip**: Use the `F3` search to quickly find any tool or setting in Bl3nder!
