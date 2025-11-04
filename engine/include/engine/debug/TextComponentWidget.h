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

namespace Font {
class CFontHandler;
}

namespace Debug {
class CTextComponentWidget : public IOverlordItem {
public:
    CTextComponentWidget(Component::CTextComponent& textComponent,
                         Utils::CFileHandler& fileHandler,
                         Font::CFontHandler& fontHandler);
    void Render() override;
    const char* GetName() const override;

private:
    Component::CTextComponent& mTextComponent;
    Font::CFontHandler& mFontHandler;
    const std::vector<std::string> mAvailableFonts;

    std::string mCurrentText;
    std::string mCurrentPoliceName;
    int mFontChoiceIndex;
};
} // namespace Debug