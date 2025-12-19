#pragma once

#include <vector>

#include "engine/debug/IOverlordItem.h"

namespace Utils {
class CFileHandler;
}

namespace Scene {
class CSceneLoader;
}

namespace Debug {

class CLevelSceneLoaderWidget : public IOverlordItem {
public:
    explicit CLevelSceneLoaderWidget(Utils::CFileHandler& fileHandler,
                                     Scene::CSceneLoader& sceneLoader);
    void Render() override;
    const char* GetName() const override;

private:
    Utils::CFileHandler& mFileHandler;
    Scene::CSceneLoader& mSceneLoader;

    std::vector<std::string> mSceneFiles;
    int mSelectedSceneIndex{-1};
    bool mNeedsRefresh{true};
    std::string mSelectedScenePath;
};
} // namespace Debug
