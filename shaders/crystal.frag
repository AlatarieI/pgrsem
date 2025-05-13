#version 330 core

struct MaterialTexture {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    sampler2D height;
};

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;

uniform MaterialTexture materialTexture1;

uniform float time;

uniform vec3 viewPos;

uniform sampler2D fogTex;
uniform bool useFog;


vec4 applyFog(vec3 fragColor, vec3 fogColor);

void main() {
    vec3 color = texture(materialTexture1.diffuse, TexCoords).rgb;
    if (useFog)
        FragColor = applyFog(color, vec3(0.6, 0.7, 0.75));
    else
        FragColor = vec4(color, 0.75);
}

vec4 applyFog(vec3 fragColor, vec3 fogColor) {
    float distance = length(FragPos - viewPos);

    float expFog = 1.0 - exp(-pow(0.05 * distance, 2.0));

    vec2 uv = FragPos.xz * 0.007 + vec2(time * 0.01, 0.0);
    uv = uv - vec2(0.0f, 0.5f);
    float textureFog = texture(fogTex, uv).r;

    float fogAmount = min(1.0, expFog + textureFog);

    return vec4(mix(fragColor, fogColor, fogAmount),0.7);
}