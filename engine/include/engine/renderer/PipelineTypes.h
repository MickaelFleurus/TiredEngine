#pragma once
struct SDL_GPUShader;

#include "engine/renderer/ShaderFactory.h" // For SShaderResources
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

    SShaderResources vertexResources;
    SShaderResources fragmentResources;

    bool operator==(const SPipelineConfig& other) const noexcept {
        return std::tie(primitiveType, fillMode, cullMode, frontFace,
                        shaderName, enableBlending, vertexLayout) ==
               std::tie(other.primitiveType, other.fillMode, other.cullMode,
                        other.frontFace, other.shaderName, other.enableBlending,
                        other.vertexLayout);
    }
};

struct SPipelineConfigHash {
    static void hash_combine(std::size_t& seed, std::size_t value) noexcept {
        // Same as Boost’s hash_combine
        seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    std::size_t operator()(const SPipelineConfig& d) const noexcept {
        std::size_t h = 0;
        hash_combine(h, std::hash<EPrimitiveType>()(d.primitiveType));
        hash_combine(h, std::hash<EFillMode>()(d.fillMode));
        hash_combine(h, std::hash<ECullMode>()(d.cullMode));
        hash_combine(h, std::hash<EFrontFace>()(d.frontFace));
        hash_combine(h, std::hash<std::string>()(d.shaderPath));
        hash_combine(h, std::hash<bool>()(d.enableBlending));
        hash_combine(h, std::hash<EVertexLayout>()(d.vertexLayout));
        return h;
    }
};

} // namespace Renderer