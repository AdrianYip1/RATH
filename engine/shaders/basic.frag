#version 450

const int MAX_LIGHTS = 3;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

struct R_Light {
    vec3 position;
    vec3 color;
};

layout(set = 0, binding = 1) uniform R_LightUbo {
    R_Light lights[MAX_LIGHTS];
    int lightCount;
} lightUbo;

layout(set = 1, binding = 0) uniform sampler2D texSampler;

layout (push_constant) uniform Push {
    mat4 model;
    vec3 color;
} PushConstants;

void main() {
    outColor = vec4(0.5 * PushConstants.color * fragColor * texture(texSampler, fragTexCoord).rgb, 1.0);
}