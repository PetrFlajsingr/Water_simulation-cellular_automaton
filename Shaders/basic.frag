#version 450

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec4 fragColor;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 fragNormal;
layout(location = 4) in vec3 fragCameraPosition;
layout(location = 10) in float fluidVolume;
layout(location = 11) uniform int showIntensity;

layout(location = 5) uniform mat4 View;



float hash(float n) { return fract(sin(n) * 1e4); }
float hash(vec2 p) { return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }

float noise(vec3 x) {
    const vec3 step = vec3(110, 241, 171);

    vec3 i = floor(x);
    vec3 f = fract(x);

    float n = dot(i, step);

    vec3 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(mix(hash(n + dot(step, vec3(0, 0, 0))), hash(n + dot(step, vec3(1, 0, 0))), u.x),
    mix(hash(n + dot(step, vec3(0, 1, 0))), hash(n + dot(step, vec3(1, 1, 0))), u.x), u.y),
    mix(mix(hash(n + dot(step, vec3(0, 0, 1))), hash(n + dot(step, vec3(1, 0, 1))), u.x),
    mix(hash(n + dot(step, vec3(0, 1, 1))), hash(n + dot(step, vec3(1, 1, 1))), u.x), u.y), u.z);
}

vec3 random3(vec3 c) {
    float j = 4096.0*sin(dot(c,vec3(17.0, 59.4, 15.0)));
    vec3 r;
    r.z = fract(512.0*j);
    j *= .125;
    r.x = fract(512.0*j);
    j *= .125;
    r.y = fract(512.0*j);
    return r-0.5;
}

const float F3 =  0.3333333;
const float G3 =  0.1666667;
float snoise(vec3 p) {

    vec3 s = floor(p + dot(p, vec3(F3)));
    vec3 x = p - s + dot(s, vec3(G3));

    vec3 e = step(vec3(0.0), x - x.yzx);
    vec3 i1 = e*(1.0 - e.zxy);
    vec3 i2 = 1.0 - e.zxy*(1.0 - e);

    vec3 x1 = x - i1 + G3;
    vec3 x2 = x - i2 + 2.0*G3;
    vec3 x3 = x - 1.0 + 3.0*G3;

    vec4 w, d;

    w.x = dot(x, x);
    w.y = dot(x1, x1);
    w.z = dot(x2, x2);
    w.w = dot(x3, x3);

    w = max(0.6 - w, 0.0);

    d.x = dot(random3(s), x);
    d.y = dot(random3(s + i1), x1);
    d.z = dot(random3(s + i2), x2);
    d.w = dot(random3(s + 1.0), x3);

    w *= w;
    w *= w;
    d *= w;

    return dot(d, vec4(52.0));
}

uniform float time;
float snoiseFractal(vec3 m) {
    float value = 0;
    float amplitude = 1;
    for (int i = 0; i < 5; i++) {
        value += amplitude * abs(snoise(m));
        m *= 2.;
        amplitude *= .5;
    }
    return value;
    m.y += time / 20;
    vec3 tmp = m;
    //tmp.x += time*2;
    return  (
    //0.5333333* snoise(m)
    //+0.2666667* snoise(2.0*m)
    +0.1333333* snoise(4.0*tmp)
    +0.0666667* snoise(8.0*tmp)
    +0.0666667/2* snoise(16.0*m)
)
    ;
}

vec2 random2( vec2 p ) {
    return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
}

float voronoi(vec3 Position) {
    vec2 st = Position.xz;
   // st.x -= floor(st.x);
    //st.y -= floor(st.y);

    // Scale
    st *= 5.;

    // Tile the space
    vec2 i_st = floor(st);
    vec2 f_st = fract(st);

    float m_dist = 10.;  // minimun distance
    vec2 m_point;        // minimum point

    for (int j=-1; j<=1; j++ ) {
        for (int i=-1; i<=1; i++ ) {
            vec2 neighbor = vec2(float(i),float(j));
            vec2 point = random2(i_st + neighbor);
            point = 0.5 + 0.5*sin(time*100 + 6.2831*point);
            vec2 diff = neighbor + point - f_st;
            float dist = length(diff);

            if( dist < m_dist ) {
                m_dist = dist;
                m_point = point;
            }
        }
    }
    return -m_dist;
}



vec3 getNormal(vec3 Position) {
    //Position.y = 0;
    float d = 1.0/(100);
    vec3 grad;
    float di = 1 + 0.00001*time;
    grad.x = snoiseFractal(Position.xyz + vec3(d, 0, 0)) / di - snoiseFractal(Position.xyz + vec3(-d, 0, 0) / di);
    grad.y = snoiseFractal(Position.xyz + vec3(0, d, 0)) / di - snoiseFractal(Position.xyz + vec3(0, -d, 0) / di);
    grad.z = snoiseFractal(Position.xyz + vec3(0, 0, d)) / di - snoiseFractal(Position.xyz + vec3(0, 0, -d) / di);

    return normalize(grad);
}

void main() {
    const float ambientStrength = 0.1f;
    const vec3 lightColor = vec3(1.0f);
    const vec3 lightPosition = vec3(15.0f, 15.0f, 15.0f);
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(fragNormal);
    norm = fluidVolume > 0 ? (13*norm + getNormal(fragPosition/5)) / 14f : norm;
    norm = normalize(norm);
    vec3 lightDirection = normalize(lightPosition - fragPosition);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = 0.7 * diff * lightColor;

    float specularStrength = 0.5f;
    vec3 viewDir = normalize(fragCameraPosition - fragPosition);
    vec3 reflectDir = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;


    vec3 color = fluidVolume >= 1.0f && bool(showIntensity) ? vec3(0, 1, 0) : fragColor.rgb;

    vec3 result = (ambient + diffuse + specular) * (color);

    if (fluidVolume >= 0 && bool(showIntensity)) {
        float fraction = 1f - fract(fluidVolume);
        result.r += fraction;
    }
    gl_FragColor = vec4(result, fragColor.a);
}
