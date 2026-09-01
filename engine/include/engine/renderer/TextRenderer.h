#pragma once

#include "engine/core/DataTypes.h"

namespace Component {
class CManager;
}

namespace Font {
class CFontHandler;
}

namespace Renderer {

class CTextRenderer {
public:
    explicit CTextRenderer(Component::CManager& componentManager,
                           Font::CFontHandler& fontHandler);

    void Prepare();
    void Update();

private:
    Component::CManager& mComponentManager;
    Font::CFontHandler& mFontHandler;
};

} // namespace Renderer
