#version 450

struct Cell{
    float fluidVolume;
    float solidVolume;
    float fluidHorizontalLefover;
    uint stable;
};

layout(points) in;
layout(points, max_vertices = 24) out;

layout(location = 0) in vec3 Position[];
layout(location = 1) in vec4 Color[];
layout(location = 4) in vec3 CameraPosition[];
layout(location = 5) in mat4 View[];
layout(location = 9) in uint instanceID[];

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


void main() {
    if (readCells[instanceID[0]].fluidVolume > 0.0){
        fragColor = Color[0];
        gl_Position = gl_in[0].gl_Position + vec4(box[0], 0.0);
        fragPosition = Position[0];
        fragTexCoord = texCoords[0];
        fragNormal = normals[0];
        fragCameraPosition = CameraPosition[0];
        fragView = View[0];
        EmitVertex();
        EndPrimitive();
    }
}
