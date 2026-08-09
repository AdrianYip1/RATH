#version 450

layout (binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    float deltaTime;
} ubo;

layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 color;

layout (location = 0) out vec3 fragColor;

void main() {
	gl_PointSize = 14.0;
	gl_Position = ubo.proj * ubo.view * vec4(pos, 1.0);
	fragColor = color.rgb;
}