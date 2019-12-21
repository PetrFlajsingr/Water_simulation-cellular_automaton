#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 4) uniform mat4 Model;
layout(location = 5) uniform mat4 View;
layout(location = 6) uniform mat4 Projection;
layout(location = 7) uniform vec3 cameraPosition;
layout(location = 8) uniform float cellSize;

layout(std430, binding=0) buffer Positions{
    vec4 positions[];
};

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec4 fragColor;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec3 fragNormal;
layout(location = 4) out vec3 fragCameraPosition;
layout(location = 5) out mat4 fragView;


void main() {
    float fluidVolume = positions[gl_InstanceID].w;
    vec3 cellPosition = positions[gl_InstanceID].xyz + vec3(0.5, 0.0, 0.5);
    vec3 vertexPos = inPosition.xyz;
    vec4 color = vec4(inColor, 1.0);
    if (fluidVolume <= 1.0){
        vertexPos.y *= fluidVolume;
        color.rgb *= 0.5 * (1-fluidVolume) + 0.5;
    }
    else if (fluidVolume > 1.0){
        color *= 0.5 * (fluidVolume/(1.0/3.0));
    }
    vec4 pos = vec4(vertexPos + cellPosition, 1.0);
    pos.xyz *= cellSize;
    fragPosition =(Model * pos).xyz;
    pos = Projection * View * Model * pos;
    gl_Position = pos;
    fragTexCoord = inTexCoord;
    fragNormal = mat3(transpose(inverse(Model))) * inNormal;
    fragColor = color;
    fragView = View;
    fragCameraPosition = cameraPosition;
}
