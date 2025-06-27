#!/bin/bash

# AthenaMist AI Agent DMG Builder
# ===============================
# Creates a professional DMG installer for macOS

set -e  # Exit on any error

# Configuration
APP_NAME="AthenaMist AI Agent"
VERSION="1.0.0"
DMG_NAME="AthenaMist_AI_Agent_v${VERSION}_macOS.dmg"
TEMP_DIR="temp_dmg"
APP_DIR="${TEMP_DIR}/${APP_NAME}.app"
CONTENTS_DIR="${APP_DIR}/Contents"
MACOS_DIR="${CONTENTS_DIR}/MacOS"
RESOURCES_DIR="${CONTENTS_DIR}/Resources"
SCRIPTS_DIR="${CONTENTS_DIR}/Scripts"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

echo -e "${PURPLE}🎉 AthenaMist AI Agent DMG Builder${NC}"
echo -e "${CYAN}=====================================${NC}"
echo ""

# Check if we're on macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    echo -e "${RED}❌ This script must be run on macOS${NC}"
    exit 1
fi

# Check for required tools
echo -e "${BLUE}🔍 Checking required tools...${NC}"
if ! command -v create-dmg &> /dev/null; then
    echo -e "${YELLOW}⚠️  create-dmg not found. Installing...${NC}"
    if command -v brew &> /dev/null; then
        brew install create-dmg
    else
        echo -e "${RED}❌ Homebrew not found. Please install create-dmg manually:${NC}"
        echo "   brew install create-dmg"
        exit 1
    fi
fi

# Clean up previous builds
echo -e "${BLUE}🧹 Cleaning up previous builds...${NC}"
rm -rf "${TEMP_DIR}"
rm -f "${DMG_NAME}"

# Create directory structure
echo -e "${BLUE}📁 Creating directory structure...${NC}"
mkdir -p "${MACOS_DIR}"
mkdir -p "${RESOURCES_DIR}"
mkdir -p "${SCRIPTS_DIR}"

# Create Info.plist
echo -e "${BLUE}📝 Creating Info.plist...${NC}"
cat > "${CONTENTS_DIR}/Info.plist" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>AthenaMistInstaller</string>
    <key>CFBundleIdentifier</key>
    <string>com.athenamist.blender-ai-agent</string>
    <key>CFBundleName</key>
    <string>${APP_NAME}</string>
    <key>CFBundleVersion</key>
    <string>${VERSION}</string>
    <key>CFBundleShortVersionString</key>
    <string>${VERSION}</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleSignature</key>
    <string>????</string>
    <key>LSMinimumSystemVersion</key>
    <string>10.15</string>
    <key>NSHighResolutionCapable</key>
    <true/>
    <key>NSPrincipalClass</key>
    <string>NSApplication</string>
    <key>CFBundleIconFile</key>
    <string>AppIcon</string>
    <key>CFBundleDocumentTypes</key>
    <array>
        <dict>
            <key>CFBundleTypeName</key>
            <string>Blender Add-on</string>
            <key>CFBundleTypeExtensions</key>
            <array>
                <string>py</string>
            </array>
            <key>CFBundleTypeRole</key>
            <string>Editor</string>
        </dict>
    </array>
</dict>
</plist>
EOF

# Create the installer script
echo -e "${BLUE}🔧 Creating installer script...${NC}"
cat > "${MACOS_DIR}/AthenaMistInstaller" << 'EOF'
#!/bin/bash

# AthenaMist AI Agent Installer
# =============================

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m'

echo -e "${PURPLE}🎉 AthenaMist AI Agent Installer${NC}"
echo -e "${CYAN}===================================${NC}"
echo ""

# Get Blender version
echo -e "${BLUE}🔍 Detecting Blender installation...${NC}"

BLENDER_VERSIONS=()
BLENDER_PATHS=(
    "/Applications/Blender.app"
    "$HOME/Applications/Blender.app"
)

# Find installed Blender versions
for path in "${BLENDER_PATHS[@]}"; do
    if [[ -d "$path" ]]; then
        version=$(defaults read "$path/Contents/Info.plist" CFBundleShortVersionString 2>/dev/null || echo "Unknown")
        BLENDER_VERSIONS+=("$version")
        echo -e "${GREEN}✓ Found Blender $version at: $path${NC}"
    fi
done

if [[ ${#BLENDER_VERSIONS[@]} -eq 0 ]]; then
    echo -e "${RED}❌ No Blender installation found!${NC}"
    echo -e "${YELLOW}Please install Blender first from: https://www.blender.org/download/${NC}"
    read -p "Press Enter to exit..."
    exit 1
fi

# Get add-ons directory
echo -e "${BLUE}📁 Locating Blender add-ons directory...${NC}"
ADDONS_DIR="$HOME/Library/Application Support/Blender"

if [[ ! -d "$ADDONS_DIR" ]]; then
    echo -e "${YELLOW}⚠️  Add-ons directory not found. Creating...${NC}"
    mkdir -p "$ADDONS_DIR"
fi

# Find the most recent Blender version
LATEST_VERSION=""
LATEST_PATH=""

for path in "${BLENDER_PATHS[@]}"; do
    if [[ -d "$path" ]]; then
        version=$(defaults read "$path/Contents/Info.plist" CFBundleShortVersionString 2>/dev/null || echo "0.0.0")
        if [[ "$version" > "$LATEST_VERSION" ]]; then
            LATEST_VERSION="$version"
            LATEST_PATH="$path"
        fi
    fi
done

# Extract version number for directory
VERSION_DIR=$(echo "$LATEST_VERSION" | cut -d. -f1-2)
TARGET_DIR="$ADDONS_DIR/$VERSION_DIR/scripts/addons"

echo -e "${GREEN}✓ Using Blender $LATEST_VERSION${NC}"
echo -e "${GREEN}✓ Target directory: $TARGET_DIR${NC}"

# Create target directory
mkdir -p "$TARGET_DIR"

# Copy add-on files
echo -e "${BLUE}📦 Installing AthenaMist AI Agent...${NC}"

# Get the script directory (where the DMG contents are)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
RESOURCES_DIR="$(dirname "$SCRIPT_DIR")/Resources"

# Copy the add-on
cp -R "$RESOURCES_DIR/ai_agent" "$TARGET_DIR/"

echo -e "${GREEN}✓ Files copied successfully!${NC}"

# Create desktop shortcut
echo -e "${BLUE}🔗 Creating desktop shortcut...${NC}"
cat > "$HOME/Desktop/AthenaMist AI Agent.command" << 'SHORTCUT'
#!/bin/bash
echo "🎉 AthenaMist AI Agent installed successfully!"
echo ""
echo "To enable the add-on in Blender:"
echo "1. Open Blender"
echo "2. Go to Edit > Preferences > Add-ons"
echo "3. Search for 'AthenaMist'"
echo "4. Enable the add-on"
echo "5. Open the 3D Viewport sidebar (N key)"
echo "6. Look for the 'AthenaMist' tab"
echo ""
echo "Enjoy your AI-powered 3D creation! 🚀"
echo ""
read -p "Press Enter to close..."
SHORTCUT

chmod +x "$HOME/Desktop/AthenaMist AI Agent.command"

echo -e "${GREEN}✓ Desktop shortcut created!${NC}"

# Show success message
echo ""
echo -e "${GREEN}🎉 Installation completed successfully!${NC}"
echo ""
echo -e "${CYAN}Next steps:${NC}"
echo "1. Open Blender"
echo "2. Go to Edit > Preferences > Add-ons"
echo "3. Search for 'AthenaMist'"
echo "4. Enable the add-on"
echo "5. Open the 3D Viewport sidebar (N key)"
echo "6. Look for the 'AthenaMist' tab"
echo ""
echo -e "${PURPLE}Enjoy your AI-powered 3D creation! 🚀${NC}"
echo ""
read -p "Press Enter to exit..."

exit 0
EOF

# Make installer executable
chmod +x "${MACOS_DIR}/AthenaMistInstaller"

# Copy add-on files to resources
echo -e "${BLUE}📦 Copying add-on files...${NC}"
cp -R "../source/blender/ai_agent" "${RESOURCES_DIR}/"

# Copy icon to resources
echo -e "${BLUE}🎨 Copying icon...${NC}"
cp "icon.icns" "${RESOURCES_DIR}/ai_agent/"

# Create background image for DMG
echo -e "${BLUE}🎨 Creating DMG background...${NC}"
mkdir -p "${TEMP_DIR}/.background"

# Create a simple background image using ImageMagick if available
if command -v magick &> /dev/null; then
    magick -size 600x400 \
        gradient:'#667eea-#764ba2' \
        -fill white \
        -gravity center \
        -pointsize 24 \
        -annotate 0 "AthenaMist AI Agent\n\nDrag to Applications to install" \
        "${TEMP_DIR}/.background/background.png"
elif command -v convert &> /dev/null; then
    convert -size 600x400 \
        gradient:'#667eea-#764ba2' \
        -fill white \
        -gravity center \
        -pointsize 24 \
        -annotate 0 "AthenaMist AI Agent\n\nDrag to Applications to install" \
        "${TEMP_DIR}/.background/background.png"
else
    echo -e "${YELLOW}⚠️  ImageMagick not found. Using default background.${NC}"
    # Create a simple text file as placeholder
    echo "AthenaMist AI Agent - Drag to Applications to install" > "${TEMP_DIR}/.background/background.txt"
fi

# Create Applications folder link
echo -e "${BLUE}🔗 Creating Applications folder link...${NC}"
ln -s /Applications "${TEMP_DIR}/Applications"

# Create DMG
echo -e "${BLUE}📦 Creating DMG file...${NC}"
create-dmg \
    --volname "${APP_NAME}" \
    --volicon "${RESOURCES_DIR}/ai_agent/icon.icns" \
    --background "${TEMP_DIR}/.background/background.png" \
    --window-pos 200 120 \
    --window-size 600 400 \
    --icon-size 100 \
    --icon "${APP_NAME}.app" 175 120 \
    --hide-extension "${APP_NAME}.app" \
    --app-drop-link 425 120 \
    --no-internet-enable \
    "${DMG_NAME}" \
    "${TEMP_DIR}/"

# Clean up
echo -e "${BLUE}🧹 Cleaning up temporary files...${NC}"
rm -rf "${TEMP_DIR}"

# Show results
echo ""
echo -e "${GREEN}🎉 DMG created successfully!${NC}"
echo -e "${CYAN}File: ${DMG_NAME}${NC}"
echo -e "${CYAN}Size: $(du -h "${DMG_NAME}" | cut -f1)${NC}"
echo ""
echo -e "${PURPLE}🚀 Ready for testing!${NC}"
echo -e "${YELLOW}You can now:${NC}"
echo "1. Double-click the DMG to mount it"
echo "2. Drag the app to Applications"
echo "3. Run the installer"
echo "4. Test the AthenaMist AI Agent in Blender"
echo ""

# Optional: Open the DMG
read -p "Would you like to open the DMG now? (y/n): " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    open "${DMG_NAME}"
fi

echo -e "${GREEN}✨ All done! Happy testing! 🎮${NC}" 