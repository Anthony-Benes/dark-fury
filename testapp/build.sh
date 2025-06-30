#!/bin/bash
# Build script for testapp
set echo on

mkdir -p ../bin

# Get a list of all the .c files
cFilenames=$(find . -type f -name "*.c")

# echo "Files:" $cFilenames

assembly="testapp"
compilerFlags="-g -fdeclspec -fPIC"
# -fms-extensions
# -Wall -Werror
includeFlags="-Isrc -I../engine/src/"
linkerFlags="-L../bin/ -lengine -Wl,-rpath,."
defines="-D_DEBUG -DDF_IMPORT"

echo "Building $assembly..."
echo clang $cFilenames $compilerFlags -o ../bin/$assembly $defines $includeFlags $linkerFlags
clang $cFilenames $compilerFlags -o ../bin/$assembly $defines $includeFlags $linkerFlags
