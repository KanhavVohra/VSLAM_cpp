#include "mono.h"
#include "PangolinViewer.h"

#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>
#include <cctype>
#include <memory>
#include <chrono>
#include <thread>

std::string fixCameraUrl(const std::string& inputUrl) {
    if (inputUrl.empty()) {
        return inputUrl;
    }

    // Webcam index like "0"
    if (inputUrl.length() == 1 &&
        std::isdigit(static_cast<unsigned char>(inputUrl[0]))) {
        return inputUrl;
    }

    bool hasProtocol =
        inputUrl.rfind("http://", 0) == 0 ||
        inputUrl.rfind("https://", 0) == 0 ||
        inputUrl.rfind("rtsp://", 0) == 0;

    std::string url = hasProtocol ? inputUrl : "http://" + inputUrl;

    std::size_t protocolEnd = url.find("://");
    std::size_t searchStart = protocolEnd == std::string::npos ? 0 : protocolEnd + 3;
    std::size_t pathStart = url.find('/', searchStart);

    // If user typed only host:port, add /video
    if (pathStart == std::string::npos) {
        url += "/video";
    }

    return url;
}

int main() {
    cv::setUseOptimized(true);

    const int windowWidth = 640;
    const int windowHeight = 480;

    PangolinViewer viewer(windowWidth, windowHeight);
    std::unique_ptr<MonoCamera> monoCamera;

    const cv::Mat blankFrame = cv::Mat::zeros(windowHeight, windowWidth, CV_8UC3);
    const auto showBlank = [&](){ viewer.show(blankFrame, blankFrame); };

    std::cout << "Enter camera URL in Pangolin menu and press Connect.\n";

    while (!viewer.shouldQuit()) {
        const auto loopStart = std::chrono::steady_clock::now();

        if (viewer.connectPressed()) {
            const std::string rawUrl = viewer.getCameraUrl();
            const std::string url = fixCameraUrl(rawUrl);

            std::cout << "Raw URL: [" << rawUrl << "]\n";
            std::cout << "Final URL: [" << url << "]\n";

            if (url.empty()) {
                std::cerr << "Please enter camera URL first.\n";
                monoCamera.reset();
            } else {
                try {
                    monoCamera = std::make_unique<MonoCamera>(
                        url,
                        windowWidth,
                        windowHeight,
                        500,
                        3
                    );
                    std::cout << "Camera connected successfully.\n";
                } catch (const std::exception& e) {
                    std::cerr << "Camera error: " << e.what() << "\n";
                    monoCamera.reset();
                }
            }
        }

        if (viewer.disconnectPressed()) {
            monoCamera.reset();
            std::cout << "Camera disconnected.\n";
        }

        if (monoCamera && monoCamera->grabFrame()) {
            monoCamera->processFrame();

            if (monoCamera->hasFrame()) {
                viewer.show(monoCamera->frame(), monoCamera->processedFrame());
            } else {
                showBlank();
            }
        } else {
            showBlank();
        }

        const auto loopEnd = std::chrono::steady_clock::now();
        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(loopEnd - loopStart);
        constexpr int targetFrameTimeMs = 33;

        if (elapsed.count() < targetFrameTimeMs) {
            std::this_thread::sleep_for(std::chrono::milliseconds(targetFrameTimeMs - elapsed.count()));
        }
    }

    return 0;
}