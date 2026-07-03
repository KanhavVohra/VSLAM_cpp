cmake_minimum_required(VERSION 3.22.1)

project(VSlam VERSION 0.1 LANGUAGES CXX)

# =====================
# C++ Standard
# =====================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =====================
# Dependencies
# =====================
find_package(OpenCV REQUIRED)
find_package(GLEW REQUIRED)
find_package(Pangolin REQUIRED)
# =====================
# Executable
# =====================

file(GLOB_RECURSE SLAM_SOURCES
    CONFIGURE_DEPENDS
    src/*.cpp
)

file(GLOB_RECURSE SLAM_HEADERS
    CONFIGURE_DEPENDS
    include/*.h
)

add_executable(VSlam
    main.cpp
    ${SLAM_SOURCES}
)





target_link_libraries(VSlam PRIVATE
    GLEW::GLEW
    OpenGL::GL
    ${Pangolin_LIBRARIES}
)

# =====================
# Include directories
# =====================
target_include_directories(VSlam
    PRIVATE
        ${OpenCV_INCLUDE_DIRS}
        include
)

# =====================
# Link libraries
# =====================
target_link_libraries(VSlam 
    PRIVATE
        ${OpenCV_LIBS}
)