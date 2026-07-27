#include "camera.h"

#include <algorithm>
#include <cctype>
#include <string>

namespace
{
std::string trimWhitespace(
    const std::string& input
)
{
    const auto firstCharacter =
        std::find_if_not(
            input.begin(),
            input.end(),
            [](unsigned char character)
            {
                return std::isspace(character) != 0;
            }
        );

    if (firstCharacter == input.end())
    {
        return "";
    }

    const auto lastCharacter =
        std::find_if_not(
            input.rbegin(),
            input.rend(),
            [](unsigned char character)
            {
                return std::isspace(character) != 0;
            }
        ).base();

    return std::string(
        firstCharacter,
        lastCharacter
    );
}

bool startsWith(
    const std::string& value,
    const std::string& prefix
)
{
    return value.rfind(prefix, 0) == 0;
}

bool isNumericCameraIndex(
    const std::string& value
)
{
    if (value.empty())
    {
        return false;
    }

    return std::all_of(
        value.begin(),
        value.end(),
        [](unsigned char character)
        {
            return std::isdigit(character) != 0;
        }
    );
}
}

std::string Camera::cameraURL(
    const std::string& inputUrl
) const
{
    std::string url =
        trimWhitespace(inputUrl);

    if (url.empty())
    {
        return "";
    }

    // Webcam indexes such as:
    // 0
    // 1
    // 10
    if (isNumericCameraIndex(url))
    {
        return url;
    }

    const bool hasProtocol =
        startsWith(url, "http://") ||
        startsWith(url, "https://") ||
        startsWith(url, "rtsp://");

    if (!hasProtocol)
    {
        url = "http://" + url;
    }

    const bool isHttpCamera =
        startsWith(url, "http://") ||
        startsWith(url, "https://");

    // Only automatically append /video for HTTP cameras.
    // RTSP cameras normally use paths such as /stream1,
    // /live, or /cam/realmonitor.
    if (isHttpCamera)
    {
        const std::size_t protocolEnd =
            url.find("://");

        const std::size_t hostStart =
            protocolEnd == std::string::npos
                ? 0
                : protocolEnd + 3;

        const std::size_t pathStart =
            url.find('/', hostStart);

        if (pathStart == std::string::npos)
        {
            url += "/video";
        }
    }

    return url;
}