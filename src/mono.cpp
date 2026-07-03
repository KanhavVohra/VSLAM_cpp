#include "mono.h"

#include <opencv2/opencv.hpp>

#include <stdexcept>
#include <cctype>

MonoCamera::MonoCamera(
    const std::string& url,
    int width,
    int height,
    int nfeatures,
    int framesToDrop
)
    : orb_(nfeatures),
      width_(width),
      height_(height),
      framesToDrop_(framesToDrop)
{
    if (isWebcamIndex(url)) {
        cap_.open(std::stoi(url));
    } else {
        cap_.open(url, cv::CAP_FFMPEG);
    }

    cap_.set(cv::CAP_PROP_BUFFERSIZE, 1);

    if (!cap_.isOpened()) {
        throw std::runtime_error("Failed to open camera: " + url);
    }
}

bool MonoCamera::isWebcamIndex(const std::string& url) const {
    if (url.empty()) {
        return false;
    }

    for (char c : url) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            return false;
        }
    }

    return true;
}

bool MonoCamera::readLatestFrame() {
    // For IP cameras, drop old buffered frames.
    // This prevents delay buildup.
    for (int i = 0; i < framesToDrop_; i++) {
        if (!cap_.grab()) {
            return false;
        }
    }

    return cap_.retrieve(rawFrame_);
}

bool MonoCamera::grabFrame() {
    bool ok = readLatestFrame();

    if (!ok || rawFrame_.empty()) {
        frame_.release();
        output_.release();
        return false;
    }

    cv::resize(
        rawFrame_,
        frame_,
        cv::Size(width_, height_),
        0,
        0,
        cv::INTER_AREA
    );

    return true;
}

void MonoCamera::processFrame() {
    if (frame_.empty()) {
        output_.release();
        return;
    }

    cv::cvtColor(frame_, gray_, cv::COLOR_BGR2GRAY);

    orb_.extract(gray_, keypoints_, descriptors_);

    frame_.copyTo(output_);

    // Manual drawing is faster than cv::drawKeypoints with rich flags.
    for (const auto& kp : keypoints_) {
        cv::circle(
            output_,
            kp.pt,
            2,
            cv::Scalar(0, 255, 0),
            -1,
            cv::LINE_8
        );
    }

    cv::putText(
        output_,
        "ORB Features: " + std::to_string(keypoints_.size()),
        cv::Point(20, 35),
        cv::FONT_HERSHEY_SIMPLEX,
        1.0,
        cv::Scalar(0, 255, 0),
        2
    );

    if (!descriptors_.empty()) {
        cv::putText(
            output_,
            "Descriptors: " + std::to_string(descriptors_.rows),
            cv::Point(20, 70),
            cv::FONT_HERSHEY_SIMPLEX,
            0.8,
            cv::Scalar(0, 255, 0),
            2
        );
    }
}

bool MonoCamera::hasFrame() const {
    return !frame_.empty() && !output_.empty();
}

const cv::Mat& MonoCamera::frame() const {
    return frame_;
}

const cv::Mat& MonoCamera::processedFrame() const {
    return output_;
}

const cv::Mat& MonoCamera::descriptors() const {
    return descriptors_;
}