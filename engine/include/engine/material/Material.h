#pragma once

#include "engine/material/AbstractMaterial.h"

namespace Material {
class CMaterial : public CAbstractMaterial {
public:
    CMaterial(EMaterialType type, Renderer::SPipelineDescriptors& pipeline)
        : CAbstractMaterial(type, pipeline) {
    }

private:
};
} // namespace Material
