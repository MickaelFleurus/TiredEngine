#pragma once

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

struct SPipelineConfig {
    EPrimitiveType PrimitiveType = EPrimitiveType::TriangleList;
    EFillMode FillMode = EFillMode::Fill;
    ECullMode CullMode = ECullMode::Back;
    EFrontFace FrontFace = EFrontFace::CounterClockwise;
};

} // namespace Renderer