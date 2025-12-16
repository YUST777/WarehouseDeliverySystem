# Warehouse & Delivery Management System

A C++ Qt6 GUI application for simulating warehouse operations, vehicle dispatch, and delivery tracking.

## Features

- 📦 **Order Management** - Add VIP/Standard orders via GUI or file
- 🏭 **Warehouse Inventory** - Real-time stock tracking
- 🚚 **Vehicle Dispatch** - Automatic assignment and routing
- ⏱️ **Event Simulation** - Step-by-step or continuous mode
- 🌙 **Dark Theme UI** - Modern, professional interface

## Quick Start

### Windows
1. Download `WarehouseDeliverySystem_Windows_x64.zip` from [Releases](../../releases)
2. Extract and run `WarehouseDeliverySystem.exe`

### Linux
1. Download `WarehouseDeliverySystem_Linux_x64.tar.gz` from [Releases](../../releases)
2. Extract and run `./run.sh`

### Build from Source

**Requirements:**
- Qt6 (6.5+)
- CMake 3.16+
- C++17 compiler

```bash
# Clone
git clone https://github.com/YOUR_USERNAME/WarehouseDeliverySystem.git
cd WarehouseDeliverySystem

# Build
mkdir build && cd build
cmake ..
make -j4

# Run
./WarehouseDeliverySystem
```

## Usage

1. **Add orders manually** using the input form in the Orders tab
2. Or **load a file** via `File → Open Input File...`
3. Click **Step** or **Run** to simulate
4. Watch orders get assigned, dispatched, and delivered!

## Screenshots

(Add screenshots here)

## Input File Format

See `input.txt` for an example. Format follows the DSA project specification.

## License

Educational use - DSA Project
