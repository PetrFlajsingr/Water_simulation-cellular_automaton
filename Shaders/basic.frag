#version 450

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec3 fragColor;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 fragNormal;

void main() {
    const float ambientStrength = 0.5f;
    const vec3 lightColor = vec3(1.0f);
    const vec3 lightPosition = vec3(5.0f, 5.0f, 5.0f);
    vec3 ambient = ambientStrength * lightColor;
    vec3 norm = normalize(fragNormal);
    vec3 lightDirection = normalize(lightPosition - fragPosition);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = ambient * fragColor;
    gl_FragColor = vec4(result, 1.0);
}
