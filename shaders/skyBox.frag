#version 330 core
in vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube skyTexture1;
uniform samplerCube skyTexture2;

uniform float blend;

void main() {
    vec4 tex1 = texture(skyTexture1, TexCoords);
    vec4 tex2 = texture(skyTexture2, TexCoords);
    FragColor = mix(tex1, tex2, blend);
}