#pragma once

namespace Core {

class CWindowData {
public:
    CWindowData(int width = 1240, int height = 720);

    int GetWidth() const;
    int GetHeight() const;

private:
    int mWidth;
    int mHeight;
};
} // namespace Core