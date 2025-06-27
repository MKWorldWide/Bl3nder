#!/bin/bash

# Create lib directory if it doesn't exist
mkdir -p lib/macos_arm64

# Download pre-compiled libraries
cd lib/macos_arm64

# Download libraries from Blender's buildbot
curl -L -o libs.tar.gz https://builder.blender.org/download/deps/Blender-4.1-macOS-arm64.tar.gz

# Extract libraries
tar -xzf libs.tar.gz

# Clean up
rm libs.tar.gz

cd ../.. 