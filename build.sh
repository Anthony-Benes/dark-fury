#!/usr/bin/bash

# Cross-platform setup
is_windows() {
    [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]
}
is_linux() {
    [[ "$OSTYPE" == "linux-gnu" ]]
}

# Get the absolute path of the current script directory
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

# If running on Windows, use premake5.bat (on Linux, we use premake5 directly)
if is_windows; then
    premake_cmd="$SCRIPT_DIR/scripts/premake5_win.exe --file=$SCRIPT_DIR/scripts/build.lua"
elif is_linux; then
    premake_cmd="$SCRIPT_DIR/scripts/premake5_lin --file=$SCRIPT_DIR/scripts/build.lua"
else
    echo "Unsupported OS."
    exit 1
fi

premake_executable=$(echo $premake_cmd | awk '{print $1}')

# Check if the platform-specific Premake executable exists
if [ ! -f "$premake_executable" ]; then
    echo "Error: Premake executable not found at $premake_executable"
    exit 1
fi

echo "Generating project files..."
$premake_cmd gmake

if [ $? -eq 0 ]; then
    echo "Project files generated successfully."
else
    echo "Error generating project files."
    exit 1
fi
