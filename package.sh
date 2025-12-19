#!/bin/bash
# Distribution packaging script for Warehouse & Delivery Management System
# This creates a portable Linux package

APP_NAME="WarehouseDeliverySystem"
DIST_DIR="dist"
LIB_DIR="$DIST_DIR/lib"

echo "=== Creating Distribution Package ==="

# Create directories
mkdir -p "$LIB_DIR"
mkdir -p "$DIST_DIR/platforms"
mkdir -p "$DIST_DIR/wayland"

# Find executable
EXE_PATH=""
if [ -f "build/Release/$APP_NAME" ]; then
    EXE_PATH="build/Release/$APP_NAME"
elif [ -f "build/Debug/$APP_NAME" ]; then
    EXE_PATH="build/Debug/$APP_NAME"
elif [ -f "build/$APP_NAME" ]; then
    EXE_PATH="build/$APP_NAME"
else
    echo "Error: Executable not found!"
    exit 1
fi

echo "Using executable: $EXE_PATH"
cp "$EXE_PATH" "$DIST_DIR/$APP_NAME"
chmod +x "$DIST_DIR/$APP_NAME"

# Copy Qt libraries
echo "Copying Qt libraries..."
for lib in $(ldd "$EXE_PATH" | grep -E "Qt|icu|xkb|xcb" | awk '{print $3}'); do
    if [ -f "$lib" ]; then
        cp "$lib" "$LIB_DIR/"
        echo "  Copied: $(basename $lib)"
    fi
done

# Copy Qt platform plugins
QT_PLUGIN_PATH="/usr/lib/x86_64-linux-gnu/qt6/plugins"
if [ -d "$QT_PLUGIN_PATH/platforms" ]; then
    cp "$QT_PLUGIN_PATH/platforms/libqxcb.so" "$DIST_DIR/platforms/" 2>/dev/null
    cp "$QT_PLUGIN_PATH/platforms/libqwayland-generic.so" "$DIST_DIR/platforms/" 2>/dev/null
    echo "  Copied platform plugins"
fi

# Copy wayland plugins if available
if [ -d "$QT_PLUGIN_PATH/wayland-shell-integration" ]; then
    cp -r "$QT_PLUGIN_PATH/wayland-shell-integration" "$DIST_DIR/" 2>/dev/null
fi

# Copy sample input file
cp input.txt "$DIST_DIR/"

# Create launcher script
cat > "$DIST_DIR/run.sh" << 'EOF'
#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
export LD_LIBRARY_PATH="$SCRIPT_DIR/lib:$LD_LIBRARY_PATH"
export QT_PLUGIN_PATH="$SCRIPT_DIR"
cd "$SCRIPT_DIR"
./WarehouseDeliverySystem "$@"
EOF
chmod +x "$DIST_DIR/run.sh"

# Create README
cat > "$DIST_DIR/README.txt" << 'EOF'
Warehouse & Delivery Management System
=======================================

HOW TO RUN:
-----------
On Linux: Double-click run.sh or run from terminal:
    ./run.sh

FEATURES:
---------
- Add orders via GUI (VIP or Standard)
- Load input files with predefined scenarios
- Step-by-step or continuous simulation
- Track order fulfillment, vehicle dispatch, inventory

TABS:
-----
📦 Orders    - Add/remove orders, view VIP and Standard queues
🏭 Warehouses - View inventory levels
🚚 Vehicles  - Monitor vehicle status

Built with Qt6 | Data Structures & Algorithms Project
EOF

# Create archive
echo ""
echo "Creating archive..."
cd dist
tar -czvf "../${APP_NAME}_Linux_x64.tar.gz" *
cd ..

echo ""
echo "=== Distribution package created ==="
echo "Package: ${APP_NAME}_Linux_x64.tar.gz"
echo "Extract and run: ./run.sh"
