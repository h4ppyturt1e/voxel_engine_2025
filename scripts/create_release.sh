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

# Create versioned directory
RELEASE_PATH="$RELEASE_DIR/voxel_engine_$VERSION"
mkdir -p "$RELEASE_PATH"

# Copy executable
if [ -f "$BUILD_DIR/bin/voxel_app" ]; then
    cp "$BUILD_DIR/bin/voxel_app" "$RELEASE_PATH/voxel_app"
    chmod +x "$RELEASE_PATH/voxel_app"
else
    echo "Error: Executable not found at $BUILD_DIR/bin/voxel_app"
    exit 1
fi

# Copy config files
cp "engine.ini" "$RELEASE_PATH/engine.ini"
cp "VERSION" "$RELEASE_PATH/VERSION"

# Copy README
cp "README.md" "$RELEASE_PATH/README.md"

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
