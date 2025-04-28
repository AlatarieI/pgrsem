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

uniform float time;             // in seconds
uniform int frameCountX;        // number of columns
uniform int frameCountY;        // number of rows
uniform float frameRate;        // frames per second

void main() {
    // Calculate current frame index
    int totalFrames = frameCountX * frameCountY;
    int frameIndex = int(time * frameRate) % totalFrames;

    // Frame row & column
    int col = frameIndex % frameCountX;
    int row = frameIndex / frameCountX;
    row = frameCountX - 1 - row;

    // Calculate UV offset and scale
    vec2 frameSize = vec2(1.0 / frameCountX, 1.0 / frameCountY);
    vec2 offset = vec2(col, row) * frameSize;

    // Adjust the UV
    vec2 uv = offset + TexCoords * frameSize;

    FragColor = texture(materialTexture1.diffuse, uv);
}