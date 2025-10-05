#!/bin/bash
# Create a stable release of the voxel engine

VERSION_FILE="VERSION"
BUILD_DIR="build"
RELEASE_DIR="stable_releases"

# Read version from VERSION file
if [ ! -f "$VERSION_FILE" ]; then
    echo "Error: VERSION file not found"
    exit 1
fi

VERSION=$(cat "$VERSION_FILE" | tr -d '\n\r')

if [ -z "$VERSION" ]; then
    echo "Error: Could not read version from $VERSION_FILE"
    exit 1
fi

echo "Creating release version $VERSION..."

SRC_BIN_DIR="$BUILD_DIR/bin"

# Create versioned directory
RELEASE_PATH="$RELEASE_DIR/voxel_engine_$VERSION"
mkdir -p "$RELEASE_PATH"

# Copy entire compiled bin contents (exe, config, assets, etc.)
if [ -d "$SRC_BIN_DIR" ]; then
    echo "Copying files from $SRC_BIN_DIR to $RELEASE_PATH ..."
    cp -R "$SRC_BIN_DIR/." "$RELEASE_PATH/"
else
    echo "Error: Source bin directory not found: $SRC_BIN_DIR"
    exit 1
fi

# Include metadata files at release root
cp "VERSION" "$RELEASE_PATH/VERSION"
cp "README.md" "$RELEASE_PATH/README.md"

# Copy default configs to release root (for first-run)
if [ -d "default_config" ]; then
  mkdir -p "$RELEASE_PATH/default_config"
  cp -R "default_config/." "$RELEASE_PATH/default_config/"
fi

# Ensure legacy path expected by ConfigManager exists: build/bin/Release/config
if [ -d "$RELEASE_PATH/config" ]; then
  mkdir -p "$RELEASE_PATH/build/bin/Release/config"
  cp -R "$RELEASE_PATH/config/." "$RELEASE_PATH/build/bin/Release/config/"
fi

# Create a simple run script
cat > "$RELEASE_PATH/run.sh" << EOF
#!/bin/bash
echo "Starting Voxel Engine $VERSION..."
export DISPLAY=:0.0
export LIBGL_ALWAYS_INDIRECT=1
./voxel_app
EOF
chmod +x "$RELEASE_PATH/run.sh"

echo "Release created at: $RELEASE_PATH"
echo ""
echo "To run: cd \"$RELEASE_PATH\" && ./run.sh"
echo ""
echo "Version: $VERSION"
echo "Executable: voxel_app"
echo "Config: engine.ini"
