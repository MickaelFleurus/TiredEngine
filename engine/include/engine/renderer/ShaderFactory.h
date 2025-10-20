#pragma once

#include <memory>
struct SDL_GPUDevice;

namespace Renderer {
class CShader;
class CShaderFactory {
public:
    CShaderFactory(SDL_GPUDevice& device);
    ~CShaderFactory();
    CShader* CreateFragmentShader(const char* filePath);
    CShader* CreateVertexShader(const char* filePath);

private:
    class CImpl;
    std::unique_ptr<CImpl> mImpl;
};
} // namespace Renderer