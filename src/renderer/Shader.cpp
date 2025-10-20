#include "renderer/Shader.h"

#include "SDL3/SDL_gpu.h"

namespace Renderer {

CShader::CShader(SDL_GPUShader* shader, SDL_GPUDevice& device)
    : mDevice(device), mShader(shader) {
}

CShader::~CShader() {
    if (mShader) {
        SDL_ReleaseGPUShader(&mDevice, mShader);
    }
}

SDL_GPUShader* CShader::Get() const {
    return mShader;
}
} // namespace Renderer