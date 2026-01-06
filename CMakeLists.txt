cmake_minimum_required(VERSION 3.10)

# Name your project
project(GBEmulator)

# 1. Add Libraries
add_subdirectory(libs/SDL)
add_subdirectory(libs/spdlog)

# 2. Define your executable (The App)
# 1.1. Find all .cpp files in the src directory
set(SOURCES 
    src/main.cpp 
    src/bus.cpp
    src/CPU.h
)

# 1.2. Create the executable using that list
add_executable(McGB ${SOURCES})

# 3. Link Libraries to your App
# This connects the "wires" so your code can use Library functions. 
# $<$<BOOL:${MINGW}>:ws2_32> included for compiling in windows usinf MINWG
target_link_libraries(McGB PRIVATE SDL3::SDL3 $<$<BOOL:${MINGW}>:ws2_32>)
target_link_libraries(McGB PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
