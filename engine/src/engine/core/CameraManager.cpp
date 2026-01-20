#include "engine/core/CameraManager.h"

#include "engine/utils/FileHandler.h"

namespace Core {

CCameraManager::CCameraManager(Renderer::CTransformManager& transformManager,
                               Utils::CFileHandler& fileHandler)
    : mTransformManager(transformManager)
    , mFileHandler(fileHandler)
    , mDefaultCamera3D(transformManager) {
    // TODO: Eventually, load multiple cameras from config files
}

CCamera3D& CCameraManager::GetDefaultCamera3D() {
    return mDefaultCamera3D;
}

CCameraUI& CCameraManager::GetCameraUI() {
    return mCameraUI;
}

} // namespace Core
