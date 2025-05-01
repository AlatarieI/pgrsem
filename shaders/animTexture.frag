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
uniform int frameCountX;
uniform int frameCountY;
uniform float frameRate;

uniform vec3 viewPos;

uniform sampler2D fogTex;
uniform bool useFog;

vec3 applyFog(vec3 fragColor, vec3 fogColor);

void main() {
    int totalFrames = frameCountX * frameCountY;
    int frameIndex = int(time * frameRate) % totalFrames;

    int col = frameIndex % frameCountX;
    int row = frameIndex / frameCountX;
    row = frameCountX - 1 - row;

    vec2 frameSize = vec2(1.0 / frameCountX, 1.0 / frameCountY);
    vec2 offset = vec2(col, row) * frameSize;

    vec2 uv = offset + TexCoords * frameSize;

    vec4 textured = texture(materialTexture1.diffuse, uv);
    if (useFog)
        FragColor = vec4(applyFog(textured.rgb, vec3(0.6, 0.7, 0.75)), textured.a);
    else
        FragColor = textured;
}

vec3 applyFog(vec3 fragColor, vec3 fogColor) {
    float distance = length(FragPos - viewPos);

    float expFog = 1.0 - exp(-pow(0.05 * distance, 2.0));

    vec2 uv = FragPos.xz * 0.007 + vec2(time * 0.01, 0.0);
    float textureFog = texture(fogTex, uv).r;

    float fogAmount = min(1.0, expFog + textureFog);

    return mix(fragColor, fogColor, fogAmount);
}