#pragma once
#include <memory>

struct SDL_GPUGraphicsPipeline;
class SDL_GPUDevice;
class SDL_Window;

namespace Renderer {
class CShader;
class CPipelineFactory {
public:
    CPipelineFactory(SDL_GPUDevice& device, SDL_Window& window);
    ~CPipelineFactory();

    std::unique_ptr<SDL_GPUGraphicsPipeline>
    CreateGraphicsPipeline(CShader& vertexShader, CShader& fragmentShader);

private:
    class CImpl;
    std::unique_ptr<CImpl> m_Impl;
};
} // namespace Renderer