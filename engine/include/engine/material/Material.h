#pragma once

#include "engine/material/AbstractMaterial.h"

namespace Material {
class CMaterial : public CAbstractMaterial {
public:
    CMaterial(EMaterialType type, Renderer::EVertexLayout vertexLayout,
              Renderer::SPipelineDescriptors& pipeline)
        : CAbstractMaterial(type, vertexLayout, pipeline) {
    }

private:
};
} // namespace Material