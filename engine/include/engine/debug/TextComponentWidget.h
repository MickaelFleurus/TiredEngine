#pragma once

#include <string>
#include <vector>

#include "engine/debug/IOverlordItem.h"

namespace Component {
class CTextComponent;
}

namespace Utils {
class CFileHandler;
}

namespace Debug {
class CTextComponentWidget : public IOverlordItem {
public:
    CTextComponentWidget(Component::CTextComponent& textComponent,
                         Utils::CFileHandler& fileHandler);
    void Render() override;
    const char* GetName() const override;

private:
    Component::CTextComponent& mTextComponent;
    // Utils::CFileHandler& mFileHandler;
    const std::vector<std::string> mAvailableFonts;

    int mFontChoiceIndex = 0;
    std::string mCurrentText;
};
} // namespace Debug