#include "engine/debug/AssetExplorerWidget.h"

#include <algorithm>
#include <array>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

#include <imgui.h>
#include <imgui_stdlib.h>

#include "engine/utils/FileHandler.h"

namespace Debug {
namespace {
constexpr std::array<std::string_view, 4> kHiddenAssetNames{
    ".asset_sources", ".DS_Store", ".DS_store", "Thumbs.db"};

bool IsHiddenAsset(const std::filesystem::path& path) {
    const auto filename = path.filename().string();
    return std::ranges::find(kHiddenAssetNames, filename) !=
           kHiddenAssetNames.end();
}
} // namespace

CAssetExplorerWidget::CAssetExplorerWidget(
    const Utils::CFileHandler& fileHandler)
    : mFileHandler(fileHandler) {
    SetVisible(true);
}

void CAssetExplorerWidget::Render() {
    if (!ImGui::Begin("Assets", &mVisible)) {
        ImGui::End();
        return;
    }

    const std::filesystem::path root{mFileHandler.GetAssetsFolder()};
    std::error_code error;
    if (mSelectedFolder.empty() ||
        !std::filesystem::exists(mSelectedFolder, error) || error) {
        mSelectedFolder = root;
        mSelectedAsset.clear();
    }

    ImGui::SetNextItemWidth(-1.0f);
    ImGui::InputTextWithHint("##AssetFilter", "Filter assets", &mFilter);
    ImGui::Separator();

    if (!std::filesystem::exists(root, error) || error) {
        ImGui::TextDisabled("Asset directory is unavailable.");
        ImGui::TextWrapped("%s", root.string().c_str());
    } else if (ImGui::BeginTable(
                   "AssetBrowser", 2,
                   ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV |
                       ImGuiTableFlags_SizingStretchProp,
                   ImVec2(0.0f, ImGui::GetContentRegionAvail().y))) {
        ImGui::TableSetupColumn("Folders", ImGuiTableColumnFlags_WidthFixed,
                                240.0f);
        ImGui::TableSetupColumn("Files", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::PushStyleColor(ImGuiCol_ChildBg,
                      ImVec4(0.105f, 0.115f, 0.13f, 1.0f));
        if (ImGui::BeginChild("AssetFolders", ImVec2(0.0f, 0.0f))) {
            const bool rootOpen = ImGui::TreeNodeEx(
                "assets",
                ImGuiTreeNodeFlags_DefaultOpen |
                    ImGuiTreeNodeFlags_OpenOnArrow |
                    ImGuiTreeNodeFlags_SpanAvailWidth |
                    (mSelectedFolder == root ? ImGuiTreeNodeFlags_Selected
                                             : ImGuiTreeNodeFlags_None));
            if (ImGui::IsItemClicked()) {
                mSelectedFolder = root;
                mSelectedAsset.clear();
            }
            if (rootOpen) {
                RenderDirectory(root, root);
                ImGui::TreePop();
            }
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();

        ImGui::TableSetColumnIndex(1);
        ImGui::PushStyleColor(ImGuiCol_ChildBg,
                              ImVec4(0.075f, 0.085f, 0.10f, 1.0f));
        if (ImGui::BeginChild("AssetFiles", ImVec2(0.0f, 0.0f))) {
            constexpr float detailsHeight = 108.0f;
            if (ImGui::BeginChild(
                    "AssetFileList",
                    ImVec2(0.0f, -detailsHeight -
                                     ImGui::GetStyle().ItemSpacing.y),
                    ImGuiChildFlags_None)) {
                RenderFiles(mSelectedFolder, root);
            }
            ImGui::EndChild();
            ImGui::Separator();
            ImGui::PushStyleColor(ImGuiCol_ChildBg,
                                  ImVec4(0.13f, 0.14f, 0.155f, 1.0f));
            if (ImGui::BeginChild("AssetDetails", ImVec2(0.0f, 0.0f),
                                  ImGuiChildFlags_None)) {
                RenderFileDetails(root);
            }
            ImGui::EndChild();
            ImGui::PopStyleColor();
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::EndTable();
    }
    ImGui::End();
}

void CAssetExplorerWidget::RenderDirectory(
    const std::filesystem::path& directory, const std::filesystem::path& root) {
    std::error_code error;
    std::vector<std::filesystem::directory_entry> entries;
    for (std::filesystem::directory_iterator it(directory, error), end;
         !error && it != end; it.increment(error)) {
        if (!IsHiddenAsset(it->path())) {
            entries.push_back(*it);
        }
    }

    std::ranges::sort(entries, [](const auto& left, const auto& right) {
        std::error_code leftError;
        std::error_code rightError;
        const bool leftDirectory = left.is_directory(leftError);
        const bool rightDirectory = right.is_directory(rightError);
        if (leftDirectory != rightDirectory) {
            return leftDirectory;
        }
        return left.path().filename().string() <
               right.path().filename().string();
    });

    if (error) {
        ImGui::TextDisabled("Unable to read %s", directory.string().c_str());
        return;
    }

    for (const auto& entry : entries) {
        std::error_code typeError;
        if (!entry.is_directory(typeError) || typeError) {
            continue;
        }
        const auto relative =
            std::filesystem::relative(entry.path(), root, typeError);
        if (typeError) {
            continue;
        }
        const auto relativeName = relative.generic_string();
        ImGui::PushID(relativeName.c_str());
        const bool selected = mSelectedFolder == entry.path();
        const bool open = ImGui::TreeNodeEx(
            entry.path().filename().string().c_str(),
            ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow |
                (selected ? ImGuiTreeNodeFlags_Selected
                          : ImGuiTreeNodeFlags_None));
        if (ImGui::IsItemClicked()) {
            mSelectedFolder = entry.path();
            mSelectedAsset.clear();
        }
        if (open) {
            RenderDirectory(entry.path(), root);
            ImGui::TreePop();
        }
        ImGui::PopID();
    }
}

void CAssetExplorerWidget::RenderFiles(const std::filesystem::path& directory,
                                       const std::filesystem::path& root) {
    ImGui::Text(
        "%s",
        std::filesystem::relative(directory, root).generic_string().c_str());
    ImGui::Separator();

    std::error_code error;
    std::vector<std::filesystem::directory_entry> files;
    for (std::filesystem::directory_iterator it(directory, error), end;
         !error && it != end; it.increment(error)) {
        if (IsHiddenAsset(it->path())) {
            continue;
        }
        std::error_code typeError;
        if (it->is_regular_file(typeError) && !typeError) {
            files.push_back(*it);
        }
    }
    std::ranges::sort(files, [](const auto& left, const auto& right) {
        return left.path().filename().string() <
               right.path().filename().string();
    });

    if (error) {
        ImGui::TextDisabled("Unable to read this folder.");
        return;
    }

    bool foundFile = false;
    for (const auto& file : files) {
        const auto relative =
            std::filesystem::relative(file.path(), root, error);
        if (error) {
            error.clear();
            continue;
        }
        if (!mFilter.empty() &&
            relative.generic_string().find(mFilter) == std::string::npos) {
            continue;
        }
        foundFile = true;
        const bool selected = mSelectedAsset == file.path();
        if (ImGui::Selectable(file.path().filename().string().c_str(),
                              selected)) {
            mSelectedAsset = file.path();
        }
    }

    if (!foundFile) {
        ImGui::TextDisabled(mFilter.empty() ? "This folder contains no files."
                                            : "No files match the filter.");
    }
}

void CAssetExplorerWidget::RenderFileDetails(
    const std::filesystem::path& root) {
    if (mSelectedAsset.empty()) {
        ImGui::TextDisabled("Select an asset to view its details.");
        return;
    }

    std::error_code error;
    if (!std::filesystem::exists(mSelectedAsset, error) || error) {
        mSelectedAsset.clear();
        ImGui::TextDisabled("The selected asset no longer exists.");
        return;
    }

    const auto relative =
        std::filesystem::relative(mSelectedAsset, root, error);
    const auto relativeName =
        error ? mSelectedAsset.filename().string() : relative.generic_string();
    ImGui::Text("%s", relativeName.c_str());

    if (std::filesystem::is_regular_file(mSelectedAsset, error) && !error) {
        const auto size = std::filesystem::file_size(mSelectedAsset, error);
        if (!error) {
            ImGui::TextDisabled("%llu bytes  |  %s",
                                static_cast<unsigned long long>(size),
                                mSelectedAsset.extension().string().c_str());
        }
        const auto source = mFileHandler.ResolveSourceAssetPath(mSelectedAsset);
        ImGui::TextWrapped("Runtime: %s", mSelectedAsset.string().c_str());
        if (source != mSelectedAsset) {
            ImGui::TextWrapped("Source: %s", source.string().c_str());
        }
    }
}

const char* CAssetExplorerWidget::GetName() const {
    return "Assets";
}
} // namespace Debug
