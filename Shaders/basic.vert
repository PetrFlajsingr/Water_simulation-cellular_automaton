#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 4) uniform mat4 Model;
layout(location = 5) uniform mat4 View;
layout(location = 6) uniform mat4 Projection;
layout(location = 7) uniform vec3 cameraPosition;

layout(std430, binding=0) buffer Positions{
    vec4 positions[];
};

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec3 fragColor;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec3 fragNormal;
layout(location = 4) out vec3 fragCameraPosition;
layout(location = 5) out mat4 fragView;


void main() {
    vec4 pos = vec4(inPosition, 1.0) + positions[gl_InstanceID];
    pos.xyz *= 0.1;
    /*pos.x += gl_InstanceID % 5;
    pos.y -= (gl_InstanceID / 5) % 5;
    pos.z -= gl_InstanceID / (25);*/
    fragPosition =(Model * pos).xyz;
    pos = Projection * View * Model * pos;
    gl_Position = pos;
    fragTexCoord = inTexCoord;
    fragNormal = mat3(transpose(inverse(Model))) * inNormal;
    fragColor = inColor;
    fragView = View;
    fragCameraPosition = cameraPosition;
}
