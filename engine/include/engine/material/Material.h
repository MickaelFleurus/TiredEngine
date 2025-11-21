#pragma once

#include "engine/material/AbstractMaterial.h"

namespace Material {
class CMaterial : public AbstractMaterial {
public:
    CMaterial(const Renderer::CDescriptorLayoutStorage& descriptorLayoutStorage,
              EMaterialType type, Renderer::EVertexLayout vertexLayout,
              Renderer::SPipelineDescriptors& pipeline)
        : AbstractMaterial(descriptorLayoutStorage, type, vertexLayout,
                           pipeline) {
    }

private:
};
} // namespace Material