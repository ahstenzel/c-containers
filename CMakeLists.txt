#====================================
# Build examples.
#====================================
cmake_minimum_required (VERSION 3.8)
project ("C_CONTAINER")

# Specify library output directory
set(OUTPUT_DIR "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}")

# Add source to this project's binary
file(GLOB_RECURSE libsrc "src/*.c" "examples/main.c")
add_executable(C_CONTAINER ${libsrc})
target_include_directories(C_CONTAINER PUBLIC "src")

# Enforce standards
target_compile_features(C_CONTAINER PUBLIC c_std_11)