#version 450

#define NO_FLAG 0
#define CELL_SOLID 1
#define CELL_SOURCE 2
#define CELL_SINK 4
#define FLOW_DOWN 8

struct CellData{
    vec3 velocity;
    float fluidVolume;
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
    CellData readCells[];
};

uvec3 to3D(uint index){
    return uvec3(index % tankSize.x, (index / tankSize.x) % tankSize.y, index / (tankSize.x * tankSize.y));
}

void main() {
    float fluidVolume = readCells[gl_InstanceID].fluidVolume;
    vec3 cellPosition = to3D(gl_InstanceID);
    vec3 vertexPos = inPosition.xyz;
    vec4 color = vec4(inColor, 1.0);
    vec4 pos = vec4(vertexPos + cellPosition, 1.0);
    pos.xyz *= cellSize;
    fragPosition = (Model * pos).xyz;
    gl_Position = pos;
    fragColor = color;
    fragView = View;
    fragCameraPosition = cameraPosition;
    instanceID = gl_InstanceID;
}
