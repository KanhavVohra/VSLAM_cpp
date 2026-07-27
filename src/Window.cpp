#include "Window.h"



Window::Window(
    int width,
    int height,
    const std::string& title
)
    : width_(width),
      height_(height),
      title_(title)
{
    if (width_ <= 0 || height_ <= 0)
    {
        throw std::invalid_argument(
            "Window width and height must be greater than zero."
        );
    }

    if (!InitializeGLFW())
    {
        Shutdown();
        throw std::runtime_error("Failed to initialize GLFW.");
    }

    if (!InitializeOpenGL())
    {
        Shutdown();
        throw std::runtime_error("Failed to initialize OpenGL.");
    }

    if (!InitializeImGui())
    {
        Shutdown();
        throw std::runtime_error("Failed to initialize ImGui.");
    }

    std::cout
        << "Window created successfully: "
        << width_
        << "x"
        << height_
        << '\n';
}

Window::~Window()
{
    Shutdown();
}

bool Window::InitializeGLFW()
{
    glfwSetErrorCallback(GLFWErrorCallback);

    if (glfwInit() == GLFW_FALSE)
    {
        std::cerr << "glfwInit() failed.\n";
        return false;
    }

    glfwInitialized_ = true;

    // Request OpenGL 3.3 Core Profile.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );

#ifdef __APPLE__
    glfwWindowHint(
        GLFW_OPENGL_FORWARD_COMPAT,
        GL_TRUE
    );
#endif

    window_ = glfwCreateWindow(
        width_,
        height_,
        title_.c_str(),
        nullptr,
        nullptr
    );

    if (window_ == nullptr)
    {
        std::cerr << "glfwCreateWindow() failed.\n";
        return false;
    }

    glfwMakeContextCurrent(window_);

    // Enable VSync.
    glfwSwapInterval(1);

    // Store this object inside GLFW so callbacks can access it.
    glfwSetWindowUserPointer(window_, this);

    glfwSetFramebufferSizeCallback(
        window_,
        FramebufferSizeCallback
    );

    return true;
}

bool Window::InitializeOpenGL()
{
    // Required so GLEW works correctly with modern OpenGL.
    glewExperimental = GL_TRUE;

    const GLenum glewResult = glewInit();

    if (glewResult != GLEW_OK)
    {
        std::cerr
            << "glewInit() failed: "
            << glewGetErrorString(glewResult)
            << '\n';

        return false;
    }

    // GLEW can generate an invalid-enum error during initialization.
    // Clear it before normal OpenGL rendering begins.
    while (glGetError() != GL_NO_ERROR)
    {
    }

    glViewport(
        0,
        0,
        width_,
        height_
    );

    // Enable depth testing for future 3D map rendering.
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    std::cout
        << "OpenGL version: "
        << glGetString(GL_VERSION)
        << '\n';

    std::cout
        << "OpenGL renderer: "
        << glGetString(GL_RENDERER)
        << '\n';

    return true;
}

bool Window::InitializeImGui()
{
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();

    // Keyboard navigation through ImGui widgets.
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Allows ImGui panels to be docked.
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    const bool glfwBackendInitialized =
        ImGui_ImplGlfw_InitForOpenGL(
            window_,
            true
        );

    if (!glfwBackendInitialized)
    {
        std::cerr
            << "ImGui GLFW backend initialization failed.\n";

        ImGui::DestroyContext();
        return false;
    }

    constexpr const char* glslVersion =
        "#version 330 core";

    const bool openGLBackendInitialized =
        ImGui_ImplOpenGL3_Init(glslVersion);

    if (!openGLBackendInitialized)
    {
        std::cerr
            << "ImGui OpenGL backend initialization failed.\n";

        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        return false;
    }

    imguiInitialized_ = true;

    return true;
}

void Window::BeginFrame()
{
    if (window_ == nullptr)
    {
        return;
    }

    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();
}

void Window::EndFrame()
{
    if (window_ == nullptr)
    {
        return;
    }

    ImGui::Render();

    int framebufferWidth = 0;
    int framebufferHeight = 0;

    glfwGetFramebufferSize(
        window_,
        &framebufferWidth,
        &framebufferHeight
    );

    glViewport(
        0,
        0,
        framebufferWidth,
        framebufferHeight
    );

    // Background color.
    glClearColor(
        0.08F,
        0.09F,
        0.11F,
        1.0F
    );

    glClear(
        GL_COLOR_BUFFER_BIT |
        GL_DEPTH_BUFFER_BIT
    );

    ImGui_ImplOpenGL3_RenderDrawData(
        ImGui::GetDrawData()
    );

    glfwSwapBuffers(window_);
}

bool Window::ShouldClose() const
{
    if (window_ == nullptr)
    {
        return true;
    }

    return glfwWindowShouldClose(window_) == GLFW_TRUE;
}

void Window::Close()
{
    if (window_ != nullptr)
    {
        glfwSetWindowShouldClose(
            window_,
            GLFW_TRUE
        );
    }
}

GLFWwindow* Window::GetNativeWindow() const
{
    return window_;
}

int Window::GetWidth() const
{
    return width_;
}

int Window::GetHeight() const
{
    return height_;
}

void Window::Shutdown()
{
    if (imguiInitialized_)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();

        ImGui::DestroyContext();

        imguiInitialized_ = false;
    }

    if (window_ != nullptr)
    {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }

    if (glfwInitialized_)
    {
        glfwTerminate();
        glfwInitialized_ = false;
    }
}

void Window::GLFWErrorCallback(
    int errorCode,
    const char* description
)
{
    std::cerr
        << "GLFW error "
        << errorCode
        << ": "
        << description
        << '\n';
}

void Window::FramebufferSizeCallback(
    GLFWwindow* window,
    int width,
    int height
)
{
    glViewport(
        0,
        0,
        width,
        height
    );

    Window* windowObject =
        static_cast<Window*>(
            glfwGetWindowUserPointer(window)
        );

    if (windowObject != nullptr)
    {
        windowObject->width_ = width;
        windowObject->height_ = height;
    }
}