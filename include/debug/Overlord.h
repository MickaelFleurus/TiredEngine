#pragma once
#include "utils/GuardedContainer.h"

class CToken;

namespace Renderer {
class CWindow;
} // namespace Renderer

namespace Debug {

class IOverlordItem;

class COverlord {
public:
    COverlord(Renderer::CWindow& window);

    static void AddMenu(IOverlordItem& item, CToken& token);
    void Render();

private:
    static CGuardedContainer<IOverlordItem> m_Items;
    Renderer::CWindow& m_Window;
};
} // namespace Debug