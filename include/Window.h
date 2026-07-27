#pragma once

#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <stdexcept>
struct GLFWwindow;

class Window
{
public:
    Window(
        int width,
        int height,
        const std::string& title
    );

    ~Window();

    // Prevent accidental copying.
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    // Prevent moving for now.
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    // Prepare OpenGL and ImGui for a new frame.
    void BeginFrame();

    // Render ImGui and display the completed frame.
    void EndFrame();

    // Returns true when the user closes the window.
    bool ShouldClose() const;

    // Request that the window close.
    void Close();

    // Access the underlying GLFW window when needed.
    GLFWwindow* GetNativeWindow() const;

    int GetWidth() const;
    int GetHeight() const;

private:
    bool InitializeGLFW();
    bool InitializeOpenGL();
    bool InitializeImGui();

    void Shutdown();

    static void GLFWErrorCallback(
        int errorCode,
        const char* description
    );

    static void FramebufferSizeCallback(
        GLFWwindow* window,
        int width,
        int height
    );

private:
    GLFWwindow* window_ = nullptr;

    int width_ = 0;
    int height_ = 0;

    std::string title_;

    bool glfwInitialized_ = false;
    bool imguiInitialized_ = false;
};

