#pragma once
#include <memory>

namespace Renderer {
class CWindow {
public:
    CWindow();
    ~CWindow();

    void Render();

private:
    class CImpl;
    std::unique_ptr<CImpl> m_Impl;
};
} // namespace Renderer