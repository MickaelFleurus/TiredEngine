#pragma once

#include "engine/core/Camera3D.h"
#include "engine/core/CameraUI.h"

namespace Utils {
class CFileHandler;
} // namespace Utils

namespace Core {

class CCameraManager {
public:
    explicit CCameraManager(Utils::CFileHandler& fileHandler);

    CCamera3D& GetDefaultCamera3D();

    CCameraUI& GetCameraUI();

private:
    Utils::CFileHandler& mFileHandler;

    CCamera3D mDefaultCamera3D;
    CCameraUI mCameraUI;
};
} // namespace Core