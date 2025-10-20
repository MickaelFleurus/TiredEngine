#include "engine/renderer/PipelineFactory.h"
#include "engine/renderer/PipelineTypes.h"
#include "engine/renderer/Shader.h"
#include <SDL3/SDL_gpu.h>

#include <functional>

namespace {
// Helper functions and types for pipeline creation
SDL_GPUPrimitiveType ConvertPrimitiveType(Renderer::EPrimitiveType type) {
    switch (type) {
    case Renderer::EPrimitiveType::TriangleList:
        return SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    case Renderer::EPrimitiveType::TriangleStrip:
        return SDL_GPU_PRIMITIVETYPE_TRIANGLESTRIP;
    case Renderer::EPrimitiveType::LineList:
        return SDL_GPU_PRIMITIVETYPE_LINELIST;
    case Renderer::EPrimitiveType::LineStrip:
        return SDL_GPU_PRIMITIVETYPE_LINESTRIP;
    case Renderer::EPrimitiveType::PointList:
        return SDL_GPU_PRIMITIVETYPE_POINTLIST;
    default:
        return SDL_GPU_PRIMITIVETYPE_TRIANGLELIST; // Default fallback
    }
}

SDL_GPUFillMode ConvertFillMode(Renderer::EFillMode mode) {
    switch (mode) {
    case Renderer::EFillMode::Fill:
        return SDL_GPU_FILLMODE_FILL;
    case Renderer::EFillMode::Line:
        return SDL_GPU_FILLMODE_LINE;
    default:
        return SDL_GPU_FILLMODE_FILL; // Default fallback
    }
}

SDL_GPUCullMode ConvertCullMode(Renderer::ECullMode mode) {
    switch (mode) {
    case Renderer::ECullMode::None:
        return SDL_GPU_CULLMODE_NONE;
    case Renderer::ECullMode::Front:
        return SDL_GPU_CULLMODE_FRONT;
    case Renderer::ECullMode::Back:
        return SDL_GPU_CULLMODE_BACK;
    default:
        return SDL_GPU_CULLMODE_BACK; // Default fallback
    }
}

SDL_GPUFrontFace ConvertFrontFace(Renderer::EFrontFace face) {
    switch (face) {
    case Renderer::EFrontFace::Clockwise:
        return SDL_GPU_FRONTFACE_CLOCKWISE;
    case Renderer::EFrontFace::CounterClockwise:
        return SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
    default:
        return SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE; // Default fallback
    }
}

} // namespace

namespace Renderer {

class CPipelineFactory::CImpl {
public:
    CImpl(SDL_GPUDevice& device, SDL_Window& window)
        : mDevice(device), mWindow(window) {
    }

    std::unique_ptr<SDL_GPUGraphicsPipeline,
                    std::function<void(SDL_GPUGraphicsPipeline*)>>
    CreateGraphicsPipeline(CShader& vertexShader, CShader& fragmentShader,
                           SPipelineConfig config = {}) {
        SDL_GPUColorTargetDescription colorTarget{};
        colorTarget.format =
            SDL_GetGPUSwapchainTextureFormat(&mDevice, &mWindow);

        SDL_GPUGraphicsPipelineTargetInfo targetInfo{};
        targetInfo.num_color_targets = 1;
        targetInfo.color_target_descriptions = &colorTarget;

        SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo{};
        pipelineCreateInfo.target_info = targetInfo;
        pipelineCreateInfo.rasterizer_state.fill_mode =
            ConvertFillMode(config.FillMode);
        pipelineCreateInfo.rasterizer_state.cull_mode =
            ConvertCullMode(config.CullMode);
        pipelineCreateInfo.rasterizer_state.front_face =
            ConvertFrontFace(config.FrontFace);
        pipelineCreateInfo.primitive_type =
            ConvertPrimitiveType(config.PrimitiveType);
        pipelineCreateInfo.vertex_shader = vertexShader.Get();
        pipelineCreateInfo.fragment_shader = fragmentShader.Get();

        SDL_GPUGraphicsPipeline* pipeline =
            SDL_CreateGPUGraphicsPipeline(&mDevice, &pipelineCreateInfo);

        // Capture device by reference in the lambda
        auto deleter = [device = &mDevice](SDL_GPUGraphicsPipeline* p) {
            if (p)
                SDL_ReleaseGPUGraphicsPipeline(device, p);
        };

        return std::unique_ptr<SDL_GPUGraphicsPipeline,
                               std::function<void(SDL_GPUGraphicsPipeline*)>>(
            pipeline, deleter);
    }

private:
    SDL_GPUDevice& mDevice;
    SDL_Window& mWindow;
};

CPipelineFactory::CPipelineFactory(SDL_GPUDevice& device, SDL_Window& window)
    : mImpl(std::make_unique<CImpl>(device, window)) {
}

CPipelineFactory::~CPipelineFactory() = default;

} // namespace Renderer