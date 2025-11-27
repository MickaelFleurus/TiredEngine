#pragma once

#include "engine/font/Police.h"
#include "engine/renderer/RendererUtils.h"
#include <string>
#include <unordered_map>

namespace Utils {
class CFileHandler;
}

namespace Material {
class CMaterialFactory;
}

namespace Renderer {
class CTextureManager;
class CVertexBufferHandleWrapper;
class CIndexesBufferHandleWrapper;
} // namespace Renderer

namespace Font {
class CFontHandler {
public:
    CFontHandler(Renderer::CTextureManager& textureManager,
                 Utils::CFileHandler& fileHandler,
                 Material::CMaterialFactory& materialFactory,
                 Renderer::CVertexBufferHandleWrapper& vertexBufferHandle,
                 Renderer::CIndexesBufferHandleWrapper& indexesBufferHandle);
    ~CFontHandler();

    CPolice& GetPolice(std::string name);

private:
    std::unordered_map<std::string, CPolice> mPolices;
    Renderer::CTextureManager& mTextureManager;
    Utils::CFileHandler& mFileHandler;
    Material::CMaterialFactory& mMaterialFactory;
    Renderer::CVertexBufferHandleWrapper& mVertexBufferHandle;
    Renderer::CIndexesBufferHandleWrapper& mIndexesBufferHandle;
};
} // namespace Font