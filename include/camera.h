#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <string>


struct CamerConfig {
    int width = 640;
    int height = 480;
    int framestodrop = 3;
    bool resizeFrame = true;
};




class Camera {
private:


public:
    std::string cameraURL(const std::string& inputUrl);
    void setWindow();

    ~Camera();

};
#endif

