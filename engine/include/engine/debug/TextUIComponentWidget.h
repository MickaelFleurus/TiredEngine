#pragma once

#include <string>
#include <vector>

#include <glm/vec4.hpp>

#include "engine/debug/IOverlordItem.h"

namespace Component {
class CTextUIComponent;
}

namespace Utils {
class CFileHandler;
}

namespace Font {
class CFontHandler;
}

namespace Debug {
class CTextUIComponentWidget : public IOverlordItem {
public:
    CTextUIComponentWidget(Component::CTextUIComponent& textComponent,
                           Utils::CFileHandler& fileHandler,
                           Font::CFontHandler& fontHandler);
    void Render() override;
    const char* GetName() const override;

private:
    Component::CTextUIComponent& mTextComponent;
    Font::CFontHandler& mFontHandler;
    const std::vector<std::string> mAvailableFonts;

    std::string mCurrentText;
    std::string mCurrentPoliceName;
    int mFontChoiceIndex;
    int mFontSize;
    glm::vec4 mFontColor;
};
} // namespace Debug
