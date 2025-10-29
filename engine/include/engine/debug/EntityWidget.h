#pragma once
#include "engine/debug/IOverlordItem.h"

#include <memory>
#include <optional>

namespace Utils {
class CFileHandler;
}

namespace Component {
class CComponentManager;
} // namespace Component

namespace Debug {

class CTransformComponentWidget;
class CTextComponentWidget;

class CEntityWidget : public IOverlordItem {
public:
    CEntityWidget(Component::CComponentManager& componentManager,
                  Utils::CFileHandler& fileHandler);
    ~CEntityWidget() override;

    void OnItemClicked(std::optional<int> entityId);

    void Render() override;
    const char* GetName() const override;

private:
    Component::CComponentManager& mComponentManager;
    Utils::CFileHandler& mFileHandler;
    std::optional<int> mEntityId;

    std::unique_ptr<CTransformComponentWidget> mTransformWidget;
    std::unique_ptr<CTextComponentWidget> mTextWidget;
};
} // namespace Debug
