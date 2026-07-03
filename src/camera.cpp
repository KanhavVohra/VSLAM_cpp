#include "camera.h"
#include "PangolinViewer.h"

std::string Camera::cameraURL(const std::string& inputUrl) {
    if (inputUrl.empty()) {
        return inputUrl;
    }

    // Webcam index like "0"
    if (inputUrl.length() == 1 &&
        std::isdigit(static_cast<unsigned char>(inputUrl[0]))) {
        return inputUrl;
    }

    bool hasProtocol =
        inputUrl.rfind("http://", 0) == 0 ||
        inputUrl.rfind("https://", 0) == 0 ||
        inputUrl.rfind("rtsp://", 0) == 0;

    std::string url = hasProtocol ? inputUrl : "http://" + inputUrl;

    std::size_t protocolEnd = url.find("://");
    std::size_t searchStart = protocolEnd == std::string::npos ? 0 : protocolEnd + 3;
    std::size_t pathStart = url.find('/', searchStart);

    // If user typed only host:port, add /video
    if (pathStart == std::string::npos) {
        url += "/video";
    }

    return url;
}



