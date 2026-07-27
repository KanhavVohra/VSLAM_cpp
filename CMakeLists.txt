cmake_minimum_required(VERSION 3.22.1)

project(VSlam VERSION 0.1 LANGUAGES CXX)

# =====================
# C++ Standard
# =====================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Helps VSCode IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# =====================
# Dependencies
# =====================
find_package(OpenCV REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

# =====================
# ImGui Paths
# =====================
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/extra/Imgui)

# =====================
# ImGui Library
# =====================
add_library(imgui
    ${IMGUI_DIR}/imgui_base/imgui.cpp
    ${IMGUI_DIR}/imgui_base/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_base/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_base/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_base/imgui_demo.cpp

    ${IMGUI_DIR}/backends_glfw_opengl/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends_glfw_opengl/imgui_impl_opengl3.cpp
)

target_include_directories(imgui
    PUBLIC
        ${IMGUI_DIR}/imgui_base
        ${IMGUI_DIR}/backends_glfw_opengl
)

target_link_libraries(imgui
    PUBLIC
        glfw
        OpenGL::GL
        ${CMAKE_DL_LIBS}
)

# =====================
# Source files
# =====================
file(GLOB_RECURSE SLAM_SOURCES
    CONFIGURE_DEPENDS
    src/*.cpp
)

file(GLOB_RECURSE SLAM_HEADERS
    CONFIGURE_DEPENDS
    include/*.h
)

# =====================
# Executable
# =====================
add_executable(VSlam
    main.cpp
    ${SLAM_SOURCES}
    ${SLAM_HEADERS}
)

# =====================
# Include directories
# =====================
target_include_directories(VSlam
    PRIVATE
        ${OpenCV_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/include
)

# =====================
# Link libraries
# =====================
target_link_libraries(VSlam
    PRIVATE
        ${OpenCV_LIBS}
        OpenGL::GL
        GLEW::GLEW
        glfw
        imgui
)