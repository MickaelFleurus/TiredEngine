#include "engine/core/CameraManager.h"

#include "engine/utils/FileHandler.h"

namespace Core {

CCameraManager::CCameraManager(Utils::CFileHandler& fileHandler)
    : mFileHandler(fileHandler) {
    // TODO: Eventually, load multiple cameras from config files
}

CCamera3D& CCameraManager::GetDefaultCamera3D() {
    return mDefaultCamera3D;
}

CCameraUI& CCameraManager::GetCameraUI() {
    return mCameraUI;
}

} // namespace Core