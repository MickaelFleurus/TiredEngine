#version 450


layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;


layout(location = 3) in mat4 modelMatrix;
layout(location = 7) in vec4 instanceColor;
layout(location = 8) in uint instanceTextureIndex;
layout(location = 9) in uint instanceMaterialIndex;

layout(push_constant) uniform PushConstants {
    mat4 projection;
    mat4 view;
} pc;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragUV;
layout(location = 2) out vec3 fragNormal;

void main() {
    gl_Position = pc.projection * pc.view * modelMatrix * vec4(vertexPosition, 1.0);
    
    // Transform normal by inverse transpose of model matrix
    fragNormal = normalize(mat3(transpose(inverse(modelMatrix))) * vertexNormal);
    
    fragColor = instanceColor;
    fragUV = vertexUV;
}