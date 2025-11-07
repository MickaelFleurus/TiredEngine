#pragma once
#include <memory>

struct SDL_GPUGraphicsPipeline;

namespace Renderer {
struct SPipelineConfig;
class CWindow;

class CPipelineFactory {
public:
    CPipelineFactory(const CWindow& window);
    ~CPipelineFactory();

    SDL_GPUGraphicsPipeline*
    CreateGraphicsPipeline(const SPipelineConfig& config);

private:
    class CImpl;
    std::unique_ptr<CImpl> mImpl;
};
} // namespace Renderer