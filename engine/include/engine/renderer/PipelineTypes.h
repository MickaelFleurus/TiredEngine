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

enum class EVertexLayout {
    Simple,
    UI,
    Mesh3D,
};

struct SPipelineConfig {
    EPrimitiveType primitiveType = EPrimitiveType::TriangleList;
    EFillMode fillMode = EFillMode::Fill;
    ECullMode cullMode = ECullMode::Back;
    EFrontFace frontFace = EFrontFace::CounterClockwise;

    std::string shaderName;
    std::string shaderPath;

    bool enableBlending = false;
    bool enableDepthTest = true;

    EVertexLayout vertexLayout = EVertexLayout::Simple;

    bool operator==(const SPipelineConfig& other) const noexcept {
        return std::tie(primitiveType, fillMode, cullMode, frontFace,
                        shaderName, enableBlending, vertexLayout) ==
               std::tie(other.primitiveType, other.fillMode, other.cullMode,
                        other.frontFace, other.shaderName, other.enableBlending,
                        other.vertexLayout);
    }
};

struct SPipelineConfigHash {

    std::size_t operator()(const SPipelineConfig& d) const noexcept {
        return Utils::CreateHash(d.primitiveType, d.fillMode, d.cullMode,
                                 d.frontFace, d.shaderPath, d.enableBlending,
                                 d.vertexLayout);
    }
};

} // namespace Renderer
