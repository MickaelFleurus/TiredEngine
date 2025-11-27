#version 450


layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;


layout(location = 2) in mat4 modelMatrix;
layout(location = 6) in vec4 instanceColor;
layout(location = 7) in uint instanceTextureIndex;
layout(location = 8) in uint instanceMaterialIndex;

layout(push_constant) uniform PushConstants {
    mat4 projection;
    mat4 view;
} pc;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec4 fragColor;
layout(location = 2) out flat uint fragTextureIndex;
layout(location = 3) out flat uint fragMaterialIndex;

void main() {
    gl_Position = pc.projection * pc.view * modelMatrix * vec4(vertexPosition, 1.0);
    fragTexCoord =  vertexUV;
    fragColor = instanceColor;
    fragTextureIndex = instanceTextureIndex;
    fragMaterialIndex = instanceMaterialIndex;
}