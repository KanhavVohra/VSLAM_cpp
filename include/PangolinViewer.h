#ifndef PANGOLINVIEWER_HPP
#define PANGOLINVIEWER_HPP

#include <opencv2/opencv.hpp>
#include <pangolin/pangolin.h>

#include <memory>
#include <string>

class PangolinViewer {
private:
    int width;
    int height;

    pangolin::GlTexture normalTexture;
    pangolin::GlTexture orbTexture;

    pangolin::View* leftView;
    pangolin::View* rightView;

    std::unique_ptr<pangolin::Var<std::string>> cameraUrl;
    std::unique_ptr<pangolin::Var<bool>> connectButton;
    std::unique_ptr<pangolin::Var<bool>> disconnectButton;

public:
    PangolinViewer(int imageWidth, int imageHeight);

    bool shouldQuit() const;

    bool connectPressed();
    bool disconnectPressed();

    std::string getCameraUrl() const;

    void show(const cv::Mat& normalFrame, const cv::Mat& orbFrame);
};

#endif // PANGOLINVIEWER_HPP

