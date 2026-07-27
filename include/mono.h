#ifndef MONO_H
#define MONO_H

#include "orb_cv.h"

#include <opencv2/opencv.hpp>

#include <string>
#include <vector>
#include <cctype>

class MonoCamera {
public:
    MonoCamera(
        const std::string& url,
        int width = 640,
        int height = 480,
        int nfeatures = 500,
        int framesToDrop = 3
    );

    bool grabFrame();
    void processFrame();

    bool hasFrame() const;

    const cv::Mat& frame() const;
    const cv::Mat& processedFrame() const;
    const cv::Mat& descriptors() const;

private:
    bool isWebcamIndex(const std::string& url) const;
    bool readLatestFrame();

private:
    cv::VideoCapture cap_;

    ORBExtractor orb_;

    int width_;
    int height_;
    int framesToDrop_;

    cv::Mat rawFrame_;
    cv::Mat frame_;
    cv::Mat gray_;
    cv::Mat output_;
    cv::Mat descriptors_;

    std::vector<cv::KeyPoint> keypoints_;
};

#endif