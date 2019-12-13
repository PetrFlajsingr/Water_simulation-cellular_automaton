#version 450

layout(location = 0) in vec3 inPosition;

layout(location = 4) uniform mat4 Model;
layout(location = 5) uniform mat4 View;
layout(location = 6) uniform mat4 Projection;

void main() {
    vec3 pos = inPosition;
    pos *= 0.5;
    gl_Position = Projection * View * Model * vec4(pos, 1.0);
}
