#!/bin/bash

# AthenaMist Blender Build Script
# ===============================
# Creates a Blender-like application with AthenaMist AI Agent integrated

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m'

echo -e "${PURPLE}🚀 AthenaMist Blender Builder${NC}"
echo -e "${CYAN}==============================${NC}"
echo ""

# Configuration
APP_NAME="AthenaMist Blender"
VERSION="1.0.0"
BUILD_DIR="build_athenamist"
APP_DIR="${BUILD_DIR}/${APP_NAME}.app"
CONTENTS_DIR="${APP_DIR}/Contents"
MACOS_DIR="${CONTENTS_DIR}/MacOS"
RESOURCES_DIR="${CONTENTS_DIR}/Resources"
FRAMEWORKS_DIR="${CONTENTS_DIR}/Frameworks"

echo -e "${BLUE}🧹 Cleaning previous build...${NC}"
rm -rf "${BUILD_DIR}"

echo -e "${BLUE}📁 Creating directory structure...${NC}"
mkdir -p "${MACOS_DIR}"
mkdir -p "${RESOURCES_DIR}"
mkdir -p "${FRAMEWORKS_DIR}"

# Create Info.plist
echo -e "${BLUE}📝 Creating Info.plist...${NC}"
cat > "${CONTENTS_DIR}/Info.plist" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>AthenaMistBlender</string>
    <key>CFBundleIdentifier</key>
    <string>com.athenamist.blender</string>
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
            <string>3D Scene</string>
            <key>CFBundleTypeExtensions</key>
            <array>
                <string>blend</string>
            </array>
            <key>CFBundleTypeRole</key>
            <string>Editor</string>
        </dict>
    </array>
</dict>
</plist>
EOF

# Create the main executable
echo -e "${BLUE}🔧 Creating main executable...${NC}"
cat > "${MACOS_DIR}/AthenaMistBlender" << 'EOF'
#!/bin/bash

# AthenaMist Blender Launcher
# ===========================

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m'

echo -e "${PURPLE}🎉 Welcome to AthenaMist Blender!${NC}"
echo -e "${CYAN}====================================${NC}"
echo ""

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
RESOURCES_DIR="$(dirname "$SCRIPT_DIR")/Resources"

# Check if Blender is installed
BLENDER_PATHS=(
    "/Applications/Blender.app"
    "$HOME/Applications/Blender.app"
)

BLENDER_FOUND=false
for path in "${BLENDER_PATHS[@]}"; do
    if [[ -d "$path" ]]; then
        BLENDER_PATH="$path"
        BLENDER_FOUND=true
        echo -e "${GREEN}✓ Found Blender at: $path${NC}"
        break
    fi
done

if [[ "$BLENDER_FOUND" == false ]]; then
    echo -e "${RED}❌ Blender not found!${NC}"
    echo -e "${YELLOW}Please install Blender first from: https://www.blender.org/download/${NC}"
    echo ""
    echo -e "${BLUE}🔧 Installing AthenaMist AI Agent as standalone...${NC}"
    
    # Install AthenaMist as standalone
    STANDALONE_DIR="$HOME/AthenaMistBlender"
    mkdir -p "$STANDALONE_DIR"
    cp -R "$RESOURCES_DIR/athenamist_integration" "$STANDALONE_DIR/"
    
    echo -e "${GREEN}✓ AthenaMist installed to: $STANDALONE_DIR${NC}"
    echo ""
    echo -e "${CYAN}To run AthenaMist AI Agent:${NC}"
    echo "cd $STANDALONE_DIR"
    echo "python3 athenamist_integration/demo.py"
    echo ""
    read -p "Press Enter to continue..."
    exit 0
fi

# Launch Blender with AthenaMist
echo -e "${BLUE}🚀 Launching Blender with AthenaMist AI Agent...${NC}"

# Copy AthenaMist to Blender's add-ons directory
ADDONS_DIR="$HOME/Library/Application Support/Blender"
if [[ ! -d "$ADDONS_DIR" ]]; then
    mkdir -p "$ADDONS_DIR"
fi

# Find the most recent Blender version
LATEST_VERSION=""
for path in "${BLENDER_PATHS[@]}"; do
    if [[ -d "$path" ]]; then
        version=$(defaults read "$path/Contents/Info.plist" CFBundleShortVersionString 2>/dev/null || echo "0.0.0")
        if [[ "$version" > "$LATEST_VERSION" ]]; then
            LATEST_VERSION="$version"
        fi
    fi
done

VERSION_DIR=$(echo "$LATEST_VERSION" | cut -d. -f1-2)
TARGET_DIR="$ADDONS_DIR/$VERSION_DIR/scripts/addons"

echo -e "${GREEN}✓ Using Blender $LATEST_VERSION${NC}"
echo -e "${GREEN}✓ Target directory: $TARGET_DIR${NC}"

# Create target directory
mkdir -p "$TARGET_DIR"

# Copy AthenaMist
cp -R "$RESOURCES_DIR/athenamist_integration" "$TARGET_DIR/"

echo -e "${GREEN}✓ AthenaMist AI Agent installed!${NC}"

# Launch Blender
echo -e "${BLUE}🎬 Starting Blender...${NC}"
open "$BLENDER_PATH"

echo ""
echo -e "${GREEN}🎉 AthenaMist Blender is ready!${NC}"
echo ""
echo -e "${CYAN}In Blender:${NC}"
echo "1. Go to Edit > Preferences > Add-ons"
echo "2. Search for 'AthenaMist'"
echo "3. Enable the add-on"
echo "4. Open the 3D Viewport sidebar (N key)"
echo "5. Look for the 'AthenaMist' tab"
echo "6. Start chatting with your AI assistant! 🤖"
echo ""
echo -e "${PURPLE}Enjoy your AI-powered 3D creation! 🚀✨${NC}"

# Keep the window open
read -p "Press Enter to exit..."

exit 0
EOF

# Make executable
chmod +x "${MACOS_DIR}/AthenaMistBlender"

# Copy AthenaMist integration
echo -e "${BLUE}📦 Copying AthenaMist integration...${NC}"
cp -R "source/blender/athenamist_integration" "${RESOURCES_DIR}/"

# Copy icon
echo -e "${BLUE}🎨 Copying icon...${NC}"
cp "source/blender/athenamist_integration/icon.png" "${RESOURCES_DIR}/AppIcon.png"

# Create Applications folder link for DMG
echo -e "${BLUE}🔗 Creating Applications folder link...${NC}"
ln -s /Applications "${BUILD_DIR}/Applications"

# Create DMG
echo -e "${BLUE}📦 Creating DMG...${NC}"
DMG_NAME="AthenaMist_Blender_v${VERSION}_macOS.dmg"

if command -v create-dmg &> /dev/null; then
    create-dmg \
        --volname "${APP_NAME}" \
        --volicon "${RESOURCES_DIR}/AppIcon.png" \
        --window-pos 200 120 \
        --window-size 600 400 \
        --icon-size 100 \
        --icon "${APP_NAME}.app" 175 120 \
        --hide-extension "${APP_NAME}.app" \
        --app-drop-link 425 120 \
        --no-internet-enable \
        "${DMG_NAME}" \
        "${BUILD_DIR}/"
else
    echo -e "${YELLOW}⚠️  create-dmg not found. Creating app bundle only.${NC}"
fi

echo ""
echo -e "${GREEN}🎉 AthenaMist Blender created successfully!${NC}"
echo -e "${CYAN}App: ${BUILD_DIR}/${APP_NAME}.app${NC}"
if [[ -f "${DMG_NAME}" ]]; then
    echo -e "${CYAN}DMG: ${DMG_NAME}${NC}"
fi
echo ""
echo -e "${PURPLE}🚀 Ready to launch!${NC}"

# Optional: Open the app
read -p "Would you like to launch AthenaMist Blender now? (y/n): " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    open "${BUILD_DIR}/${APP_NAME}.app"
fi

echo -e "${GREEN}✨ All done! Happy creating! 🎮${NC}" 