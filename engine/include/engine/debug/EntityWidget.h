#pragma once
#include <memory>
#include <optional>
#include <string>

#include "engine/component/ComponentType.h"
#include "engine/core/GameObjectId.h"
#include "engine/debug/IOverlordItem.h"

namespace Utils {
class CFileHandler;
}

namespace Component {
class CComponentManager;
} // namespace Component

namespace Core {
class CGameObject;
}

namespace Font {
class CFontHandler;
}

namespace Renderer {
class CTransformManager;
}

namespace Debug {

class CTransformWidget;
class CTextUIComponentWidget;
class CCameraComponentWidget;
class CMeshComponentWidget;

class CEntityWidget : public IOverlordItem {
public:
    CEntityWidget(Component::CComponentManager& componentManager,
                  Utils::CFileHandler& fileHandler,
                  Font::CFontHandler& fontHandler,
                  Renderer::CTransformManager& transformManager);
    ~CEntityWidget() override;

    void OnItemClicked(std::optional<Core::GameObjectId> id);

    void Render() override;
    const char* GetName() const override;
    bool IsSelected(Core::GameObjectId id) const;

private:
    void RenderEntityHeader();
    void RenderComponentsSection();
    void RenderComponentWithHeader(Component::EComponentType componentType,
                                   IOverlordItem* widget);
    void RenderAddComponentSection();
    void RenderAddComponentPopup();

    Component::CComponentManager& mComponentManager;
    Utils::CFileHandler& mFileHandler;
    Font::CFontHandler& mFontHandler;
    Renderer::CTransformManager& mTransformManager;

    std::optional<Core::GameObjectId> mId{std::nullopt};

    std::unique_ptr<CTransformWidget> mTransformWidget;
    std::unique_ptr<CTextUIComponentWidget> mTextWidget;
    std::unique_ptr<CCameraComponentWidget> mCameraWidget;
    std::unique_ptr<CMeshComponentWidget> mMeshWidget;

    // UI state for collapsible sections
    bool mTransformExpanded = true;
    bool mTextUIExpanded = false;
    bool mCameraExpanded = false;
    bool mMeshExpanded = false;
};
} // namespace Debug
