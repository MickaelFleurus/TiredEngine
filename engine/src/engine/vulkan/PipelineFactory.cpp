#include "engine/vulkan/PipelineFactory.h"

#include <array>
#include <functional>

#include <vulkan/vulkan.h>

#include "engine/core/DataTypes.h"
#include "engine/renderer/PipelineTypes.h"
#include "engine/renderer/RendererUtils.h"
#include "engine/utils/Logger.h"
#include "engine/vulkan/DescriptorStorage.h"
#include "engine/vulkan/ShaderFactory.h"
#include "engine/vulkan/VulkanContext.h"

namespace {

constexpr std::array<VkDynamicState, 2> kDynamicStates = {
    VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

constexpr VkPipelineDynamicStateCreateInfo kDynamicStateInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
    .dynamicStateCount = static_cast<uint32_t>(kDynamicStates.size()),
    .pDynamicStates = kDynamicStates.data()};

constexpr VkPipelineViewportStateCreateInfo kViewportStateInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
    .viewportCount = 1,
    .scissorCount = 1};

VkPrimitiveTopology ConvertPrimitiveType(Renderer::EPrimitiveType type) {
    switch (type) {
    case Renderer::EPrimitiveType::TriangleList:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    case Renderer::EPrimitiveType::TriangleStrip:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    case Renderer::EPrimitiveType::LineList:
        return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    case Renderer::EPrimitiveType::LineStrip:
        return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
    case Renderer::EPrimitiveType::PointList:
        return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    default:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    }
}

VkPolygonMode ConvertFillMode(Renderer::EFillMode mode) {
    switch (mode) {
    case Renderer::EFillMode::Fill:
        return VK_POLYGON_MODE_FILL;
    case Renderer::EFillMode::Line:
        return VK_POLYGON_MODE_LINE;
    default:
        return VK_POLYGON_MODE_FILL;
    }
}

VkCullModeFlags ConvertCullMode(Renderer::ECullMode mode) {
    switch (mode) {
    case Renderer::ECullMode::None:
        return VK_CULL_MODE_NONE;
    case Renderer::ECullMode::Front:
        return VK_CULL_MODE_FRONT_BIT;
    case Renderer::ECullMode::Back:
        return VK_CULL_MODE_BACK_BIT;
    default:
        return VK_CULL_MODE_BACK_BIT;
    }
}

VkFrontFace ConvertFrontFace(Renderer::EFrontFace face) {
    switch (face) {
    case Renderer::EFrontFace::Clockwise:
        return VK_FRONT_FACE_CLOCKWISE;
    case Renderer::EFrontFace::CounterClockwise:
        return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    default:
        return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    }
}

} // namespace

namespace Vulkan {

class CPipelineFactory::CImpl {
public:
    CImpl(const Vulkan::CVulkanContext& contextGetter)
        : mContextGetter(contextGetter), mShaderFactory(contextGetter) {
    }

    ~CImpl() {
        for (auto& [path, pipeline] : mPipelineCache) {
            vkDestroyPipelineLayout(mContextGetter.GetDevice(),
                                    pipeline.pipelineLayout, nullptr);
            vkDestroyPipeline(mContextGetter.GetDevice(), pipeline.pipeline,
                              nullptr);
        }
    }

    Renderer::SPipelineDescriptors
    GetOrCreateGraphicsPipeline(Renderer::SPipelineConfig config,
                                CDescriptorStorage& layoutStorage) {
        if (!mPipelineCache.contains(config)) {
            auto vertexShader = mShaderFactory.CreateVertexShader(
                config.shaderName, config.shaderPath);
            auto fragmentShader = mShaderFactory.CreateFragmentShader(
                config.shaderName, config.shaderPath);

            if (vertexShader == VK_NULL_HANDLE ||
                fragmentShader == VK_NULL_HANDLE) {
                LOG_ERROR("Failed to create shaders for pipeline: {}",
                          config.shaderName);
                return {};
            }

            VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
            inputAssembly.sType =
                VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology = ConvertPrimitiveType(config.primitiveType);
            inputAssembly.primitiveRestartEnable = VK_FALSE;

            VkPipelineRasterizationStateCreateInfo rasterizer{};
            rasterizer.sType =
                VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer.depthClampEnable = VK_FALSE;
            rasterizer.rasterizerDiscardEnable = VK_FALSE;
            rasterizer.polygonMode = ConvertFillMode(config.fillMode);
            rasterizer.cullMode =
                VK_CULL_MODE_NONE; // ConvertCullMode(config.cullMode);
            rasterizer.frontFace = ConvertFrontFace(config.frontFace);
            rasterizer.lineWidth = 1.0f;
            rasterizer.depthBiasEnable = VK_FALSE;

            VkPipelineMultisampleStateCreateInfo multisampling{};
            multisampling.sType =
                VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampling.sampleShadingEnable = VK_FALSE;
            multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
            multisampling.minSampleShading = 1.0f;
            multisampling.pSampleMask = nullptr;
            multisampling.alphaToCoverageEnable = VK_FALSE;
            multisampling.alphaToOneEnable = VK_FALSE;

            VkPipelineColorBlendAttachmentState colorBlendAttachment{};
            if (config.enableBlending) {
                colorBlendAttachment.blendEnable = VK_TRUE;
                colorBlendAttachment.srcColorBlendFactor =
                    VK_BLEND_FACTOR_SRC_ALPHA;
                colorBlendAttachment.dstColorBlendFactor =
                    VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
                colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
                colorBlendAttachment.dstAlphaBlendFactor =
                    VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
                colorBlendAttachment.colorWriteMask =
                    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                    VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

            } else {
                colorBlendAttachment.blendEnable = VK_FALSE;
                // When blending is disabled, still write all channels
                colorBlendAttachment.colorWriteMask =
                    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                    VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            }
            VkPipelineColorBlendStateCreateInfo colorBlending{};
            colorBlending.sType =
                VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlending.logicOpEnable = VK_FALSE;
            colorBlending.logicOp = VK_LOGIC_OP_COPY;
            colorBlending.attachmentCount = 1;
            colorBlending.pAttachments = &colorBlendAttachment;
            colorBlending.blendConstants[0] = 0.0f;
            colorBlending.blendConstants[1] = 0.0f;
            colorBlending.blendConstants[2] = 0.0f;
            colorBlending.blendConstants[3] = 0.0f;

            auto layoutInfo = CreateVertexLayout(config.vertexLayout);

            VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
            vertexInputInfo.sType =
                VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputInfo.pNext = nullptr;

            vertexInputInfo.pVertexBindingDescriptions =
                layoutInfo.bufferDescriptions.data();
            vertexInputInfo.vertexBindingDescriptionCount =
                static_cast<uint32_t>(layoutInfo.bufferDescriptions.size());
            vertexInputInfo.pVertexAttributeDescriptions =
                layoutInfo.attributes.data();
            vertexInputInfo.vertexAttributeDescriptionCount =
                static_cast<uint32_t>(layoutInfo.attributes.size());

            VkPipelineLayout pipelineLayout;
            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType =
                VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            std::vector<VkDescriptorSetLayout> descriptorSetLayouts = {
                layoutStorage.GetLayout()};

            pipelineLayoutInfo.setLayoutCount =
                static_cast<uint32_t>(descriptorSetLayouts.size());
            pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();

            VkPushConstantRange pushConstantRange{};
            pushConstantRange.stageFlags =
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
            pushConstantRange.offset = 0;
            pushConstantRange.size = sizeof(Core::SPushConstantData);

            pipelineLayoutInfo.pushConstantRangeCount = 1;
            pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

            // Create the pipeline layout first
            if (vkCreatePipelineLayout(mContextGetter.GetDevice(),
                                       &pipelineLayoutInfo, nullptr,
                                       &pipelineLayout) != VK_SUCCESS) {
                LOG_ERROR("Failed to create pipeline layout!");
                return {};
            }

            // Define shader stages
            VkPipelineShaderStageCreateInfo vertexShaderStageInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = vertexShader,
                .pName = "main"};
            VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = fragmentShader,
                .pName = "main"};
            std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages = {
                vertexShaderStageInfo, fragmentShaderStageInfo};

            VkGraphicsPipelineCreateInfo pipelineInfo{};
            pipelineInfo.sType =
                VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount = shaderStages.size();
            pipelineInfo.pStages = shaderStages.data();
            pipelineInfo.pVertexInputState = &vertexInputInfo;
            pipelineInfo.pInputAssemblyState = &inputAssembly;
            pipelineInfo.pViewportState = &kViewportStateInfo;
            pipelineInfo.pRasterizationState = &rasterizer;
            pipelineInfo.pMultisampleState = &multisampling;
            pipelineInfo.pColorBlendState = &colorBlending;
            pipelineInfo.pDynamicState = &kDynamicStateInfo;
            pipelineInfo.layout = pipelineLayout;
            pipelineInfo.renderPass = mContextGetter.GetRenderPass();
            pipelineInfo.subpass = 0;

            VkPipeline graphicsPipeline;
            if (vkCreateGraphicsPipelines(
                    mContextGetter.GetDevice(), VK_NULL_HANDLE, 1,
                    &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
                LOG_ERROR("Failed to create graphics pipeline!");
                return {};
            }

            mPipelineCache.try_emplace(config, graphicsPipeline,
                                       pipelineLayout);
        }

        return mPipelineCache[config];
    }

private:
    const Vulkan::CVulkanContext& mContextGetter;
    CShaderFactory mShaderFactory;
    std::unordered_map<Renderer::SPipelineConfig,
                       Renderer::SPipelineDescriptors,
                       Renderer::SPipelineConfigHash>
        mPipelineCache;
};

CPipelineFactory::CPipelineFactory(const Vulkan::CVulkanContext& contextGetter)
    : mImpl(std::make_unique<CImpl>(contextGetter)) {
}

CPipelineFactory::~CPipelineFactory() = default;

Renderer::SPipelineDescriptors CPipelineFactory::GetOrCreateGraphicsPipeline(
    const Renderer::SPipelineConfig& config,
    CDescriptorStorage& layoutStorage) {
    return mImpl->GetOrCreateGraphicsPipeline(config, layoutStorage);
}

} // namespace Vulkan