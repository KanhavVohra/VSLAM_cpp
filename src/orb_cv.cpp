#include "orb_cv.h"

ORBExtractor::ORBExtractor(int nfeatures) {
    orb_ = cv::ORB::create(
        nfeatures,
        1.2f,
        8
    );
}

void ORBExtractor::detect(
    const cv::Mat& gray,
    std::vector<cv::KeyPoint>& keypoints
) {
    orb_->detect(gray, keypoints);
}

void ORBExtractor::extract(
    const cv::Mat& gray,
    std::vector<cv::KeyPoint>& keypoints,
    cv::Mat& descriptors
) {
    orb_->detectAndCompute(gray, cv::noArray(), keypoints, descriptors);
}