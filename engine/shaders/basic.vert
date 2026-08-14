#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout (set = 0, binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    float deltaTime;
} ubo;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragNormal;
layout(location = 3) out vec3 modelLocation;

layout (push_constant) uniform Push {
    mat4 model;
    vec3 color;
} PushConstants;

void main() {
    gl_Position = ubo.proj * ubo.view * PushConstants.model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    // Rotate normal by the model matrix
    fragNormal = mat3(PushConstants.model) * inNormal;
    modelLocation = (PushConstants.model * vec4(inPosition, 1.0)).xyz;
}