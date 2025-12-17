#pragma once

namespace Scene {
class CSceneLoader {
public:
    CSceneLoader() = default;
    ~CSceneLoader() = default;

    void LoadSceneFromFile(const std::string& filePath);
};
} // namespace Scene
