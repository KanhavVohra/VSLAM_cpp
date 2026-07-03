
#include "stero.h"

#include <iostream>

StereoCamera::StereoCamera(const std::string& left_url, const std::string& right_url)
    : orb_(2000)
{
    cap_left_.open(left_url, cv::CAP_FFMPEG);
    cap_right_.open(right_url, cv::CAP_FFMPEG);

    cap_left_.set(cv::CAP_PROP_BUFFERSIZE, 1);
    cap_right_.set(cv::CAP_PROP_BUFFERSIZE, 1);

    if (!cap_left_.isOpened() || !cap_right_.isOpened()) {
        throw std::runtime_error("Failed to open one or both IP camera streams");
    }
}

bool StereoCamera::grabFrames(){
    bool capL = cap_left_.read(frame_left_);
    bool capR = cap_right_.read(frame_right_);
    return capL && capR;
};

void StereoCamera::processFrames(){
    cv::cvtColor(frame_left_, gray_left_, cv::COLOR_BGR2GRAY);
    cv::cvtColor(frame_right_, gray_right_, cv::COLOR_BGR2GRAY);

    cv::equalizeHist(gray_left_, output_left_);
    cv::equalizeHist(gray_right_, output_right_);

    cv::drawKeypoints(
        output_left_, std::vector<cv::KeyPoint>(), output_left_,
        cv::Scalar(0, 255, 0),
        cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS
    );

    cv::drawKeypoints(
        output_right_, std::vector<cv::KeyPoint>(), output_right_,
        cv::Scalar(0, 255, 0),
        cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS
    );
};

void StereoCamera::show(){
  cv::imshow("Stereo Left", output_left_);
  cv::imshow("Stereo Right", output_right_);  
};