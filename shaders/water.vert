#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

void main() {
    TexCoords = aTexCoords;
    vec3 pos = aPos;

    float wave1 = sin(pos.x * 4.0 + time * 2.0) * 0.05;
    float wave2 = sin(pos.z * 3.0 + time * 1.5) * 0.04;

    pos.y += wave1 + wave2;

    FragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * model * vec4(pos, 1.0);
}