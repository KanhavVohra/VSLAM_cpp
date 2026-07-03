
#ifndef ORB_CV
#define ORB_CV


#include <opencv2/opencv.hpp>
#include <vector>

class ORBExtractor {
public:
    ORBExtractor(int nfeatures = 500);

    void detect(
        const cv::Mat& gray,
        std::vector<cv::KeyPoint>& keypoints
    );

    void extract(
        const cv::Mat& gray,
        std::vector<cv::KeyPoint>& keypoints,
        cv::Mat& descriptors
    );

private:
    cv::Ptr<cv::ORB> orb_;
};

#endif