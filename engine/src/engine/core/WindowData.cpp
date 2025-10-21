#include "engine/core/WindowData.h"

namespace Core {
CWindowData::CWindowData(int width, int height)
    : mWidth(width), mHeight(height) {
}

int CWindowData::GetWidth() const {
    return mWidth;
}

int CWindowData::GetHeight() const {
    return mHeight;
}

} // namespace Core