#ifndef STERO_H
#define STERO_H

#include <opencv2/opencv.hpp>
#include "orb_cv.h"


class StereoCamera {
    public:
        StereoCamera(const std::string& left_url, const std::string& right_url);
        bool grabFrames();
        void processFrames();
        void show();
    private:
        cv::VideoCapture cap_left_;
        cv::VideoCapture cap_right_;
        cv::Mat frame_left_, frame_right_;
        cv::Mat gray_left_, gray_right_;
        cv::Mat output_left_, output_right_;   
        
        
        ORBExtractor orb_;
        std::vector<cv::KeyPoint> keypoints_;
        cv::Mat descriptors_;
};




#endif 