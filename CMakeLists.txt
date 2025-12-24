cmake_minimum_required(VERSION 3.10)

# Name your project
project(GBEmulator)

# 1. Add SDL3 from your local "libs" folder
# This looks inside "libs/SDL" and builds the library for you.
add_subdirectory(libs/SDL)

# 2. Define your executable (The App)
# It tells CMake that "src/main.cpp" is your main code file.
add_executable(McGB src/main.cpp)

# 3. Link SDL3 to your App
# This connects the "wires" so your code can use SDL functions.
target_link_libraries(McGB PRIVATE SDL3::SDL3)
