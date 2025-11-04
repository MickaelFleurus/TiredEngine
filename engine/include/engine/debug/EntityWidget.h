#pragma once
#include "engine/debug/IOverlordItem.h"

#include <memory>
#include <optional>
#include <string>

namespace Utils {
class CFileHandler;
}

namespace Component {
class CComponentManager;
} // namespace Component

namespace Font {
class CFontHandler;
}

namespace Debug {

class CTransformComponentWidget;
class CTextComponentWidget;
class CCameraWidget;

class CEntityWidget : public IOverlordItem {
public:
    CEntityWidget(Component::CComponentManager& componentManager,
                  Utils::CFileHandler& fileHandler,
                  Font::CFontHandler& fontHandler);
    ~CEntityWidget() override;

    void OnItemClicked(std::optional<int> entityId, std::string name = "");

    void Render() override;
    const char* GetName() const override;

    std::optional<int> GetEntityId() const;

private:
    Component::CComponentManager& mComponentManager;
    Utils::CFileHandler& mFileHandler;
    Font::CFontHandler& mFontHandler;

    std::optional<int> mEntityId;
    std::string mName;

    std::unique_ptr<CTransformComponentWidget> mTransformWidget;
    std::unique_ptr<CTextComponentWidget> mTextWidget;
    std::unique_ptr<CCameraWidget> mCameraWidget;
};
} // namespace Debug
