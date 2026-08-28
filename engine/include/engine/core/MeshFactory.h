#pragma once
#include <expected>
#include <filesystem>

#include "engine/core/DataTypes.h"
#include "engine/core/Mesh.h"

namespace Core {
class CMeshManager;
class CMeshFactory {
public:
    CMeshFactory(CMeshManager& meshManager);

    void StartMassLoad();

    SMesh CreateTriangle();
    SMesh CreateCube(float size);
    SMesh CreateQuad(float width, float height, float depth);
    std::optional<SMesh> LoadFromFile(const std::filesystem::path& filePath);

    const SGlobalMeshBuffers& GetStagingBuffer() const;

private:
    CMeshManager& mMeshManager;
    SGlobalMeshBuffers mStagingBuffer;
    Meshes::SMeshAssetRegistry mRegistry;
};
} // namespace Core
