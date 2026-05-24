cmake_minimum_required(VERSION 3.10)

# Set name of project
project(ray)

# Preprocessor definitions
add_compile_definitions(
    DATRAW_DISABLE_NEON  # disable an ARM-specific hardware optimization
)

# Set all .cpp files to be source files
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# Set up the executable (./cmake-build-<profile>/ray)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Find OpenMP package on the machine and tell the linker to use it.
# Uncomment and remove -fopenmp below if you cannot get OpenMP to work.
find_package(OpenMP REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_CXX)

# Set compilation options for debugging (slow) and release (fast)
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    --std=c++17
    $<$<CONFIG:Debug>:   -O0 -g>
    $<$<CONFIG:Release>: -O3 -fopenmp>
)
