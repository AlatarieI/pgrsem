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

uniform mat3 texTransform; // Transformation matrix for UVs

void main() {
    // Convert UV to vec3 for matrix multiplication (homogeneous coords)
    vec3 color = texture(materialTexture1.diffuse, (texTransform * vec3(TexCoords, 1.0)).xy).rgb;
    FragColor = vec4(color, 0.75);
}