#version 430

struct Cell{
    float fluidVolume;
    float solidVolume;
    float fluidHorizontalLefover;
    uint stable;
};

layout(points) in;
//layout(points, max_vertices = 24) out;
layout(triangle_strip, max_vertices = 24) out;

layout(location = 0) in vec3 Position[];
layout(location = 1) in vec4 Color[];
layout(location = 4) in vec3 CameraPosition[];
layout(location = 5) in mat4 View[];
layout(location = 9) in uint instanceID[];

uniform mat4 Model;

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec4 fragColor;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec3 fragNormal;
layout(location = 4) out vec3 fragCameraPosition;
layout(location = 5) out mat4 fragView;

layout(std430, binding=1) buffer ReadBuffer{
    Cell readCells[];
};

vec3 cubeVertices[] = {
{ 0.0, 0.0, 0.0 },
{ 0.0, 1.0, 0.0 },
{ 1.0, 0.0, 0.0 },
{ 1.0, 0.0, -1.0 },
{ 1.0, 1.0, -1.0 },
{ 0.0, 0.0, -1.0 },
{ 0.0, 1.0, -1.0 },
{ 0.0, 0.0, -1.0 },
{ 0.0, 0.0, 0.0 },

{ 0.0, 1.0, 0.0 },
{ 0.0, 1.0, -1.0 },
{ 1.0, 1.0, 0.0 },
{ 1.0, 1.0, -1.0 },

{ 0.0, 0.0, 0.0 },
{ 0.0, 0.0, -1.0 },
{ 1.0, 0.0, 0.0 },
{ 1.0, 0.0, -1.0 },
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
vec3(1.00000, 0.000000, -1.00000),
vec3(1.00000, 0.000000, 1.00000),
vec3(-1.00000, 0.000000, 1.00000),
vec3(-1.00000, 0.000000, -1.00000),
vec3(1.00000, 1.000000, -1.00000),
vec3(1.00000, 1.000000, 1.00000),
vec3(-1.00000, 1.000000, 1.00000),
vec3(-1.00000, 1.000000, -1.00000)
};
const uvec3 cube_index_table[12] = {
uvec3(1, 3, 0),
uvec3(7, 5, 4),
uvec3(4, 1, 0),
uvec3(5, 2, 1),
uvec3(2, 7, 3),
uvec3(0, 7, 4),
uvec3(1, 2, 3),
uvec3(7, 6, 5),
uvec3(4, 5, 1),
uvec3(5, 6, 2),
uvec3(2, 6, 7),
uvec3(0, 3, 7)
};



void main() {
    if (readCells[instanceID[0]].fluidVolume > 0.0){
        fragColor = Color[0];
        fragTexCoord = texCoords[0];
        fragNormal = normals[0];
        fragCameraPosition = CameraPosition[0];
        fragView = View[0];
        for (uint i = 0; i < 12; ++i) {
            for (uint j = 0; j < 3; ++j) {
                gl_Position = vec4(gl_in[0].gl_Position.xyz + cube_vec_table[cube_index_table[i][j]], 1.0);
                fragPosition = Position[0] + cube_vec_table[cube_index_table[i][j]];
                EmitVertex();
            }
        }
        EndPrimitive();
    }

    /*if (readCells[instanceID[0]].fluidVolume > 0.0){
        fragColor = Color[0];
        gl_Position = gl_in[0].gl_Position;
        fragPosition = Position[0];
        fragTexCoord = texCoords[0];
        fragNormal = normals[0];
        fragCameraPosition = CameraPosition[0];
        fragView = View[0];
        EmitVertex();
        EndPrimitive();
    }*/
}
