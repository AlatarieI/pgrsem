#version 330 core

out vec4 FragColor;

uniform float id;

void main() {
    FragColor = vec4(id/255, 1.0, 1.0 ,1.0);
}