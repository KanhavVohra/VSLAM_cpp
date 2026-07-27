#include "Window.h"
#include "camera.h"
#include "mono.h"

#include "imgui.h"

#include <GL/glew.h>

#include <opencv2/opencv.hpp>

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <exception>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

namespace
{
struct OpenGLTexture
{
    GLuint id = 0;
    int width = 0;
    int height = 0;
};

enum class CameraControlMode
{
    None,
    Mono,
    Stereo
};

bool isWebcamIndex(const std::string& source)
{
    if (source.empty())
    {
        return false;
    }

    return std::all_of(
        source.begin(),
        source.end(),
        [](unsigned char character)
        {
            return std::isdigit(character) != 0;
        }
    );
}

void destroyTexture(OpenGLTexture& texture)
{
    if (texture.id != 0)
    {
        glDeleteTextures(1, &texture.id);
    }

    texture.id = 0;
    texture.width = 0;
    texture.height = 0;
}

bool updateTextureFromFrame(
    OpenGLTexture& texture,
    const cv::Mat& inputFrame
)
{
    if (inputFrame.empty())
    {
        return false;
    }

    cv::Mat rgbaFrame;

    if (inputFrame.channels() == 3)
    {
        cv::cvtColor(
            inputFrame,
            rgbaFrame,
            cv::COLOR_BGR2RGBA
        );
    }
    else if (inputFrame.channels() == 4)
    {
        cv::cvtColor(
            inputFrame,
            rgbaFrame,
            cv::COLOR_BGRA2RGBA
        );
    }
    else if (inputFrame.channels() == 1)
    {
        cv::cvtColor(
            inputFrame,
            rgbaFrame,
            cv::COLOR_GRAY2RGBA
        );
    }
    else
    {
        return false;
    }

    if (texture.id == 0)
    {
        glGenTextures(1, &texture.id);

        glBindTexture(
            GL_TEXTURE_2D,
            texture.id
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MIN_FILTER,
            GL_LINEAR
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MAG_FILTER,
            GL_LINEAR
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_WRAP_S,
            GL_CLAMP_TO_EDGE
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_WRAP_T,
            GL_CLAMP_TO_EDGE
        );
    }
    else
    {
        glBindTexture(
            GL_TEXTURE_2D,
            texture.id
        );
    }

    glPixelStorei(
        GL_UNPACK_ALIGNMENT,
        1
    );

    const bool sizeChanged =
        texture.width != rgbaFrame.cols ||
        texture.height != rgbaFrame.rows;

    if (sizeChanged)
    {
        texture.width = rgbaFrame.cols;
        texture.height = rgbaFrame.rows;

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA8,
            texture.width,
            texture.height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            rgbaFrame.data
        );
    }
    else
    {
        glTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            0,
            0,
            texture.width,
            texture.height,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            rgbaFrame.data
        );
    }

    glBindTexture(
        GL_TEXTURE_2D,
        0
    );

    return true;
}

ImVec2 calculateImageSize(
    int imageWidth,
    int imageHeight,
    const ImVec2& availableSpace
)
{
    if (
        imageWidth <= 0 ||
        imageHeight <= 0 ||
        availableSpace.x <= 0.0F ||
        availableSpace.y <= 0.0F
    )
    {
        return ImVec2(0.0F, 0.0F);
    }

    const float widthScale =
        availableSpace.x /
        static_cast<float>(imageWidth);

    const float heightScale =
        availableSpace.y /
        static_cast<float>(imageHeight);

    const float scale =
        std::min(widthScale, heightScale);

    return ImVec2(
        static_cast<float>(imageWidth) * scale,
        static_cast<float>(imageHeight) * scale
    );
}
}

int main()
{
    try
    {
        Window window(
            1280,
            720,
            "VSLAM"
        );

        bool showDemoWindow = false;

        // ---------------------------------------------------------
        // Application state
        // ---------------------------------------------------------

        Camera cameraUrlHandler;
        CameraConfig monoConfig;

        std::unique_ptr<MonoCamera> monoCamera;

        OpenGLTexture processedTexture;

        CameraControlMode cameraControlMode =
            CameraControlMode::None;

        bool slamRunning = false;

        std::string trackingStatus = "Waiting";
        std::string connectedCameraUrl;

        int trackedFeatures = 0;
        int mapPoints = 0;

        // ---------------------------------------------------------
        // Camera URL input buffers
        // ---------------------------------------------------------

        std::array<char, 256> monoCameraUrl{};
        std::array<char, 256> leftCameraUrl{};
        std::array<char, 256> rightCameraUrl{};

        // ---------------------------------------------------------
        // Fixed layout dimensions
        // ---------------------------------------------------------

        const float TopBarHeight = 30.0F;
        const float sidebarWidth = 350.0F;

        const float processedFrameWidth = 350.0F;
        const float processedFrameHeight = 300.0F;

        while (!window.ShouldClose())
        {
            window.BeginFrame();

            ImGuiViewport* viewport =
                ImGui::GetMainViewport();

            // -----------------------------------------------------
            // Capture and process the current camera frame
            // -----------------------------------------------------

            if (monoCamera && slamRunning)
            {
                if (monoCamera->grabFrame())
                {
                    monoCamera->processFrame();

                    if (monoCamera->hasFrame())
                    {
                        const cv::Mat& processedFrame =
                            monoCamera->processedFrame();

                        if (
                            updateTextureFromFrame(
                                processedTexture,
                                processedFrame
                            )
                        )
                        {
                            trackedFeatures =
                                monoCamera->descriptors().rows;

                            trackingStatus = "Tracking";
                        }
                        else
                        {
                            trackingStatus =
                                "Failed to create camera texture";
                        }
                    }
                    else
                    {
                        trackingStatus =
                            "Processed frame is empty";
                    }
                }
                else
                {
                    trackingStatus =
                        "Failed to read camera frame";

                    slamRunning = false;
                }
            }

            if (showDemoWindow)
            {
                ImGui::ShowDemoWindow(
                    &showDemoWindow
                );
            }

            // =====================================================
            // Top toolbar
            // =====================================================

            ImGui::SetNextWindowPos(
                viewport->WorkPos,
                ImGuiCond_Always
            );

            ImGui::SetNextWindowSize(
                ImVec2(
                    viewport->WorkSize.x,
                    TopBarHeight
                ),
                ImGuiCond_Always
            );

            const ImGuiWindowFlags topBarFlags =
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoScrollbar |
                ImGuiWindowFlags_NoScrollWithMouse |
                ImGuiWindowFlags_NoSavedSettings;

            ImGui::Begin(
                "Top Toolbar",
                nullptr,
                topBarFlags
            );

            if (ImGui::Button("Start SLAM"))
            {
                if (monoCamera)
                {
                    slamRunning = true;
                    trackingStatus = "Starting";
                }
                else
                {
                    trackingStatus =
                        "Connect a mono camera first";
                }
            }

            ImGui::SameLine();

            if (ImGui::Button("Stop SLAM"))
            {
                slamRunning = false;

                if (monoCamera)
                {
                    trackingStatus = "Stopped";
                }
                else
                {
                    trackingStatus = "Waiting";
                }
            }

            ImGui::SameLine();

            if (ImGui::Button("Exit"))
            {
                window.Close();
            }

            ImGui::End();

            // =====================================================
            // Processed frame
            // =====================================================

            const float processedFrameX =
                viewport->WorkPos.x;

            const float processedFrameY =
                viewport->WorkPos.y +
                TopBarHeight;

            ImGui::SetNextWindowPos(
                ImVec2(
                    processedFrameX,
                    processedFrameY
                ),
                ImGuiCond_Always
            );

            ImGui::SetNextWindowSize(
                ImVec2(
                    processedFrameWidth,
                    processedFrameHeight
                ),
                ImGuiCond_Always
            );

            const ImGuiWindowFlags processedFrameFlags =
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoScrollbar |
                ImGuiWindowFlags_NoScrollWithMouse |
                ImGuiWindowFlags_NoSavedSettings;

            ImGui::Begin(
                "Processed Frame",
                nullptr,
                processedFrameFlags
            );

            if (processedTexture.id != 0)
            {
                const ImVec2 availableSpace =
                    ImGui::GetContentRegionAvail();

                const ImVec2 imageSize =
                    calculateImageSize(
                        processedTexture.width,
                        processedTexture.height,
                        availableSpace
                    );

                const float horizontalOffset =
                    std::max(
                        (
                            availableSpace.x -
                            imageSize.x
                        ) * 0.5F,
                        0.0F
                    );

                const float verticalOffset =
                    std::max(
                        (
                            availableSpace.y -
                            imageSize.y
                        ) * 0.5F,
                        0.0F
                    );

                ImGui::SetCursorPosX(
                    ImGui::GetCursorPosX() +
                    horizontalOffset
                );

                ImGui::SetCursorPosY(
                    ImGui::GetCursorPosY() +
                    verticalOffset
                );

                ImGui::Image(
                    (ImTextureID)(
                        intptr_t
                    )processedTexture.id,
                    imageSize,
                    ImVec2(0.0F, 0.0F),
                    ImVec2(1.0F, 1.0F)
                );
            }
            else
            {
                ImGui::TextWrapped(
                    "Waiting for camera frame..."
                );

                ImGui::Spacing();

                ImGui::TextWrapped(
                    "Connect a camera and press Start SLAM."
                );
            }

            ImGui::End();

            // =====================================================
            // VSLAM controls
            // =====================================================

            const float controlsX =
                viewport->WorkPos.x;

            const float controlsY =
                viewport->WorkPos.y +
                TopBarHeight +
                processedFrameHeight;

            const float availableControlsHeight =
                viewport->WorkSize.y -
                TopBarHeight -
                processedFrameHeight;

            const float controlsHeight =
                std::max(
                    availableControlsHeight,
                    1.0F
                );

            ImGui::SetNextWindowPos(
                ImVec2(
                    controlsX,
                    controlsY
                ),
                ImGuiCond_Always
            );

            ImGui::SetNextWindowSize(
                ImVec2(
                    sidebarWidth,
                    controlsHeight
                ),
                ImGuiCond_Always
            );

            const ImGuiWindowFlags controlsFlags =
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoSavedSettings;

            ImGui::Begin(
                "VSLAM Controls",
                nullptr,
                controlsFlags
            );

            // -----------------------------------------------------
            // Camera mode selection
            // -----------------------------------------------------

            if (ImGui::Button("MONO"))
            {
                cameraControlMode =
                    CameraControlMode::Mono;
            }

            ImGui::SameLine();

            if (ImGui::Button("STEREO"))
            {
                cameraControlMode =
                    CameraControlMode::Stereo;
            }

            ImGui::SameLine();

            if (ImGui::Button("Close Controls"))
            {
                cameraControlMode =
                    CameraControlMode::None;
            }

            // -----------------------------------------------------
            // Mono camera controls
            // -----------------------------------------------------

            if (
                cameraControlMode ==
                CameraControlMode::Mono
            )
            {
                ImGui::Separator();
                ImGui::TextUnformatted("Mono Camera");
                ImGui::Separator();

                ImGui::TextUnformatted(
                    "Camera Source"
                );

                ImGui::SetNextItemWidth(-1.0F);

                ImGui::InputText(
                    "##MonoCameraURL",
                    monoCameraUrl.data(),
                    monoCameraUrl.size()
                );

                if (
                    ImGui::Button(
                        "Connect Mono Camera",
                        ImVec2(-1.0F, 0.0F)
                    )
                )
                {
                    try
                    {
                        const std::string inputUrl(
                            monoCameraUrl.data()
                        );

                        const std::string normalizedUrl =
                            cameraUrlHandler.cameraURL(
                                inputUrl
                            );

                        if (normalizedUrl.empty())
                        {
                            throw std::invalid_argument(
                                "Camera source cannot be empty"
                            );
                        }

                        const int framesToDrop =
                            isWebcamIndex(normalizedUrl)
                                ? 0
                                : monoConfig.framesToDrop;

                        auto newMonoCamera =
                            std::make_unique<MonoCamera>(
                                normalizedUrl,
                                monoConfig.width,
                                monoConfig.height,
                                monoConfig.nfeatures,
                                framesToDrop
                            );

                        // Only replace the old camera after the
                        // new camera has connected successfully.
                        monoCamera =
                            std::move(newMonoCamera);

                        destroyTexture(
                            processedTexture
                        );

                        connectedCameraUrl =
                            normalizedUrl;

                        slamRunning = false;
                        trackedFeatures = 0;
                        mapPoints = 0;

                        trackingStatus =
                            "Camera connected";

                        std::cout
                            << "Mono camera connected: "
                            << normalizedUrl
                            << '\n';
                    }
                    catch (
                        const std::exception& exception
                    )
                    {
                        trackingStatus =
                            std::string(
                                "Connection failed: "
                            ) +
                            exception.what();

                        std::cerr
                            << trackingStatus
                            << '\n';
                    }
                }

                if (monoCamera)
                {
                    if (
                        ImGui::Button(
                            "Disconnect Mono Camera",
                            ImVec2(-1.0F, 0.0F)
                        )
                    )
                    {
                        slamRunning = false;

                        monoCamera.reset();

                        destroyTexture(
                            processedTexture
                        );

                        connectedCameraUrl.clear();

                        trackedFeatures = 0;
                        mapPoints = 0;

                        trackingStatus =
                            "Camera disconnected";
                    }
                }
            }

            // -----------------------------------------------------
            // Stereo camera controls
            // -----------------------------------------------------

            if (
                cameraControlMode ==
                CameraControlMode::Stereo
            )
            {
                ImGui::Separator();
                ImGui::TextUnformatted(
                    "Stereo Camera"
                );
                ImGui::Separator();

                ImGui::TextUnformatted(
                    "Left Camera Source"
                );

                ImGui::SetNextItemWidth(-1.0F);

                ImGui::InputText(
                    "##LeftCameraURL",
                    leftCameraUrl.data(),
                    leftCameraUrl.size()
                );

                ImGui::TextUnformatted(
                    "Right Camera Source"
                );

                ImGui::SetNextItemWidth(-1.0F);

                ImGui::InputText(
                    "##RightCameraURL",
                    rightCameraUrl.data(),
                    rightCameraUrl.size()
                );

                if (
                    ImGui::Button(
                        "Connect Stereo Cameras",
                        ImVec2(-1.0F, 0.0F)
                    )
                )
                {
                    const std::string normalizedLeftUrl =
                        cameraUrlHandler.cameraURL(
                            leftCameraUrl.data()
                        );

                    const std::string normalizedRightUrl =
                        cameraUrlHandler.cameraURL(
                            rightCameraUrl.data()
                        );

                    std::cout
                        << "Left camera: "
                        << normalizedLeftUrl
                        << '\n';

                    std::cout
                        << "Right camera: "
                        << normalizedRightUrl
                        << '\n';

                    trackingStatus =
                        "Stereo camera is not implemented yet";
                }
            }

            // -----------------------------------------------------
            // Tracking information
            // -----------------------------------------------------

            ImGui::Separator();

            ImGui::TextUnformatted(
                "Tracking Status"
            );

            ImGui::TextWrapped(
                "Status: %s",
                trackingStatus.c_str()
            );

            ImGui::Text(
                "Camera: %s",
                monoCamera
                    ? "Connected"
                    : "Disconnected"
            );

            ImGui::Text(
                "SLAM: %s",
                slamRunning
                    ? "Running"
                    : "Stopped"
            );

            ImGui::Text(
                "Tracked Features: %d",
                trackedFeatures
            );

            ImGui::Text(
                "Map Points: %d",
                mapPoints
            );

            if (!connectedCameraUrl.empty())
            {
                ImGui::Separator();

                ImGui::TextUnformatted(
                    "Connected Source:"
                );

                ImGui::TextWrapped(
                    "%s",
                    connectedCameraUrl.c_str()
                );
            }

            ImGui::End();

            window.EndFrame();
        }

        // The OpenGL context still exists here, so delete the
        // texture before the Window object is destroyed.
        destroyTexture(processedTexture);

        monoCamera.reset();
    }
    catch (const std::exception& exception)
    {
        std::cerr
            << "Application error: "
            << exception.what()
            << '\n';

        return 1;
    }

    return 0;
}