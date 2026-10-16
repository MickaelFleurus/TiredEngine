#pragma once

#include <glm/vec4.hpp>

#include "engine/utils/Hashing.h"

namespace Renderer {
enum class EPrimitiveType {
    TriangleList,
    TriangleStrip,
    LineList,
    LineStrip,
    PointList
};

enum class EFillMode { Fill, Line, Point };

enum class ECullMode { None, Front, Back, FrontAndBack };

enum class EFrontFace { Clockwise, CounterClockwise };

// FIXME: Can be constexpr? Do I really need a string here.
struct SComputePipelineConfig {
    std::string shaderName;
    std::string shaderPath;

    bool operator==(const SComputePipelineConfig& other) const noexcept {
        return std::tie(shaderName, shaderPath) ==
               std::tie(other.shaderName, other.shaderPath);
    }
};

struct SPipelineConfig {
    EPrimitiveType primitiveType = EPrimitiveType::TriangleList;
    EFillMode fillMode = EFillMode::Fill;
    ECullMode cullMode = ECullMode::Back;
    EFrontFace frontFace = EFrontFace::CounterClockwise;
    std::size_t pushConstantSize = 0;

    std::string shaderName;
    std::string shaderPath;

    bool enableBlending = false;
    bool enableDepthTest = true;

    bool operator==(const SPipelineConfig& other) const noexcept {
        return std::tie(primitiveType, fillMode, cullMode, frontFace,
                        shaderName, enableBlending) ==
               std::tie(other.primitiveType, other.fillMode, other.cullMode,
                        other.frontFace, other.shaderName,
                        other.enableBlending);
    }
};

struct SPipelineConfigHash {

    std::size_t operator()(const SPipelineConfig& d) const noexcept {
        return Utils::CreateHash(d.primitiveType, d.fillMode, d.cullMode,
                                 d.frontFace, d.shaderPath, d.enableBlending);
    }
};

struct SComputePipelineConfigHash {

    std::size_t operator()(const SComputePipelineConfig& d) const noexcept {
        return Utils::CreateHash(d.shaderName, d.shaderPath);
    }
};

} // namespace Renderer
