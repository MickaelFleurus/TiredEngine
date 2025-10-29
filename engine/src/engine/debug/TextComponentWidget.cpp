#include "engine/debug/TextComponentWidget.h"

#include "engine/component/TextComponent.h"
#include "engine/utils/FileHandler.h"

#include <imgui.h>
#include <imgui_stdlib.h>

namespace Debug {
CTextComponentWidget::CTextComponentWidget(
    Component::CTextComponent& textComponent, Utils::CFileHandler& fileHandler)
    : mTextComponent(textComponent)
    , mAvailableFonts(fileHandler.GetFonts())
    , mCurrentText(textComponent.getText()) {
    SetVisible(true);
}

void CTextComponentWidget::Render() {

    if (ImGui::InputText("Text", &mCurrentText)) {
        mTextComponent.updateText(mCurrentText);
    }
    if (ImGui::BeginCombo("Font", mAvailableFonts[mFontChoiceIndex].c_str())) {
        for (int n = 0; n < static_cast<int>(mAvailableFonts.size()); n++) {
            const bool isSelected = (mFontChoiceIndex == n);
            if (ImGui::Selectable(mAvailableFonts[n].c_str(), isSelected)) {
                mFontChoiceIndex = n;
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}

const char* CTextComponentWidget::GetName() const {
    return "Text Component";
}
} // namespace Debug