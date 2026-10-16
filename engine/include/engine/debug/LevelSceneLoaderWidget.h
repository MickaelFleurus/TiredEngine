#pragma once

#include <string>
#include <vector>

#include "engine/debug/IOverlordItem.h"
#include "engine/utils/Asset.h"

namespace Scene {
class CSceneLoader;
}
class CAssetParser;
namespace Debug {

class CLevelSceneLoaderWidget : public IOverlordItem {
public:
    explicit CLevelSceneLoaderWidget(const CAssetParser& assetParser,
                                     Scene::CSceneLoader& sceneLoader);
    void Render() override;
    const char* GetName() const override;

private:
    const CAssetParser& mAssetParser;
    Scene::CSceneLoader& mSceneLoader;

    std::vector<SAsset> mLevels;
    int mSelectedSceneIndex{-1};
    bool mNeedsRefresh{true};
    std::filesystem::path mSelectedScenePath;
};
} // namespace Debug
