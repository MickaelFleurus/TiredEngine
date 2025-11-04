#include "engine/renderer/RendererUtils.h"

namespace {
Renderer::VertexLayoutInfo CreateSimpleVertexLayout() {
    Renderer::VertexLayoutInfo info;

    // Buffer 0: Position + TexCoord (for basic sprites/quads)
    SDL_GPUVertexAttribute attr0{};
    attr0.location = 0;
    attr0.buffer_slot = 0;
    attr0.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
    attr0.offset = 0;

    SDL_GPUVertexAttribute attr1{};
    attr1.location = 1;
    attr1.buffer_slot = 0;
    attr1.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    attr1.offset = sizeof(float) * 3;

    info.attributes = {attr0, attr1};

    SDL_GPUVertexBufferDescription bufferDesc{};
    bufferDesc.slot = 0;
    bufferDesc.pitch = sizeof(float) * 5; // vec3 + vec2
    bufferDesc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    bufferDesc.instance_step_rate = 0;

    info.bufferDescriptions = {bufferDesc};

    return info;
}

Renderer::VertexLayoutInfo CreateInstancedVertexLayout() {
    Renderer::VertexLayoutInfo info;

    // Buffer 0: Position + TexCoord (per-vertex)
    SDL_GPUVertexAttribute attr0{};
    attr0.location = 0;
    attr0.buffer_slot = 0;
    attr0.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
    attr0.offset = 0;

    SDL_GPUVertexAttribute attr1{};
    attr1.location = 1;
    attr1.buffer_slot = 0;
    attr1.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    attr1.offset = sizeof(float) * 3;

    // Buffer 1: Instance data
    SDL_GPUVertexAttribute attr2{}; // instancePosition
    attr2.location = 2;
    attr2.buffer_slot = 1;
    attr2.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    attr2.offset = 0;

    SDL_GPUVertexAttribute attr3{}; // instanceSize
    attr3.location = 3;
    attr3.buffer_slot = 1;
    attr3.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    attr3.offset = sizeof(float) * 2;

    SDL_GPUVertexAttribute attr4{}; // instanceUVRect
    attr4.location = 4;
    attr4.buffer_slot = 1;
    attr4.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
    attr4.offset = sizeof(float) * 4;

    SDL_GPUVertexAttribute attr5{}; // instanceColor
    attr5.location = 5;
    attr5.buffer_slot = 1;
    attr5.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
    attr5.offset = sizeof(float) * 8;

    info.attributes = {attr0, attr1, attr2, attr3, attr4, attr5};

    SDL_GPUVertexBufferDescription vertexBufferDesc{};
    vertexBufferDesc.slot = 0;
    vertexBufferDesc.pitch = sizeof(float) * 5;
    vertexBufferDesc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    vertexBufferDesc.instance_step_rate = 0;

    SDL_GPUVertexBufferDescription instanceBufferDesc{};
    instanceBufferDesc.slot = 1;
    instanceBufferDesc.pitch = sizeof(float) * 12; // vec2 + vec2 + vec4 + vec4
    instanceBufferDesc.input_rate = SDL_GPU_VERTEXINPUTRATE_INSTANCE;
    instanceBufferDesc.instance_step_rate = 0;

    info.bufferDescriptions = {vertexBufferDesc, instanceBufferDesc};

    return info;
}

Renderer::VertexLayoutInfo CreateMeshVertexLayout() {
    Renderer::VertexLayoutInfo info;

    // Buffer 0: Position + Normal + TexCoord (for 3D meshes)
    SDL_GPUVertexAttribute attr0{};
    attr0.location = 0;
    attr0.buffer_slot = 0;
    attr0.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
    attr0.offset = 0;

    SDL_GPUVertexAttribute attr1{}; // Normal
    attr1.location = 1;
    attr1.buffer_slot = 0;
    attr1.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
    attr1.offset = sizeof(float) * 3;

    SDL_GPUVertexAttribute attr2{}; // TexCoord
    attr2.location = 2;
    attr2.buffer_slot = 0;
    attr2.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    attr2.offset = sizeof(float) * 6;

    info.attributes = {attr0, attr1, attr2};

    SDL_GPUVertexBufferDescription bufferDesc{};
    bufferDesc.slot = 0;
    bufferDesc.pitch = sizeof(float) * 8; // vec3 + vec3 + vec2
    bufferDesc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    bufferDesc.instance_step_rate = 0;

    info.bufferDescriptions = {bufferDesc};

    return info;
}

} // namespace

namespace Renderer {
VertexLayoutInfo CreateVertexLayout(Renderer::EVertexLayout layoutType) {
    switch (layoutType) {
    case Renderer::EVertexLayout::Simple:
        return CreateSimpleVertexLayout();
    case Renderer::EVertexLayout::Instanced:
        return CreateInstancedVertexLayout();
    case Renderer::EVertexLayout::Mesh3D:
        return CreateMeshVertexLayout();
    default:
        return CreateSimpleVertexLayout();
    }
}
} // namespace Renderer