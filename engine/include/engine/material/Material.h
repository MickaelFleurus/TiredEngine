#pragma once

#include "engine/material/AbstractMaterial.h"

namespace Material {
class CMaterial : public AbstractMaterial {
public:
    CMaterial(EMaterialType type, SDL_GPUGraphicsPipeline* pipeline)
        : AbstractMaterial(type, pipeline) {
    }

private:
};
} // namespace Material