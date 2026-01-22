#pragma once

#include "engine/core/Camera3D.h"
#include "engine/core/CameraUI.h"

namespace Utils {
class CFileHandler;
} // namespace Utils

namespace Renderer {
class CTransformManager;
}

namespace Core {

class CCameraManager {
public:
    explicit CCameraManager(Renderer::CTransformManager& transformManager,
                            Utils::CFileHandler& fileHandler);

    CCamera3D& GetDefaultCamera3D();

    CCameraUI& GetCameraUI();

private:
    Renderer::CTransformManager& mTransformManager;
    Utils::CFileHandler& mFileHandler;

    CCamera3D mDefaultCamera3D;
    CCameraUI mCameraUI;
};
} // namespace Core
