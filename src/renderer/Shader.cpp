#include "renderer/Shader.h"

#include "SDL3/SDL_gpu.h"

namespace Renderer {

CShader::CShader(SDL_GPUShader* shader, SDL_GPUDevice& device)
    : m_Device(device), m_Shader(shader) {
}

CShader::~CShader() {
    if (m_Shader) {
        SDL_ReleaseGPUShader(&m_Device, m_Shader);
    }
}
} // namespace Renderer