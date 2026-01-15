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

namespace Debug {

class CTransformComponentWidget;
class CTextUIComponentWidget;
class CCameraWidget;
class CMeshComponentWidget;

class CEntityWidget : public IOverlordItem {
public:
    CEntityWidget(Component::CComponentManager& componentManager,
                  Utils::CFileHandler& fileHandler,
                  Font::CFontHandler& fontHandler);
    ~CEntityWidget() override;

    void OnItemClicked(Core::CGameObject* obj);

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

    std::optional<Core::CGameObject*> mObj;

    std::unique_ptr<CTransformComponentWidget> mTransformWidget;
    std::unique_ptr<CTextUIComponentWidget> mTextWidget;
    std::unique_ptr<CCameraWidget> mCameraWidget;
    std::unique_ptr<CMeshComponentWidget> mMeshWidget;

    // UI state for collapsible sections
    bool mTransformExpanded = true;
    bool mTextUIExpanded = false;
    bool mCameraExpanded = false;
    bool mMeshExpanded = false;
};
} // namespace Debug
