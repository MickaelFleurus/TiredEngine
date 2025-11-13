#pragma once

#include "engine/material/AbstractMaterial.h"

namespace Material {
class CMaterial : public AbstractMaterial {
public:
    CMaterial(EMaterialType type, VkPipeline pipeline)
        : AbstractMaterial(type, pipeline) {
    }

private:
};
} // namespace Material