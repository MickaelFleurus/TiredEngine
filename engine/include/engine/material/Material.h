#pragma once

#include "engine/material/AbstractMaterial.h"

namespace Material {
class CMaterial : public AbstractMaterial {
public:
    CMaterial(EMaterialType type, Renderer::EVertexLayout vertexLayout,
              Renderer::SPipelineDescriptors& pipeline)
        : AbstractMaterial(type, vertexLayout, pipeline) {
    }

private:
};
} // namespace Material