#version 430

#define NO_FLAG 0
#define CELL_SOLID 1
#define CELL_SOURCE 2
#define CELL_SINK 4
#define FLOW_DOWN 8

struct CellData{
    float fluidVolume;
    vec4 velocity;
};

struct CellInfo{
    float fluidVerticalLefover;
    int flags;
};

layout(points) in;
layout(triangle_strip, max_vertices = 36) out;

layout(location = 0) in vec3 Position[];
layout(location = 1) in vec4 Color[];
layout(location = 4) in vec3 CameraPosition[];
layout(location = 5) in mat4 View[];
uniform mat4 Projection;
layout(location = 8) uniform float cellSize;
layout(location = 9) in uint instanceID[];

uniform mat4 Model;
uniform uvec3 tankSize;

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec4 fragColor;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec3 fragNormal;
layout(location = 4) out vec3 fragCameraPosition;

layout(std430, binding=1) buffer ReadBuffer{
    CellData readCells[];
};

layout(std430, binding=2) buffer InfoBuffer{
    CellInfo infoCells[];
};

vec3 normals[] = {
{ 0.0, 0.0, 1.0 },
{ 1.0, 0.0, 0.0 },
{ 0.0, 0.0, -1.0 },
{ -1.0, 0.0, 0.0 },

{ 0.0, 1.0, 0.0 },
{ 0.0, -1.0, 0.0 }
};

vec2 texCoords[1] = {
{ 1.0, 1.0 }
};

const vec3 cube_vec_table[8] = {
vec3(1.00000, 0.000000, 0),
vec3(1.00000, 0.000000, 1.00000),
vec3(0, 0.000000, 1.00000),
vec3(0, 0.000000, 0),
vec3(1.00000, 1.000000, 0),
vec3(1.00000, 1.000000, 1.00000),
vec3(0, 1.000000, 1.00000),
vec3(0, 1.000000, 0)
};
const uvec3 cube_index_table[12] = {
uvec3(0, 1, 3),
uvec3(4, 7, 5),
uvec3(0, 4, 1),
uvec3(1, 5, 2),
uvec3(3, 2, 7),
uvec3(4, 0, 7),
uvec3(3, 1, 2),
uvec3(5, 7, 6),
uvec3(1, 4, 5),
uvec3(2, 5, 6),
uvec3(7, 2, 6),
uvec3(7, 0, 3)
};

float[8] avgVolume() {
    float result[8] = { 1, 1, 1, 1, 0.f, 0.f, 0.f, 0.f };
    if (bool(infoCells[instanceID[0]].flags & (CELL_SOLID | FLOW_DOWN))){
        float cube[8] = { 1, 1, 1, 1, 1.f, 1.f, 1.f, 1.f };
        return cube;
    }
    uint zOffset = tankSize.x * tankSize.y;
    result[5] = readCells[instanceID[0]].fluidVolume + readCells[instanceID[0] + 1u].fluidVolume
    + readCells[instanceID[0] + 1u + zOffset].fluidVolume
    + readCells[instanceID[0] + zOffset].fluidVolume;
    result[5] *= float(readCells[instanceID[0]].fluidVolume > 0) * float(readCells[instanceID[0] + 1u].fluidVolume > 0)
    + float(readCells[instanceID[0] + 1u + zOffset].fluidVolume > 0)
    + float(readCells[instanceID[0] + zOffset].fluidVolume > 0);

    result[4] = readCells[instanceID[0]].fluidVolume + readCells[instanceID[0] + 1u].fluidVolume
    + readCells[instanceID[0] + 1u - zOffset].fluidVolume
    + readCells[instanceID[0] - zOffset].fluidVolume;
    result[4] *= float(readCells[instanceID[0]].fluidVolume > 0) * float(readCells[instanceID[0] + 1u].fluidVolume > 0)
    + float(readCells[instanceID[0] + 1u - zOffset].fluidVolume > 0)
    + float(readCells[instanceID[0] - zOffset].fluidVolume > 0);

    result[6] = readCells[instanceID[0]].fluidVolume + readCells[instanceID[0] - 1u].fluidVolume
    + readCells[instanceID[0] - 1u + zOffset].fluidVolume
    + readCells[instanceID[0] + zOffset].fluidVolume;
    result[6] *= float(readCells[instanceID[0]].fluidVolume > 0) * float(readCells[instanceID[0] - 1u].fluidVolume > 0)
    + float(readCells[instanceID[0] - 1u + zOffset].fluidVolume > 0)
    + float(readCells[instanceID[0] + zOffset].fluidVolume > 0);

    result[7] = readCells[instanceID[0]].fluidVolume + readCells[instanceID[0] - 1u].fluidVolume
    + readCells[instanceID[0] - 1u - zOffset].fluidVolume
    + readCells[instanceID[0] - zOffset].fluidVolume;
    result[7] *= float(readCells[instanceID[0]].fluidVolume > 0) * float(readCells[instanceID[0] - 1u].fluidVolume > 0)
    + float(readCells[instanceID[0] - 1u - zOffset].fluidVolume > 0)
    + float(readCells[instanceID[0] - zOffset].fluidVolume > 0);
    for (uint i = 4; i < 8; ++i) {
        result[i] =  result[i] / 4.f;
    }

    return result;
}


void main() {
    if (readCells[instanceID[0]].fluidVolume > 0.0 || bool(infoCells[instanceID[0]].flags & CELL_SOLID)){
        float vols[8] = avgVolume();
        for (uint i = 0; i < 12; ++i) {
            vec4 poly[3];
            vec3 fragy[3];
            for (uint j = 0; j < 3; ++j) {
                const float sizeModifier = cellSize;
                vec3 cubeVertex = sizeModifier * cube_vec_table[cube_index_table[i][j]];

                cubeVertex.y = cubeVertex.y * vols[cube_index_table[i][j]];
                poly[j] = Projection * View[0] * Model * vec4(gl_in[0].gl_Position.xyz + cubeVertex, gl_in[0].gl_Position.w);
                fragy[j] = Position[0] + cubeVertex;
            }
            fragColor = Color[0];
            if (bool(infoCells[instanceID[0]].flags & CELL_SOLID)){
                fragColor = vec4(0.0, 1.0, 0.0, 1.0);
            }
            fragTexCoord = texCoords[0];
            fragCameraPosition = CameraPosition[0];
            fragNormal = -cross(fragy[2] - fragy[0], fragy[1] - fragy[0]);
            for (uint j = 0; j < 3; ++j) {
                gl_Position = poly[j];
                fragPosition = fragy[j];
                EmitVertex();
            }
            EndPrimitive();
        }
    }
}
