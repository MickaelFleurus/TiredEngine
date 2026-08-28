// #include "engine/debug/TextUIComponentWidget.h"

// #include <imgui.h>
// #include <imgui_stdlib.h>

// #include "engine/component/TextUIComponent.h"
// #include "engine/font/FontHandler.h"
// #include "engine/font/Police.h"
// #include "engine/utils/AssetParser.h"
// #include "engine/utils/FileHandler.h"
// #include "engine/utils/StringUtils.h"

// namespace {
// int GetFontIndexByName(const std::vector<SAsset>& availableFonts,
//                        const std::string& policeName) {
//     for (size_t i = 0; i < availableFonts.size(); ++i) {
//         if (Utils::CompareIgnoreCase(availableFonts[i].mPath.stem().string(),
//                                      policeName)) {
//             return static_cast<int>(i);
//         }
//     }
//     return 0;
// }
// } // namespace

// namespace Debug {
// CTextUIComponentWidget::CTextUIComponentWidget(
//     Component::CTextUIComponent& textComponent,
//     Utils::CFileHandler& fileHandler, Font::CFontHandler& fontHandler,
//     const CAssetParser& fileParser)
//     : mTextComponent(textComponent)
//     , mFontHandler(fontHandler)
//     , mFileParser(fileParser)
//     , mAvailableFonts(fileParser.Get(EAssetType::Font))
//     , mCurrentText(textComponent.GetText())
//     , mFontSize(mTextComponent.GetFontSize())
//     , mFontColor(textComponent.GetColor())
//     , mFontChoiceIndex(GetFontIndexByName(
//           mAvailableFonts, mTextComponent.GetPolice()
//                                ? mTextComponent.GetPolice()->GetName()
//                                : "None")) {
//     SetVisible(true);
// }

// void CTextUIComponentWidget::Render() {

//     if (ImGui::InputTextMultiline("Text", &mCurrentText)) {
//         mTextComponent.SetText(mCurrentText);
//     }
//     if (ImGui::BeginCombo(
//             "Font", mAvailableFonts[mFontChoiceIndex].mPath.stem().c_str()))
//             {
//         for (int n = 0; n < static_cast<int>(mAvailableFonts.size()); n++) {
//             const bool isSelected = (mFontChoiceIndex == n);
//             if (ImGui::Selectable(mAvailableFonts[n].mPath.stem().c_str(),
//                                   isSelected)) {
//                 mFontChoiceIndex = n;
//                 mTextComponent.SetPolice(&mFontHandler.GetPolice(
//                     mAvailableFonts[n].mPath.stem().string()));
//             }
//             if (isSelected) {
//                 ImGui::SetItemDefaultFocus();
//             }
//         }
//         ImGui::EndCombo();
//     }
//     if (ImGui::SliderInt("Font Size", &mFontSize, 8, 72)) {
//         mTextComponent.SetFontSize(static_cast<unsigned int>(mFontSize));
//     }

//     if (ImGui::ColorEdit4("Font Color", &mFontColor.r)) {
//         mTextComponent.SetColor(mFontColor);
//     }
// }

// const char* CTextUIComponentWidget::GetName() const {
//     return "Text Component";
// }
// } // namespace Debug
