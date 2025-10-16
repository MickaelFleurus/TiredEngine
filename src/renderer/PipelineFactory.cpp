#include "renderer/PipelineFactory.h"
#include "renderer/PipelineTypes.h"
#include "renderer/Shader.h"
#include <SDL_gpu.h>

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
        : m_Device(device), m_Window(window) {
    }

    std::unique_ptr<SDL_GPUGraphicsPipeline>
    CreateGraphicsPipeline(CShader& vertexShader, CShader& fragmentShader,
                           SPipelineConfig config = {}) {
        SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
            .target_info =
                {
                    .num_color_targets = 1,
                    .color_target_descriptions =
                        (SDL_GPUColorTargetDescription[]){
                            {.format = SDL_GetGPUSwapchainTextureFormat(
                                 device, window)}},
                },

            .rasterizer_state.fill_mode = ConvertFillMode(config.FillMode),
            .primitive_type = ConvertPrimitiveType(config.PrimitiveType),
            .vertex_shader = vertexShader.Get(),
            .fragment_shader = fragmentShader.Get(),
        };

        return nullptr;
    }

private:
    SDL_GPUDevice& m_Device;
    SDL_Window& m_Window;
}; // namespace Renderer

CPipelineFactory::CPipelineFactory(SDL_GPUDevice& device, SDL_Window& window)
    : m_Impl(std::make_unique<CImpl>(device, window)) {
}

CPipelineFactory::~CPipelineFactory() = default;

} // namespace Renderer