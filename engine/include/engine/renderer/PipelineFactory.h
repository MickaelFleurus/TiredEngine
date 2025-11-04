#pragma once
#include <memory>

struct SDL_GPUGraphicsPipeline;
struct SDL_GPUDevice;
struct SDL_Window;

namespace Renderer {
struct SPipelineConfig;

class CPipelineFactory {
public:
    CPipelineFactory(SDL_GPUDevice& device, SDL_Window& window);
    ~CPipelineFactory();

    SDL_GPUGraphicsPipeline*
    CreateGraphicsPipeline(const SPipelineConfig& config);

private:
    class CImpl;
    std::unique_ptr<CImpl> mImpl;
};
} // namespace Renderer