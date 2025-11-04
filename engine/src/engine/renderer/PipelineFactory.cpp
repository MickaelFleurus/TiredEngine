#include "engine/renderer/PipelineFactory.h"
#include "engine/renderer/PipelineTypes.h"
#include "engine/renderer/RendererUtils.h"
#include "engine/renderer/ShaderFactory.h"
#include <SDL3/SDL_gpu.h>

#include <functional>

namespace {

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
        return SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    }
}

SDL_GPUFillMode ConvertFillMode(Renderer::EFillMode mode) {
    switch (mode) {
    case Renderer::EFillMode::Fill:
        return SDL_GPU_FILLMODE_FILL;
    case Renderer::EFillMode::Line:
        return SDL_GPU_FILLMODE_LINE;
    default:
        return SDL_GPU_FILLMODE_FILL;
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
        return SDL_GPU_CULLMODE_BACK;
    }
}

SDL_GPUFrontFace ConvertFrontFace(Renderer::EFrontFace face) {
    switch (face) {
    case Renderer::EFrontFace::Clockwise:
        return SDL_GPU_FRONTFACE_CLOCKWISE;
    case Renderer::EFrontFace::CounterClockwise:
        return SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
    default:
        return SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
    }
}

} // namespace

namespace Renderer {

class CPipelineFactory::CImpl {
public:
    CImpl(SDL_GPUDevice& device, SDL_Window& window)
        : mDevice(device), mWindow(window), mShaderFactory(device) {
    }

    ~CImpl() {
        for (auto& [path, pipeline] : mPipelineCache) {
            SDL_ReleaseGPUGraphicsPipeline(&mDevice, pipeline);
        }
    }

    SDL_GPUGraphicsPipeline* CreateGraphicsPipeline(SPipelineConfig config) {
        if (!mPipelineCache.contains(config)) {

            auto* vertexShader = mShaderFactory.CreateVertexShader(
                config.shaderName, config.shaderPath, config.vertexResources);
            auto* fragmentShader = mShaderFactory.CreateFragmentShader(
                config.shaderName, config.shaderPath, config.fragmentResources);

            SDL_GPUColorTargetDescription colorTarget{};
            colorTarget.format =
                SDL_GetGPUSwapchainTextureFormat(&mDevice, &mWindow);

            if (config.enableBlending) {
                colorTarget.blend_state.enable_blend = true;

                // Standard alpha blending: srcAlpha * src + (1-srcAlpha) * dst
                colorTarget.blend_state.src_color_blendfactor =
                    SDL_GPU_BLENDFACTOR_SRC_ALPHA;
                colorTarget.blend_state.dst_color_blendfactor =
                    SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
                colorTarget.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;

                // Same for alpha channel
                colorTarget.blend_state.src_alpha_blendfactor =
                    SDL_GPU_BLENDFACTOR_ONE;
                colorTarget.blend_state.dst_alpha_blendfactor =
                    SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
                colorTarget.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;

                // Write to all color channels
                colorTarget.blend_state.color_write_mask =
                    SDL_GPU_COLORCOMPONENT_R | SDL_GPU_COLORCOMPONENT_G |
                    SDL_GPU_COLORCOMPONENT_B | SDL_GPU_COLORCOMPONENT_A;
            } else {
                colorTarget.blend_state.enable_blend = false;
                // When blending is disabled, still write all channels
                colorTarget.blend_state.color_write_mask =
                    SDL_GPU_COLORCOMPONENT_R | SDL_GPU_COLORCOMPONENT_G |
                    SDL_GPU_COLORCOMPONENT_B | SDL_GPU_COLORCOMPONENT_A;
            }

            SDL_GPUGraphicsPipelineTargetInfo targetInfo{};
            targetInfo.num_color_targets = 1;
            targetInfo.color_target_descriptions = &colorTarget;

            auto layoutInfo = CreateVertexLayout(config.vertexLayout);

            SDL_GPUVertexInputState vertexInputState = {};
            vertexInputState.vertex_buffer_descriptions =
                layoutInfo.bufferDescriptions.data();
            vertexInputState.num_vertex_buffers =
                static_cast<Uint32>(layoutInfo.bufferDescriptions.size());
            vertexInputState.vertex_attributes = layoutInfo.attributes.data();
            vertexInputState.num_vertex_attributes =
                static_cast<Uint32>(layoutInfo.attributes.size());

            SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo{};
            pipelineCreateInfo.target_info = targetInfo;
            pipelineCreateInfo.vertex_input_state = vertexInputState;
            pipelineCreateInfo.rasterizer_state.fill_mode =
                ConvertFillMode(config.fillMode);
            pipelineCreateInfo.rasterizer_state.cull_mode =
                ConvertCullMode(config.cullMode);
            pipelineCreateInfo.rasterizer_state.front_face =
                ConvertFrontFace(config.frontFace);
            pipelineCreateInfo.primitive_type =
                ConvertPrimitiveType(config.primitiveType);
            pipelineCreateInfo.vertex_shader = vertexShader;
            pipelineCreateInfo.fragment_shader = fragmentShader;

            SDL_GPUGraphicsPipeline* pipeline =
                SDL_CreateGPUGraphicsPipeline(&mDevice, &pipelineCreateInfo);

            mPipelineCache.emplace(config, pipeline);
        }

        return mPipelineCache[config];
    }

private:
    SDL_GPUDevice& mDevice;
    SDL_Window& mWindow;
    CShaderFactory mShaderFactory;
    std::unordered_map<SPipelineConfig, SDL_GPUGraphicsPipeline*,
                       SPipelineConfigHash>
        mPipelineCache;
};

CPipelineFactory::CPipelineFactory(SDL_GPUDevice& device, SDL_Window& window)
    : mImpl(std::make_unique<CImpl>(device, window)) {
}

CPipelineFactory::~CPipelineFactory() = default;

SDL_GPUGraphicsPipeline*
CPipelineFactory::CreateGraphicsPipeline(const SPipelineConfig& config) {
    return mImpl->CreateGraphicsPipeline(config);
}

} // namespace Renderer