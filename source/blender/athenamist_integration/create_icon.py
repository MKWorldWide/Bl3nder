#!/usr/bin/env python3
"""
AthenaMist AI Agent Icon Generator
==================================

Creates a beautiful icon for the AthenaMist AI Agent DMG.
"""

import os
import sys
from pathlib import Path

# Add PIL to path if available
try:
    from PIL import Image, ImageDraw, ImageFont
    PIL_AVAILABLE = True
except ImportError:
    PIL_AVAILABLE = False
    print("PIL not available, creating text-based icon")

def create_text_icon():
    """Create a simple text-based icon."""
    icon_content = """
    ╔══════════════════════════════════════════════════════════════╗
    ║                                                              ║
    ║                    🎉 ATHENAMIST 🎉                         ║
    ║                                                              ║
    ║                   AI AGENT FOR BLENDER                      ║
    ║                                                              ║
    ║                    🤖 ✨ 🎨 🚀                               ║
    ║                                                              ║
    ║              Revolutionary AI Assistant                      ║
    ║                                                              ║
    ║              • Natural Language Understanding                ║
    ║              • Multiple AI Personalities                     ║
    ║              • Immersive Interface                           ║
    ║              • Workflow Automation                           ║
    ║                                                              ║
    ╚══════════════════════════════════════════════════════════════╝
    """
    
    # Create icon directory
    icon_dir = Path(__file__).parent
    icon_file = icon_dir / "icon.txt"
    
    with open(icon_file, 'w') as f:
        f.write(icon_content)
    
    print(f"Text icon created: {icon_file}")
    return icon_file

def create_pil_icon():
    """Create a beautiful PIL-based icon."""
    # Create a 512x512 icon
    size = (512, 512)
    icon = Image.new('RGBA', size, (0, 0, 0, 0))
    draw = ImageDraw.Draw(icon)
    
    # Create gradient background
    for y in range(size[1]):
        # Purple to blue gradient
        r = int(102 + (y / size[1]) * 30)  # 102-132
        g = int(126 + (y / size[1]) * 30)  # 126-156
        b = int(234 + (y / size[1]) * 21)  # 234-255
        color = (r, g, b, 255)
        draw.line([(0, y), (size[0], y)], fill=color)
    
    # Draw main circle
    center = (size[0] // 2, size[1] // 2)
    radius = 180
    draw.ellipse([
        center[0] - radius, center[1] - radius,
        center[0] + radius, center[1] + radius
    ], fill=(255, 255, 255, 200), outline=(255, 255, 255, 255), width=8)
    
    # Draw AI brain pattern
    brain_points = []
    for i in range(8):
        angle = (i / 8) * 2 * 3.14159
        x = center[0] + int(120 * (0.8 + 0.2 * (i % 2)) * (1 if i % 2 == 0 else 0.7) * (1 if i < 4 else 0.8))
        y = center[1] + int(120 * (0.8 + 0.2 * (i % 2)) * (1 if i % 2 == 0 else 0.7) * (1 if i < 4 else 0.8))
        brain_points.append((x, y))
    
    # Draw brain connections
    for i in range(len(brain_points)):
        for j in range(i + 1, len(brain_points)):
            if (i + j) % 3 == 0:  # Connect some points
                draw.line([brain_points[i], brain_points[j]], 
                         fill=(102, 126, 234, 180), width=3)
    
    # Draw brain nodes
    for point in brain_points:
        draw.ellipse([
            point[0] - 8, point[1] - 8,
            point[0] + 8, point[1] + 8
        ], fill=(102, 126, 234, 255))
    
    # Add text
    try:
        # Try to use a nice font
        font_large = ImageFont.truetype("/System/Library/Fonts/Arial.ttf", 48)
        font_small = ImageFont.truetype("/System/Library/Fonts/Arial.ttf", 24)
    except:
        # Fallback to default font
        font_large = ImageFont.load_default()
        font_small = ImageFont.load_default()
    
    # Draw title
    title = "ATHENAMIST"
    title_bbox = draw.textbbox((0, 0), title, font=font_large)
    title_width = title_bbox[2] - title_bbox[0]
    title_x = center[0] - title_width // 2
    title_y = center[1] + radius + 20
    
    # Draw title with shadow
    draw.text((title_x + 2, title_y + 2), title, font=font_large, fill=(0, 0, 0, 100))
    draw.text((title_x, title_y), title, font=font_large, fill=(255, 255, 255, 255))
    
    # Draw subtitle
    subtitle = "AI Agent"
    subtitle_bbox = draw.textbbox((0, 0), subtitle, font=font_small)
    subtitle_width = subtitle_bbox[2] - subtitle_bbox[0]
    subtitle_x = center[0] - subtitle_width // 2
    subtitle_y = title_y + 60
    
    draw.text((subtitle_x + 1, subtitle_y + 1), subtitle, font=font_small, fill=(0, 0, 0, 100))
    draw.text((subtitle_x, subtitle_y), subtitle, font=font_small, fill=(255, 255, 255, 200))
    
    # Add some sparkles
    sparkle_positions = [
        (center[0] - 200, center[1] - 200),
        (center[0] + 200, center[1] - 180),
        (center[0] - 180, center[1] + 200),
        (center[0] + 180, center[1] + 180),
    ]
    
    for sparkle_pos in sparkle_positions:
        # Draw star
        points = []
        for i in range(5):
            angle = (i / 5) * 2 * 3.14159
            outer_x = sparkle_pos[0] + int(15 * (1 if i % 2 == 0 else 0.5) * (1 if i < 2 else 0.8))
            outer_y = sparkle_pos[1] + int(15 * (1 if i % 2 == 0 else 0.5) * (1 if i < 2 else 0.8))
            points.append((outer_x, outer_y))
        
        # Draw star shape
        for i in range(len(points)):
            draw.line([points[i], points[(i + 2) % len(points)]], 
                     fill=(255, 255, 255, 150), width=2)
    
    # Save icon
    icon_dir = Path(__file__).parent
    icon_file = icon_dir / "icon.png"
    icon.save(icon_file, 'PNG')
    
    print(f"PIL icon created: {icon_file}")
    return icon_file

def main():
    """Main function to create the icon."""
    print("🎨 Creating AthenaMist AI Agent Icon...")
    
    if PIL_AVAILABLE:
        try:
            icon_file = create_pil_icon()
            print(f"✅ Beautiful icon created: {icon_file}")
        except Exception as e:
            print(f"⚠️  PIL icon creation failed: {e}")
            print("📝 Falling back to text icon...")
            icon_file = create_text_icon()
    else:
        print("📝 Creating text-based icon...")
        icon_file = create_text_icon()
    
    print("🎉 Icon creation completed!")
    return icon_file

if __name__ == "__main__":
    main() 