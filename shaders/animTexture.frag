#version 330 core

struct MaterialTexture {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    sampler2D height;
};

out vec4 FragColor;

in vec2 TexCoords;

uniform MaterialTexture materialTexture1;

uniform float time;
uniform int frameCountX;
uniform int frameCountY;
uniform float frameRate;

void main() {
    int totalFrames = frameCountX * frameCountY;
    int frameIndex = int(time * frameRate) % totalFrames;

    int col = frameIndex % frameCountX;
    int row = frameIndex / frameCountX;
    row = frameCountX - 1 - row;

    vec2 frameSize = vec2(1.0 / frameCountX, 1.0 / frameCountY);
    vec2 offset = vec2(col, row) * frameSize;

    vec2 uv = offset + TexCoords * frameSize;

    FragColor = texture(materialTexture1.diffuse, uv);
}