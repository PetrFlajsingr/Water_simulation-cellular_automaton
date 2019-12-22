#version 450

struct Cell{
    float fluidVolume;
    float solidVolume;
    float fluidHorizontalLefover;
    uint stable;
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 4) uniform mat4 Model;
layout(location = 5) uniform mat4 View;
layout(location = 6) uniform mat4 Projection;
layout(location = 7) uniform vec3 cameraPosition;
layout(location = 8) uniform float cellSize;
layout(location = 9) uniform uvec3 tankSize;

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec4 fragColor;
layout(location = 4) out vec3 fragCameraPosition;
layout(location = 5) out mat4 fragView;
layout(location = 9) out uint instanceID;

layout(std430, binding=1) buffer ReadBuffer{
    Cell readCells[];
};

uvec3 to3D(uint index){
    return uvec3(index % tankSize.x, (index / tankSize.x) % tankSize.y, index / (tankSize.x * tankSize.y));
}

void main() {
    float fluidVolume = readCells[gl_InstanceID].fluidVolume;
    vec3 cellPosition = to3D(gl_InstanceID) + vec3(0.5, 0.0, 0.5);
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
    fragColor = color;
    fragView = View;
    fragCameraPosition = cameraPosition;
    instanceID = gl_InstanceID;
}
