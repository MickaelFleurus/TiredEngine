#pragma once

#include "engine/utils/Hashing.h"
#include <glm/vec4.hpp>

namespace Renderer {
enum class EPrimitiveType {
    TriangleList,  /**< A series of separate triangles. */
    TriangleStrip, /**< A series of connected triangles. */
    LineList,      /**< A series of separate lines. */
    LineStrip,     /**< A series of connected lines. */
    PointList      /**< A series of separate points. */
};

enum class EFillMode {
    Fill, /**< Fill the area of the primitive. */
    Line, /**< Draw lines along the edges of the primitive. */
    Point /**< Draw points at the vertices of the primitive. */
};

enum class ECullMode {
    None,        /**< No culling is performed. */
    Front,       /**< Cull front-facing primitives. */
    Back,        /**< Cull back-facing primitives. */
    FrontAndBack /**< Cull both front and back-facing primitives. */
};

enum class EFrontFace {
    Clockwise,       /**< Vertices are defined in a clockwise order. */
    CounterClockwise /**< Vertices are defined in a counter-clockwise order. */
};

enum class EVertexLayout {
    Simple,    /**< Position + TexCoord only (for sprites, quads) */
    Instanced, /**< Position + TexCoord + Instance data (for text, particles) */
    Mesh3D,    /**< Position + Normal + TexCoord (for 3D meshes) */
};

struct SPipelineConfig {
    EPrimitiveType primitiveType = EPrimitiveType::TriangleList;
    EFillMode fillMode = EFillMode::Fill;
    ECullMode cullMode = ECullMode::Back;
    EFrontFace frontFace = EFrontFace::CounterClockwise;

    std::string shaderName;
    std::string shaderPath;

    bool enableBlending = false;

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