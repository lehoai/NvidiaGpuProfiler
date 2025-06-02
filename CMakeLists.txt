cmake_minimum_required(VERSION 3.10)
project(GPUProfiler)

# Enable compile commands generation
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add debug flags
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -Wall -Wextra")

# Find wxWidgets package
find_package(wxWidgets REQUIRED COMPONENTS core base adv)
include(${wxWidgets_USE_FILE})

# Add source files
add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/MainFrame.cpp
    src/Nvidia.cpp
    src/TimelineChart.cpp
)

# Link wxWidgets libraries
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})

# link nvidia-ml library
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} nvidia-ml)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE include) 