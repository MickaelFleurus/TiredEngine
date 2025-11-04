#version 450


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 2) in vec2 instancePosition;
layout(location = 3) in vec2 instanceSize;
layout(location = 4) in vec4 instanceUVRect;
layout(location = 5) in vec4 instanceColor;

layout(push_constant) uniform SMatrices {
    mat4 projection;
    mat4 view;
    mat4 model;  // Add model matrix
} pc;


layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec4 fragColor;

void main() {
    vec2 scaledPosition = inPosition.xy * instanceSize;

    vec2 localPosition = scaledPosition + instancePosition;

    gl_Position = pc.projection * pc.view * pc.model * vec4(localPosition, 0.0, 1.0);
    fragTexCoord = instanceUVRect.xy + inTexCoord * instanceUVRect.zw;
    fragColor = instanceColor;
}