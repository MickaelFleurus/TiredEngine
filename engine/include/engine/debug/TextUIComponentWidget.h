#pragma once

#include <string>
#include <vector>

#include <glm/vec4.hpp>

#include "engine/debug/IOverlordItem.h"
#include "engine/utils/Asset.h"

namespace Component {
class CTextUIComponent;
}

namespace Utils {
class CFileHandler;
}

namespace Font {
class CFontHandler;
}
class CAssetParser;
namespace Debug {
class CTextUIComponentWidget : public IOverlordItem {
public:
    CTextUIComponentWidget(Component::CTextUIComponent& textComponent,
                           Utils::CFileHandler& fileHandler,
                           Font::CFontHandler& fontHandler,
                           const CAssetParser& fileParser);
    void Render() override;
    const char* GetName() const override;

private:
    Component::CTextUIComponent& mTextComponent;
    Font::CFontHandler& mFontHandler;
    const CAssetParser& mFileParser;
    const std::vector<SAsset> mAvailableFonts;

    std::string mCurrentText;
    std::string mCurrentPoliceName;
    int mFontChoiceIndex;
    int mFontSize;
    glm::vec4 mFontColor;
};
} // namespace Debug
