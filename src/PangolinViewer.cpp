#include "PangolinViewer.h"

#include <GL/gl.h>
#include <iostream>

PangolinViewer::PangolinViewer(int imageWidth, int imageHeight)
    : width(imageWidth),
      height(imageHeight),
      leftView(nullptr),
      rightView(nullptr),
      cameraUrl(nullptr),
      connectButton(nullptr),
      disconnectButton(nullptr) {

    // Create OpenGL context first
    pangolin::CreateWindowAndBind("IP Camera ORB Viewer", width * 2, height);

    // glDisable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_TEST);
    // Now initialize textures AFTER window/context exists
    normalTexture.Reinitialise(
        width,
        height,
        GL_RGB,
        false,
        0,
        GL_BGR,
        GL_UNSIGNED_BYTE
    );

    orbTexture.Reinitialise(
        width,
        height,
        GL_RGB,
        false,
        0,
        GL_BGR,
        GL_UNSIGNED_BYTE
    );

    // Menu panel on left side
    pangolin::CreatePanel("menu")
        .SetBounds(0.0, 1.0, 0.0, 0.25);

    cameraUrl = std::make_unique<pangolin::Var<std::string>>(
        "menu.Camera_URL",
        ""
    );

    connectButton = std::make_unique<pangolin::Var<bool>>(
        "menu.Connect",
        false,
        false
    );

    disconnectButton = std::make_unique<pangolin::Var<bool>>(
        "menu.Disconnect",
        false,
        false
    );

    // Normal view in middle
    leftView = &pangolin::Display("left")
        .SetBounds(0.0, 1.0, 0.25, 0.625);

    // ORB view on right
    rightView = &pangolin::Display("right")
        .SetBounds(0.0, 1.0, 0.625, 1.0);
}

bool PangolinViewer::shouldQuit() const {
    return pangolin::ShouldQuit();
}

bool PangolinViewer::connectPressed() {
    return pangolin::Pushed(*connectButton);
}

bool PangolinViewer::disconnectPressed() {
    return pangolin::Pushed(*disconnectButton);
}

std::string PangolinViewer::getCameraUrl() const {
    return cameraUrl->Get();
}

void PangolinViewer::show(const cv::Mat& normalFrame, const cv::Mat& orbFrame) {
    glClear(GL_COLOR_BUFFER_BIT);

    if (normalFrame.empty() || orbFrame.empty()) {
        pangolin::FinishFrame();
        return;
    }

    cv::Mat normalDisplay;
    cv::Mat orbDisplay;

    cv::resize(normalFrame, normalDisplay, cv::Size(width, height));
    cv::resize(orbFrame, orbDisplay, cv::Size(width, height));

    if (normalDisplay.channels() == 1) {
        cv::cvtColor(normalDisplay, normalDisplay, cv::COLOR_GRAY2BGR);
    }

    if (orbDisplay.channels() == 1) {
        cv::cvtColor(orbDisplay, orbDisplay, cv::COLOR_GRAY2BGR);
    }

    if (!normalDisplay.isContinuous()) {
        normalDisplay = normalDisplay.clone();
    }

    if (!orbDisplay.isContinuous()) {
        orbDisplay = orbDisplay.clone();
    }

    normalTexture.Upload(
        normalDisplay.data,
        GL_BGR,
        GL_UNSIGNED_BYTE
    );

    orbTexture.Upload(
        orbDisplay.data,
        GL_BGR,
        GL_UNSIGNED_BYTE
    );

    leftView->Activate();
    normalTexture.RenderToViewportFlipY();

    rightView->Activate();
    orbTexture.RenderToViewportFlipY();

    pangolin::FinishFrame();
}