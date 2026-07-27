#ifndef CAMERA_H
#define CAMERA_H

#include <string>

struct CameraConfig
{
    int width = 640;
    int height = 480;

    int nfeatures = 1000;

    // Used for network/IP cameras.
    // Webcam sources automatically use zero.
    int framesToDrop = 3;
};

class Camera
{
public:
    Camera() = default;
    ~Camera() = default;

    std::string cameraURL(const std::string& inputUrl) const;
    bool isWebcamIndex(const std::string& source);
};

#endif