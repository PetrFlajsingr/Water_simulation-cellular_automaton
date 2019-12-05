#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 4) uniform mat4 MVP;

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec3 fragColor;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec3 fragNormal;

void main() {
    vec4 pos = MVP * vec4(inPosition, 1.0);
    gl_Position = pos;
    fragTexCoord = inTexCoord;
    fragNormal = inNormal;
    fragPosition = inPosition;
    fragColor = inColor;
}
