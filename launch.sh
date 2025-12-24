#!/bin/bash

# Constants
SDL_PATH="libs/SDL"
BUILD_DIR="build"
EXE_NAME="McGB"

# Text Formating
RESET='\033[0m'
RED='\033[0;31m'
GOLD='\033[1;33m'
UNDERLINE='\033[4m'


# 1. Check if SDL exists
if [ ! -d "$SDL_PATH" ]; then
    echo -e "\n${RED}Error${RESET}: SDL3 source not found in '$SDL_PATH'."
    echo -e "Please run: ${UNDERLINE}git clone https://github.com/libsdl-org/SDL $SDL_PATH${RESET}\n"
    exit 1
fi

# 2. Configure (Only if build folder is missing)
if [ ! -d "$BUILD_DIR" ]; then
    echo -e "${GOLD}${GOLD}Launch.sh${RESET}${RESET}: Creating build files..."
    cmake -S . -B "$BUILD_DIR"
fi

# 3. Compile (This is the only part that needs to run every time)
echo -e "${GOLD}Launch.sh${RESET}: Compiling..."
cmake --build "$BUILD_DIR"

# 4. Check if compile succeeded before launching
if [ $? -eq 0 ]; then
    echo -e "${GOLD}Launch.sh${RESET}: Launching $EXE_NAME..."
    ./"$BUILD_DIR"/"$EXE_NAME" "$@"     # $@ to allow passage of files for example if I wanna run tetris.gb
else
    echo -e "${GOLD}Launch.sh${RESET}: Compilation failed. Fix errors above."
    exit 1
fi