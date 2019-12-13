#version 450

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec3 fragColor;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 fragNormal;
layout(location = 4) in vec3 fragCameraPosition;
layout(location = 5) in mat4 fragView;


void main() {
    const float ambientStrength = 0.1f;
    const vec3 lightColor = vec3(1.0f);
    const vec3 lightPosition = vec3(15.0f, 15.0f, 15.0f);
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(fragNormal);
    vec3 lightDirection = normalize(lightPosition - fragPosition);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5f;
    vec3 viewDir = normalize(fragCameraPosition - fragPosition);
    vec3 reflectDir = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * fragColor;
    gl_FragColor = vec4(result, 1.0);
}
