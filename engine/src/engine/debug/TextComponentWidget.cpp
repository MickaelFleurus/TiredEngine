#include "engine/debug/TextComponentWidget.h"

#include "engine/component/TextComponent.h"
#include "engine/font/FontHandler.h"
#include "engine/font/Police.h"
#include "engine/utils/FileHandler.h"
#include "engine/utils/StringUtils.h"
#include <imgui.h>
#include <imgui_stdlib.h>

namespace {
constexpr const char* kFontExtension = ".ttf";

int GetFontIndexByName(const std::vector<std::string>& availableFonts,
                       const std::string& policeName) {
    for (size_t i = 0; i < availableFonts.size(); ++i) {
        if (Utils::CompareIgnoreCase(availableFonts[i], policeName)) {
            return static_cast<int>(i);
        }
    }
    return 0;
}
} // namespace

namespace Debug {
CTextComponentWidget::CTextComponentWidget(
    Component::CTextComponent& textComponent, Utils::CFileHandler& fileHandler,
    Font::CFontHandler& fontHandler)
    : mTextComponent(textComponent)
    , mFontHandler(fontHandler)
    , mAvailableFonts(fileHandler.GetFileNames(kFontExtension))
    , mCurrentText(textComponent.getText())
    , mFontSize(mTextComponent.getPolice()->GetSize())
    , mFontColor(textComponent.getPolice()->GetColor())
    , mFontChoiceIndex(GetFontIndexByName(
          mAvailableFonts, mTextComponent.getPolice()->GetName())) {
    SetVisible(true);
}

void CTextComponentWidget::Render() {

    if (ImGui::InputText("Text", &mCurrentText)) {
        mTextComponent.setText(mCurrentText);
    }
    if (ImGui::BeginCombo("Font", mAvailableFonts[mFontChoiceIndex].c_str())) {
        for (int n = 0; n < static_cast<int>(mAvailableFonts.size()); n++) {
            const bool isSelected = (mFontChoiceIndex == n);
            if (ImGui::Selectable(mAvailableFonts[n].c_str(), isSelected)) {
                mFontChoiceIndex = n;
                mTextComponent.setPolice(
                    &mFontHandler.GetPolice(mAvailableFonts[n].c_str(), 20));
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    if (ImGui::SliderInt("Font Size", &mFontSize, 8, 72)) {
        Font::CPolice* police = mTextComponent.getPolice();
        if (police) {
            police->SetSize(static_cast<unsigned int>(mFontSize));
            mTextComponent.setDirty(true);
        }
    }

    if (ImGui::ColorEdit4("Font Color", &mFontColor.r)) {
        Font::CPolice* police = mTextComponent.getPolice();
        if (police) {
            police->SetColor(mFontColor);
            mTextComponent.setDirty(true);
        }
    }
}

const char* CTextComponentWidget::GetName() const {
    return "Text Component";
}
} // namespace Debug